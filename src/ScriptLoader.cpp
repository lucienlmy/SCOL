#include "ScriptLoader.hpp"
#include "Natives.hpp"
#include "Pointers.hpp"
#include "ScriptFile.hpp"
#include "ScriptFunction.hpp"

namespace SCOL
{
    std::string ScriptLoader::RemoveAllExtensions(const std::filesystem::path& path)
    {
        std::string name = path.filename().string();
        while (!std::filesystem::path(name).extension().empty())
            name = std::filesystem::path(name).stem().string();
        return name;
    }

    ScriptLoader::ScriptType ScriptLoader::GetScriptType(const std::filesystem::path& path)
    {
        auto filename = path.filename().string();

        if (filename.ends_with(".sco"))
            return ScriptType::DEFAULT;
        else if (filename.ends_with(".ysc"))
            return ScriptType::STREAMED;
        else if (filename.ends_with(".ysc.full"))
            return ScriptType::STREAMED_FULL;

        return ScriptType::INVALID;
    }

    std::uint32_t ScriptLoader::LoadDefaultScript(const std::filesystem::path& path, void* args, std::uint32_t argCount, std::uint32_t stackSize)
    {
        if (auto programHash = g_Pointers.LoadScriptProgram(path.string().c_str(), path.stem().string().c_str()))
        {
            if (auto program = rage::scrProgram::GetByHash(programHash))
            {
                program->m_RefCount--; // Due to the same reason explained in LoadStreamedFullScript.
                return g_Pointers.StartNewGtaThread(programHash, args, argCount * sizeof(rage::scrValue), stackSize);
            }
        }

        return 0;
    }

    // TO-DO: Invalidate the script if it is already registered
    std::uint32_t ScriptLoader::LoadStreamedScript(const std::filesystem::path& path, void* args, std::uint32_t argCount, std::uint32_t stackSize)
    {
        std::uint32_t index = 0xFFFFFFFF;
        g_Pointers.RegisterIndividualFile(&index, path.string().c_str(), true, path.filename().string().c_str(), false, false);

        if (index != 0xFFFFFFFF)
        {
            g_Pointers.RequestStreamedObject(g_Pointers.StreamingEngineInfo, index, 21);
            g_Pointers.LoadAllStreamedObjects(g_Pointers.StreamingEngineLoader, true);
            m_strIndices.push_back({index, path.filename().string()});

            return g_Pointers.StartNewGtaThread(Joaat(path.stem().string().c_str()), args, argCount * sizeof(rage::scrValue), stackSize);
        }

        return 0;
    }

    std::uint32_t ScriptLoader::LoadStreamedFullScript(const std::filesystem::path& path, void* args, std::uint32_t argCount, std::uint32_t stackSize)
    {
        auto scName = RemoveAllExtensions(path);
        auto program = rage::scrProgram::GetByHash(Joaat(scName));
        if (program)
            return g_Pointers.StartNewGtaThread(program->m_NameHash, args, argCount * sizeof(rage::scrValue), stackSize);

        ScriptFile script(path);
        if (!script.IsValid())
            return 0;

        program = reinterpret_cast<rage::scrProgram*>(g_Pointers.rage_new(sizeof(rage::scrProgram), 16, 0, 0));
        if (!program)
            return 0;

        auto& header = script.GetHeader();
        auto globalCount = header.GlobalCountAndBlock & 0x3FFFF;
        auto globalBlock = header.GlobalCountAndBlock >> 0x12;

        g_Pointers.ScriptProgramCtor(
            program,
            script.GetName().c_str(),
            header.CodeSize,
            header.NativeCount,
            header.StaticCount,
            globalCount,
            globalBlock,
            header.StringsSize,
            header.ArgCount,
            header.GlobalVersion);

        auto& code = script.GetCode();
        for (std::uint32_t i = 0; i < header.CodeSize; i++)
            *program->GetCode(i) = code[i];

        auto& statics = script.GetStatics();
        for (std::uint32_t i = 0; i < header.StaticCount; i++)
            program->m_Statics[i].Any = statics[i];

        auto& globals = script.GetGlobals();
        for (std::uint32_t i = 0; i < globalCount; i++)
            program->GetGlobal(i)->Any = globals[i];
        if (globalCount > 0)
            g_Pointers.AllocateGlobalBlock(program);

        auto& natives = script.GetNatives();
        for (std::uint32_t i = 0; i < header.NativeCount; i++)
            program->m_Natives[i] = natives[i];
        g_Pointers.InitNativeTables(program);

        std::uint32_t offset = 0;
        auto& strings = script.GetStrings();
        for (std::size_t i = 0; i < strings.size(); i++)
        {
            auto& str = strings[i];
            std::memcpy((void*)program->GetString(offset), str.c_str(), str.size() + 1);
            offset += str.size() + 1;
        }

        rage::scrProgram::Add(program);

        auto id = g_Pointers.StartNewGtaThread(program->m_NameHash, args, argCount * sizeof(rage::scrValue), stackSize);

        // At this point, the program has two references:
        // 1. Added by the constructor
        // 2. Added by scrThread::CreateThread via StartNewGtaThread
        // We decrement once here so the ref count becomes 1.
        // When scrThread::Kill is called, the count will drop to 0 and the program will be freed.
        program->m_RefCount--;
        return id;
    }

