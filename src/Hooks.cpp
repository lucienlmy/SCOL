#include "Hooks.hpp"
#include "Pointers.hpp"
#include "ScriptFiber.hpp"
#include "ScriptLoader.hpp"

namespace SCOL
{
    static LRESULT WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
    {
        if (umsg == WM_KEYUP && wparam == g_Variables.ReloadKey)
            g_LoadRequested = true;

        return g_Hooks.WndProc.call<LRESULT>(hwnd, umsg, wparam, lparam);
    }

    static bool UpdateScriptThreads(std::uint32_t insnCount)
    {
        auto ret = g_Hooks.UpdateScriptThreads.call<bool>(insnCount);

        ScriptFiber::Tick();
        return ret;
    }

    static void AllocateGlobalBlock(rage::scrProgram* program)
    {
        const auto block = program->GetGlobalBlock();
        if (g_Pointers.ScriptGlobals[block] != nullptr)
        {
            LOGF(INFO, "Global block {} has already been allocated, freeing it.", block);

            g_Pointers.sysVirtualFree(g_Pointers.ScriptGlobals[block]);
            g_Pointers.ScriptGlobals[block] = nullptr;
        }

        g_Hooks.AllocateGlobalBlock.call<void>(program);
    }

    static uint32_t StartNewGtaThread(uint32_t programHash, void* args, uint32_t argCount, uint32_t stackSize)
    {
        if (auto path = ScriptLoader::GetScriptOverridePath(programHash); !path.empty())
        {
            if (auto program = rage::scrProgram::GetByHash(programHash))
            {
                auto destructor = *(*reinterpret_cast<void***>(program) + 6);
                reinterpret_cast<void (*)(rage::scrProgram*, bool)>(destructor)(program, true); // Free the program loaded by natives first, we will create a new one.
            }

            if (auto id = ScriptLoader::LoadScript(path, args, argCount, stackSize))
            {
                LOGF(INFO, "Loaded script override from path '{}'.", path.string().c_str());
                return id;
            }
        }

        return g_Hooks.StartNewGtaThread.call<uint32_t>(programHash, args, argCount, stackSize);
    }

    void Hooks::Init()
    {
        static auto CreateInline = [](const char* name, SafetyHookInline& inlineHook, void* target, void* destination) {
            inlineHook = safetyhook::create_inline(target, destination);
            LOGF(INFO, "Created inline hook for {}.", name);
        };

        CreateInline("WndProc", g_Hooks.WndProc, g_Pointers.WndProc, SCOL::WndProc);
        CreateInline("UpdateScriptThreads", g_Hooks.UpdateScriptThreads, g_Pointers.UpdateScriptThreads, SCOL::UpdateScriptThreads);
        CreateInline("AllocateGlobalBlock", g_Hooks.AllocateGlobalBlock, g_Pointers.AllocateGlobalBlock, SCOL::AllocateGlobalBlock);
        CreateInline("StartNewGtaThread", g_Hooks.StartNewGtaThread, g_Pointers.StartNewGtaThread, SCOL::StartNewGtaThread);
    }
}