    void ScriptLoader::CleanupScripts()
    {
        for (auto id : m_ThreadIds)
        {
            if (auto thread = rage::scrThread::GetById(id))
            {
                if (auto data = Settings::GetScriptData(thread->m_ScriptName); data.CleanupFunction != 0)
                    ScriptFunction::Call(thread->m_ScriptHash, data.CleanupFunction); // We assume the function doesn't take any arguments. Return type doesn't matter.

                // Even if a script calls TERMINATE_THIS_THREAD (which internally calls scrThread::Kill),
                // it only sets the thread state to KILLED if the thread is the current thread and does
                // not release the script program. We call Kill here to ensure that the program is
                // released, so that AllocateGlobalBlock is called for the next load, which we need in order to reset globals.
                thread->Kill();
                LOGF(INFO, "Killed thread with ID {}.", id);
            }
        }

        for (auto idx : m_strIndices)
        {
            g_Pointers.ClearRequiredFlag(g_Pointers.StreamingEngineInfo, idx.first, 17);
            g_Pointers.RemoveStreamedObject(g_Pointers.StreamingEngineInfo, idx.first, false);
            g_Pointers.UnregisterStreamedObject(g_Pointers.StreamingEngineInfo, idx.first);
            g_Pointers.InvalidateIndividualFile(idx.second.c_str());
        }

        m_ThreadIds.clear();
        m_strIndices.clear();
    }

    std::uint32_t ScriptLoader::LoadScriptImpl(const std::filesystem::path& path, void* args, std::uint32_t argCount, std::uint32_t stackSize)
    {
        std::uint32_t id = 0;

        auto type = GetScriptType(path);
        switch (type)
        {
        case ScriptType::DEFAULT:
            id = LoadDefaultScript(path, args, argCount, stackSize);
            break;
        case ScriptType::STREAMED:
            id = LoadStreamedScript(path, args, argCount, stackSize);
            break;
        case ScriptType::STREAMED_FULL:
            id = LoadStreamedFullScript(path, args, argCount, stackSize);
            break;
        case ScriptType::INVALID:
            break;
        }

        if (id != 0)
        {
            if (auto thread = rage::scrThread::GetById(id))
                Natives::CleanupScriptResources(thread->m_ScriptHash);
        }

        return id; // Don't push this to m_ThreadIds yet, we don't want to allow reloading script overrides.
    }

    void ScriptLoader::LoadScriptsImpl()
    {
        CleanupScripts();

        auto scriptsFolder = std::filesystem::absolute(g_Variables.ScriptsFolder);

        if (!std::filesystem::exists(scriptsFolder) || !std::filesystem::is_directory(scriptsFolder))
        {
            LOGF(WARNING, "Scripts folder is invalid: {}", scriptsFolder.string());
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(scriptsFolder))
        {
            if (!entry.is_regular_file())
                continue;

            if (auto type = GetScriptType(entry.path()); type == ScriptType::INVALID)
                continue;

            auto name = RemoveAllExtensions(entry.path());
            auto data = Settings::GetScriptData(name);
            auto argCount = static_cast<uint32_t>(data.Args.size());
            LOGF(INFO, "Loaded data for script '{}'. ArgCount={}, StackSize={}, CleanupFunction=0x{:X}", name, argCount, data.StackSize, data.CleanupFunction);

            if (auto id = LoadScript(entry.path(), argCount ? data.Args.data() : nullptr, argCount, data.StackSize))
            {
                m_ThreadIds.push_back(id);
                LOGF(INFO, "Started new thread with ID {}.", id);
            }
        }
    }

    std::filesystem::path ScriptLoader::GetScriptOverridePathImpl(uint32_t hash)
    {
        auto scriptOverridesFolder = std::filesystem::absolute(g_Variables.ScriptOverridesFolder);

        if (!std::filesystem::exists(scriptOverridesFolder) || !std::filesystem::is_directory(scriptOverridesFolder))
            return {};

        for (const auto& entry : std::filesystem::directory_iterator(scriptOverridesFolder))
        {
            if (!entry.is_regular_file())
                continue;

            if (auto type = GetScriptType(entry.path()); type == ScriptType::INVALID)
                continue;

            auto name = RemoveAllExtensions(entry.path());
            if (Joaat(name) == hash)
                return entry.path();
        }

        return {};
    }
}