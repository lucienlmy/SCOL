#include "Natives.hpp"
#include "Pointers.hpp"
#include "Scanner.hpp"
#include "ScriptFunction.hpp"
#include <nlohmann/json.hpp>

namespace SCOL::Natives
{
    // TO-DO: If we set a static to a literal string, it will point to invalid memory next time we reload the script as our string table will be freed.
    // NOTE: Globals cannot be declared as STRING, so we don't have SET/GET methods for them.

    // clang-format off

    // Core
    static constexpr std::uint64_t LOG_TO_FILE                                       = 0x7F41C15A89FDEE9F;
    static constexpr std::uint64_t CLEAR_LOG_FILE                                    = 0x8E45F12A1A2B3C4D;
    static constexpr std::uint64_t REGISTER_INDIVIDUAL_FILE                          = 0x383C067F9AA5DBC9;
    static constexpr std::uint64_t INVALIDATE_INDIVIDUAL_FILE                        = 0xDCAC1A79714643D7;

    // JSON
    static constexpr std::uint64_t JSON_SET_INT                                      = 0x669CB77130D296AD;
    static constexpr std::uint64_t JSON_SET_BOOL                                     = 0xF800174D11572F70;
    static constexpr std::uint64_t JSON_SET_FLOAT                                    = 0x851E3C7AB9BB63E5;
    static constexpr std::uint64_t JSON_SET_STRING                                   = 0xDA71BF7EDA3632C8;
    static constexpr std::uint64_t JSON_GET_INT                                      = 0xCA18D380BB30B3EE;
    static constexpr std::uint64_t JSON_GET_BOOL                                     = 0xB343A4FE9DC54F35;
    static constexpr std::uint64_t JSON_GET_FLOAT                                    = 0xEC9BEF67DB133874;
    static constexpr std::uint64_t JSON_GET_STRING                                   = 0x77A722DEBCB308CB;

    // Memory
    static constexpr std::uint64_t MEMORY_SCAN_PATTERN                               = 0x0E7D68BA1B32BA2A;
    static constexpr std::uint64_t MEMORY_ADD                                        = 0x043339EE866586BD;
    static constexpr std::uint64_t MEMORY_RIP                                        = 0x6E496FA081015FC3;
    static constexpr std::uint64_t MEMORY_DEREF                                      = 0x9C980189DFBDADAD;
    static constexpr std::uint64_t MEMORY_WRITE                                      = 0xD763E0EF3AFCE986;
    static constexpr std::uint64_t MEMORY_READ                                       = 0x5353EB6CABD2A870;
    static constexpr std::uint64_t MEMORY_WRITE_INT                                  = 0xA1CDE11FA6D2838F;
    static constexpr std::uint64_t MEMORY_READ_INT                                   = 0x1884B5B84D20DEF9;
    static constexpr std::uint64_t MEMORY_WRITE_FLOAT                                = 0xFFD64B1EC611C6DB;
    static constexpr std::uint64_t MEMORY_READ_FLOAT                                 = 0xF2A6DA07E3EE4850;
    static constexpr std::uint64_t MEMORY_WRITE_STRING                               = 0x907338E62DD47ACD;
    static constexpr std::uint64_t MEMORY_READ_STRING                                = 0x959095C3F1E3E44A;
    static constexpr std::uint64_t MEMORY_WRITE_VECTOR                               = 0x077B1F0EEB16F19A;
    static constexpr std::uint64_t MEMORY_READ_VECTOR                                = 0x0DD6010289D2A79C;

    // Script Threads
    static constexpr std::uint64_t SET_CURRENT_SCRIPT_THREAD                         = 0x7AFACDB81809E2C1;
    static constexpr std::uint64_t SET_SCRIPT_THREAD_STATE                           = 0x2B53D1F2FAD6DB0E;

    // Statics
    static constexpr std::uint64_t SCRIPT_STATIC_SET_INT                             = 0x857A2700DC9407CF;
    static constexpr std::uint64_t SCRIPT_STATIC_SET_FLOAT                           = 0xD8AABF55B8C2ABCC;
    static constexpr std::uint64_t SCRIPT_STATIC_SET_STRING                          = 0xBBED7EBD9CB32457;
    static constexpr std::uint64_t SCRIPT_STATIC_SET_TEXT_LABEL                      = 0x82486D97F828522B;
    static constexpr std::uint64_t SCRIPT_STATIC_SET_VECTOR                          = 0x1CBDE8A15884D019;
    static constexpr std::uint64_t SCRIPT_STATIC_GET_INT                             = 0xE60444296FBC9C3C;
    static constexpr std::uint64_t SCRIPT_STATIC_GET_FLOAT                           = 0x02917A468A9F9203;
    static constexpr std::uint64_t SCRIPT_STATIC_GET_STRING                          = 0x50AECCD9E4A23B3A;
    static constexpr std::uint64_t SCRIPT_STATIC_GET_TEXT_LABEL                      = 0x83C64F9F51CCB284;
    static constexpr std::uint64_t SCRIPT_STATIC_GET_VECTOR                          = 0x022347480BDA5340;
    static constexpr std::uint64_t SCRIPT_STATIC_GET_POINTER                         = 0x8B1A5E688A9ABF31;

    // Globals
    static constexpr std::uint64_t SCRIPT_GLOBAL_SET_INT                             = 0xE308F800129466D7;
    static constexpr std::uint64_t SCRIPT_GLOBAL_SET_FLOAT                           = 0xCE23B2BC1A4037EE;
    static constexpr std::uint64_t SCRIPT_GLOBAL_SET_TEXT_LABEL                      = 0xC7540C6E3F588A63;
    static constexpr std::uint64_t SCRIPT_GLOBAL_SET_VECTOR                          = 0x2D670A4779E7390E;
    static constexpr std::uint64_t SCRIPT_GLOBAL_GET_INT                             = 0x95257993CA3F052E;
    static constexpr std::uint64_t SCRIPT_GLOBAL_GET_FLOAT                           = 0xDC21946AD79CBEAF;
    static constexpr std::uint64_t SCRIPT_GLOBAL_GET_TEXT_LABEL                      = 0x79516115C6DB77F2;
    static constexpr std::uint64_t SCRIPT_GLOBAL_GET_VECTOR                          = 0x673E810DC3EC1307;
    static constexpr std::uint64_t SCRIPT_GLOBAL_GET_POINTER                         = 0xDFFE51F613A8E53F;

    // Script Functions (array parameters and text label/struct returns are currently not supported)
    static constexpr std::uint64_t SCRIPT_FUNCTION_BEGIN_CALL                        = 0xC0E22B4A435AC866;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_INT                     = 0x08498C0E9D0B40FF;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_FLOAT                   = 0x77447CC2B714B3CD;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_STRING                  = 0x274DF0AFD6AE55ED;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_VECTOR                  = 0x4C033060974B7B16;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_POINTER                 = 0x6D56C40AE63AF5EB;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_INT           = 0x23C957EA2CAC32C1;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_FLOAT         = 0xD1F101417313D51C;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_TEXT_LABEL_15 = 0x5C322B4B4444B1D5;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_TEXT_LABEL_23 = 0xE4FED9824D4F4506;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_TEXT_LABEL_31 = 0x3A991D3A46ED3946;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_TEXT_LABEL_63 = 0xB8051D0558A34D66;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_VECTOR        = 0x51A946C23ABD71E8;
    static constexpr std::uint64_t SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_STRUCT        = 0xBB8A36B6AF274653;
    static constexpr std::uint64_t SCRIPT_FUNCTION_END_CALL_PROC                     = 0x56A2FF1109E4288A;
    static constexpr std::uint64_t SCRIPT_FUNCTION_END_CALL_INT                      = 0x21448D2E73E6268B;
    static constexpr std::uint64_t SCRIPT_FUNCTION_END_CALL_FLOAT                    = 0xD67665C45FB0350D;
    static constexpr std::uint64_t SCRIPT_FUNCTION_END_CALL_STRING                   = 0xCF772F62E284AC66;
    static constexpr std::uint64_t SCRIPT_FUNCTION_END_CALL_VECTOR                   = 0xBDAC87D43E99E594;

    // clang-format on

    class ScriptFunctionCallContext
    {
        joaat_t m_Hash;
        std::uint32_t m_Pc;
        std::vector<std::uint64_t> m_Params;
        std::uint32_t m_Index;

    public:
        void BeginCall(joaat_t hash, std::uint32_t pc)
        {
            m_Hash = hash;
            m_Pc = pc;
            m_Params.clear();
            m_Index = 0;
        }

        template <typename Param>
        void AddParam(Param&& value)
        {
            if (m_Index >= m_Params.size())
                m_Params.resize(m_Index + 1);

            *reinterpret_cast<std::remove_cv_t<std::remove_reference_t<Param>>*>(m_Params.data() + m_Index) = value;
            m_Index++;
        }

        template <typename Ret>
        Ret EndCall()
        {
            Ret retVal{};
            SCOL::ScriptFunction::CallInternal(m_Hash, m_Pc, m_Params.data(), static_cast<std::uint32_t>(m_Params.size()), &retVal, sizeof(retVal));
            m_Params.clear();
            m_Index = 0;
            return retVal;
        }
    };

    class ScriptJSON
    {
    public:
        template <typename T>
        void Set(const char* section, const char* key, T value)
        {
            m_JSON[section][key] = value;
            Save();
        }

        template <typename T>
        T Get(const char* section, const char* key, T _default)
        {
            auto& sec = m_JSON[section];
            if (!sec.contains(key))
            {
                sec[key] = _default;
                Save();
                return _default;
            }

            try
            {
                return sec[key].get<T>();
            }
            catch (...) // in case the value is malformed
            {
                sec[key] = _default;
                Save();
                return _default;
            }
        }

        static ScriptJSON& GetOrCreate()
        {
            auto thread = *rage::scrThread::TLS::Get()->m_CurrentThread;
            auto hash = thread->m_ScriptHash;
            auto name = thread->m_ScriptName;

            auto it = m_ScriptJSONs.find(hash);
            if (it != m_ScriptJSONs.end())
                return it->second;

            auto path = std::filesystem::path(g_Variables.ScriptsFolder) / (std::string(name) + ".json");

            it = m_ScriptJSONs.emplace(std::make_pair(hash, ScriptJSON(path))).first;
            return it->second;
        }

        static void Remove(joaat_t hash)
        {
            if (auto it = m_ScriptJSONs.find(hash); it != m_ScriptJSONs.end())
                m_ScriptJSONs.erase(it);
        }

    private:
        ScriptJSON(const std::filesystem::path& path)
            : m_Path(path)
        {
            std::ifstream file(m_Path);
            if (file.is_open())
            {
                try
                {
                    file >> m_JSON;
                }
                catch (...)
                {
                    m_JSON = nlohmann::json::object();
                }
            }
            else
            {
                m_JSON = nlohmann::json::object();
                Save();
            }
        }

        void Save() // TO-DO: consider making saving a separate native
        {
            std::ofstream file(m_Path);
            file << m_JSON.dump(4);
        }

        std::filesystem::path m_Path;
        nlohmann::json m_JSON;

        static inline std::unordered_map<joaat_t, ScriptJSON> m_ScriptJSONs;
    };

    static ScriptFunctionCallContext g_scrFunctionCallContext;
    static std::unordered_map<joaat_t, std::ofstream> g_ScriptLogs;

    static void CleanupScriptLog(joaat_t scriptHash)
    {
        auto it = g_ScriptLogs.find(scriptHash);
        if (it != g_ScriptLogs.end())
        {
            it->second.close();
            g_ScriptLogs.erase(it);
        }
    }

    void CleanupScriptResources(joaat_t scriptHash)
    {
        CleanupScriptLog(scriptHash);
        ScriptJSON::Remove(scriptHash);
        LOGF(INFO, "Cleaned up resources for script with hash 0x{:X}.", scriptHash);
    }

    static void NativeCommandLogToFile(rage::scrThread::NativeContext* ctx)
    {
        auto thread = *rage::scrThread::TLS::Get()->m_CurrentThread;
        auto hash = thread->m_ScriptHash;
        auto name = thread->m_ScriptName;

        auto it = g_ScriptLogs.find(hash);
        if (it == g_ScriptLogs.end())
        {
            auto path = std::filesystem::path(g_Variables.ScriptsFolder) / (std::string(name) + ".log");
            it = g_ScriptLogs.emplace(hash, std::ofstream{path, std::ios::out | std::ios::trunc}).first;
        }

        auto& logFile = it->second;

        auto argCount = ctx->m_ArgCount - 1;
        auto args = ctx->m_Args + 1;
        auto typeMask = args[-1].Int;

        while (argCount > 0)
        {
            switch (typeMask & 3)
            {
            case 0:
                logFile << std::format("{}", args->Int);
                break;
            case 1:
                logFile << std::format("{:.2f}", args->Float);
                break;
            case 2:
                logFile << std::format("{}", args->String ? args->String : "NULL");
                break;
            case 3:
                logFile << std::format("<<{:.4f}, {:.4f}, {:.4f}>>", args->Reference[0].Float, args->Reference[1].Float, args->Reference[2].Float);
                break;
            }

            ++args;
            typeMask >>= 2;
            --argCount;
        }

        logFile << std::endl;
        logFile.flush();
    }

    static void NativeCommandClearLogFile(rage::scrThread::NativeContext* ctx)
    {
        auto thread = *rage::scrThread::TLS::Get()->m_CurrentThread;
        auto hash = thread->m_ScriptHash;
        auto name = thread->m_ScriptName;

        auto it = g_ScriptLogs.find(hash);
        if (it != g_ScriptLogs.end())
        {
            it->second.close();
            auto path = std::filesystem::path(g_Variables.ScriptsFolder) / (std::string(name) + ".log");
            it->second.open(path, std::ios::out | std::ios::trunc);
        }
    }

    static void NativeCommandRegisterIndividualFile(rage::scrThread::NativeContext* ctx)
    {
        auto file = ctx->m_Args[0].String;
        auto relativePath = ctx->m_Args[1].String;

        std::uint32_t index = 0xFFFFFFFF;
        g_Pointers.RegisterIndividualFile(&index, file, true, relativePath, false, false);
        ctx->m_Rets->Int = index != 0xFFFFFFFF;
    }

    static void NativeCommandInvalidateIndividualFile(rage::scrThread::NativeContext* ctx)
    {
        auto file = ctx->m_Args[0].String;

        g_Pointers.InvalidateIndividualFile(file);
    }

    static void NativeCommandJSONSetInt(rage::scrThread::NativeContext* ctx)
    {
        auto section = ctx->m_Args[0].String;
        auto key = ctx->m_Args[1].String;
        auto value = ctx->m_Args[2].Int;

        auto& json = ScriptJSON::GetOrCreate();
        json.Set(section, key, value);
    }

    static void NativeCommandJSONSetBool(rage::scrThread::NativeContext* ctx)
    {
        auto section = ctx->m_Args[0].String;
        auto key = ctx->m_Args[1].String;
        auto value = ctx->m_Args[2].Int;

        auto& json = ScriptJSON::GetOrCreate();
        json.Set(section, key, static_cast<bool>(value));
    }

    static void NativeCommandJSONSetFloat(rage::scrThread::NativeContext* ctx)
    {
        auto section = ctx->m_Args[0].String;
        auto key = ctx->m_Args[1].String;
        auto value = ctx->m_Args[2].Float;

        auto& json = ScriptJSON::GetOrCreate();
        json.Set(section, key, value);
    }

    static void NativeCommandJSONSetString(rage::scrThread::NativeContext* ctx)
    {
        auto section = ctx->m_Args[0].String;
        auto key = ctx->m_Args[1].String;
        auto value = ctx->m_Args[2].String;

        auto& json = ScriptJSON::GetOrCreate();
        json.Set(section, key, value);
    }

    static void NativeCommandJSONGetInt(rage::scrThread::NativeContext* ctx)
    {
        auto section = ctx->m_Args[0].String;
        auto key = ctx->m_Args[1].String;
        auto _default = ctx->m_Args[2].Int;

        auto& json = ScriptJSON::GetOrCreate();
        ctx->m_Rets->Int = json.Get(section, key, _default);
    }

    static void NativeCommandJSONGetBool(rage::scrThread::NativeContext* ctx)
    {
        auto section = ctx->m_Args[0].String;
        auto key = ctx->m_Args[1].String;
        auto _default = ctx->m_Args[2].Int;

        auto& json = ScriptJSON::GetOrCreate();
        ctx->m_Rets->Int = json.Get(section, key, static_cast<bool>(_default));
    }

    static void NativeCommandJSONGetFloat(rage::scrThread::NativeContext* ctx)
    {
        auto section = ctx->m_Args[0].String;
        auto key = ctx->m_Args[1].String;
        auto _default = ctx->m_Args[2].Float;

        auto& json = ScriptJSON::GetOrCreate();
        ctx->m_Rets->Float = json.Get(section, key, _default);
    }

    static void NativeCommandJSONGetString(rage::scrThread::NativeContext* ctx)
    {
        auto section = ctx->m_Args[0].String;
        auto key = ctx->m_Args[1].String;
        auto value = ctx->m_Args[2].Reference;
        auto _default = ctx->m_Args[3].String;

        auto& json = ScriptJSON::GetOrCreate();
        auto dest = reinterpret_cast<char*>(value);
        auto src = json.Get(section, key, std::string(_default));
        auto size = src.size();

        if (size > 63)
            size = 63;

        // We don't return a string because its lifetime wouldn't be guaranteed
        // Since the max native return count is 3, we can't return a text label, so we pass it as an argument instead
        std::strncpy(dest, src.c_str(), size);
        dest[size] = '\0';
    }

    static void NativeCommandMemoryScanPattern(rage::scrThread::NativeContext* ctx)
    {
        auto name = ctx->m_Args[0].String;
        auto pattern = ctx->m_Args[1].String;

        std::uint64_t ptr{};
        if (auto addr = Scanner::ScanPattern(name, pattern))
            ptr = addr->As<std::uint64_t>();

        ctx->m_Rets->Any = ptr;
    }

    static void NativeCommandMemoryAdd(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;
        auto offset = ctx->m_Args[1].Int;

        if (!ptr)
        {
            ctx->m_Rets->Any = 0;
            return;
        }

        ctx->m_Rets->Any = Memory(ptr).Add(offset).As<std::uint64_t>();
    }

    static void NativeCommandMemoryRip(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;

        if (!ptr)
        {
            ctx->m_Rets->Any = 0;
            return;
        }

        ctx->m_Rets->Any = Memory(ptr).Rip().As<std::uint64_t>();
    }

    static void NativeCommandMemoryDeref(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;

        if (!ptr)
        {
            ctx->m_Rets->Any = 0;
            return;
        }

        ctx->m_Rets->Any = static_cast<std::uint64_t>(*Memory(ptr).As<std::uint64_t*>());
    }

    static void NativeCommandMemoryWrite(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;
        auto patch = ctx->m_Args[1].Reference;

        if (!ptr || !patch)
            return;

        auto count = static_cast<std::uint32_t>(patch[0].Any);

        auto dst = Memory(ptr).As<std::uint8_t*>();

        std::vector<std::uint8_t> bytes;
        bytes.reserve(count);

        for (std::uint32_t i = 0; i < count; i++)
            bytes.push_back(static_cast<std::uint8_t>(patch[i + 1].Any));

        DWORD oldProtect, temp;
        VirtualProtect(dst, count, PAGE_EXECUTE_READWRITE, &oldProtect);
        std::memcpy(dst, bytes.data(), count);
        VirtualProtect(dst, count, oldProtect, &temp);
    }

    static void NativeCommandMemoryRead(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;
        auto patch = ctx->m_Args[1].Reference;

        if (!ptr || !patch)
            return;

        auto count = static_cast<std::uint32_t>(patch[0].Any);

        auto src = Memory(ptr).As<std::uint8_t*>();

        for (std::uint32_t i = 0; i < count; i++)
            patch[i + 1].Any = static_cast<std::uint64_t>(src[i]);
    }

    static void NativeCommandMemoryWriteInt(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;
        auto value = ctx->m_Args[1].Int;

        if (!ptr)
            return;

        *Memory(ptr).As<std::int32_t*>() = value;
    }

    static void NativeCommandMemoryReadInt(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;

        std::int32_t retVal{};
        if (ptr)
            retVal = *Memory(ptr).As<std::int32_t*>();

        ctx->m_Rets->Int = retVal;
    }

    static void NativeCommandMemoryWriteFloat(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;
        auto value = ctx->m_Args[1].Float;

        if (!ptr)
            return;

        *Memory(ptr).As<float*>() = value;
    }

    static void NativeCommandMemoryReadFloat(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;

        float retVal{};
        if (ptr)
            retVal = *Memory(ptr).As<float*>();

        ctx->m_Rets->Float = retVal;
    }

    static void NativeCommandMemoryWriteString(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;
        auto value = ctx->m_Args[1].String;
        auto size = ctx->m_Args[1].Int;

        if (!ptr)
            return;

        std::strncpy(Memory(ptr).As<char*>(), value, size);
    }

    static void NativeCommandMemoryReadString(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;

        const char* retVal{};
        if (ptr)
            retVal = Memory(ptr).As<const char*>();

        ctx->m_Rets->String = retVal;
    }

    static void NativeCommandMemoryWriteVector(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;
        auto valueX = ctx->m_Args[2].Float;
        auto valueY = ctx->m_Args[3].Float;
        auto valueZ = ctx->m_Args[4].Float;

        if (!ptr)
            return;

        *Memory(ptr).As<rage::Vector3*>() = rage::Vector3(valueX, valueY, valueZ);
    }

    static void NativeCommandMemoryReadVector(rage::scrThread::NativeContext* ctx)
    {
        auto ptr = ctx->m_Args[0].Any;

        rage::Vector3 retVal{};
        if (ptr)
            retVal = *Memory(ptr).As<rage::Vector3*>();

        ctx->m_Rets[0].Float = retVal.x;
        ctx->m_Rets[1].Float = retVal.y;
        ctx->m_Rets[2].Float = retVal.z;
    }

    static void NativeCommandSetCurrentScriptThread(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;

        if (auto thread = rage::scrThread::GetByHash(hash))
        {
            *rage::scrThread::TLS::Get()->m_CurrentThread = thread;
            *rage::scrThread::TLS::Get()->m_CurrentThreadActive = true;
        }
    }

    static void NativeCommandSetScriptThreadState(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto state = static_cast<rage::scrThread::State>(ctx->m_Args[1].Int);

        if (state < rage::scrThread::State::RUNNING || state > rage::scrThread::State::PAUSED)
            return;

        if (auto thread = rage::scrThread::GetByHash(hash))
            thread->m_Context.m_State = state;
    }

    static void NativeCommandScriptStaticSetInt(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;
        auto value = ctx->m_Args[2].Int;

        if (auto thread = rage::scrThread::GetByHash(hash))
            thread->m_Stack[index].Int = value;
    }

    static void NativeCommandScriptStaticSetFloat(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;
        auto value = ctx->m_Args[2].Float;

        if (auto thread = rage::scrThread::GetByHash(hash))
            thread->m_Stack[index].Float = value;
    }

    static void NativeCommandScriptStaticSetString(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;
        auto value = ctx->m_Args[2].String;

        if (auto thread = rage::scrThread::GetByHash(hash))
            thread->m_Stack[index].String = value;
    }

    static void NativeCommandScriptStaticSetTextLabel(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;
        auto value = ctx->m_Args[2].String;
        auto size = ctx->m_Args[3].Int;

        if (auto thread = rage::scrThread::GetByHash(hash))
        {
            if (size > 63)
                size = 63;

            auto dest = reinterpret_cast<char*>(&thread->m_Stack[index]);
            std::strncpy(dest, value, size - 1);
            dest[size - 1] = '\0';
        }
    }

    static void NativeCommandScriptStaticSetVector(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;
        auto valueX = ctx->m_Args[2].Float;
        auto valueY = ctx->m_Args[3].Float;
        auto valueZ = ctx->m_Args[4].Float;

        if (auto thread = rage::scrThread::GetByHash(hash))
        {
            thread->m_Stack[index + 0].Float = valueX;
            thread->m_Stack[index + 1].Float = valueY;
            thread->m_Stack[index + 2].Float = valueZ;
        }
    }

    static void NativeCommandScriptStaticGetInt(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;

        std::int32_t retVal{};
        if (auto thread = rage::scrThread::GetByHash(hash))
            retVal = thread->m_Stack[index].Int;

        ctx->m_Rets->Int = retVal;
    }

    static void NativeCommandScriptStaticGetFloat(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;

        float retVal{};
        if (auto thread = rage::scrThread::GetByHash(hash))
            retVal = thread->m_Stack[index].Float;

        ctx->m_Rets->Float = retVal;
    }

    static void NativeCommandScriptStaticGetString(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;

        const char* retVal{""};
        if (auto thread = rage::scrThread::GetByHash(hash))
            retVal = thread->m_Stack[index].String;

        ctx->m_Rets->String = retVal;
    }

    static void NativeCommandScriptStaticGetTextLabel(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;

        char* retVal{};
        if (auto thread = rage::scrThread::GetByHash(hash))
            retVal = reinterpret_cast<char*>(&thread->m_Stack[index]);

        ctx->m_Rets->String = retVal ? retVal : "";
    }

    static void NativeCommandScriptStaticGetVector(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;

        rage::scrVector retVal{};
        if (auto thread = rage::scrThread::GetByHash(hash))
        {
            retVal.x = thread->m_Stack[index + 0].Float;
            retVal.y = thread->m_Stack[index + 1].Float;
            retVal.z = thread->m_Stack[index + 2].Float;
        }

        ctx->m_Rets[0].Float = retVal.x;
        ctx->m_Rets[1].Float = retVal.y;
        ctx->m_Rets[2].Float = retVal.z;
    }

    static void NativeCommandScriptStaticGetPointer(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto index = ctx->m_Args[1].Int;

        std::uint64_t retVal{};
        if (auto thread = rage::scrThread::GetByHash(hash))
            retVal = reinterpret_cast<std::uint64_t>(&thread->m_Stack[index]);

        ctx->m_Rets->Any = retVal;
    }

    static void NativeCommandScriptGlobalSetInt(rage::scrThread::NativeContext* ctx)
    {
        auto index = ctx->m_Args[0].Int;
        auto value = ctx->m_Args[1].Int;

        g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF].Int = value;
    }

    static void NativeCommandScriptGlobalSetFloat(rage::scrThread::NativeContext* ctx)
    {
        auto index = ctx->m_Args[0].Int;
        auto value = ctx->m_Args[1].Float;

        g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF].Float = value;
    }

    static void NativeCommandScriptGlobalSetTextLabel(rage::scrThread::NativeContext* ctx)
    {
        auto index = ctx->m_Args[0].Int;
        auto value = ctx->m_Args[1].String;
        auto size = ctx->m_Args[2].Int;

        if (size > 63)
            size = 63;

        auto dest = reinterpret_cast<char*>(&g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF]);
        std::strncpy(dest, value, size - 1);
        dest[size - 1] = '\0';
    }

    static void NativeCommandScriptGlobalSetVector(rage::scrThread::NativeContext* ctx)
    {
        auto index = ctx->m_Args[0].Int;
        auto valueX = ctx->m_Args[1].Float;
        auto valueY = ctx->m_Args[2].Float;
        auto valueZ = ctx->m_Args[3].Float;

        g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 0].Float = valueX;
        g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 1].Float = valueY;
        g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 2].Float = valueZ;
    }

    static void NativeCommandScriptGlobalGetInt(rage::scrThread::NativeContext* ctx)
    {
        auto index = ctx->m_Args[0].Int;

        ctx->m_Rets->Int = g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF].Int;
    }

    static void NativeCommandScriptGlobalGetFloat(rage::scrThread::NativeContext* ctx)
    {
        auto index = ctx->m_Args[0].Int;

        ctx->m_Rets->Float = g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF].Float;
    }

    static void NativeCommandScriptGlobalGetTextLabel(rage::scrThread::NativeContext* ctx)
    {
        auto index = ctx->m_Args[0].Int;

        ctx->m_Rets->String = reinterpret_cast<char*>(&g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF]);
    }

    static void NativeCommandScriptGlobalGetVector(rage::scrThread::NativeContext* ctx)
    {
        auto index = ctx->m_Args[0].Int;

        ctx->m_Rets[0].Float = g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 0].Float;
        ctx->m_Rets[1].Float = g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 1].Float;
        ctx->m_Rets[2].Float = g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][(index & 0x3FFFF) + 2].Float;
    }

    static void NativeCommandScriptGlobalGetPointer(rage::scrThread::NativeContext* ctx)
    {
        auto index = ctx->m_Args[0].Int;

        ctx->m_Rets->Any = reinterpret_cast<std::uint64_t>(&g_Pointers.ScriptGlobals[index >> 0x12 & 0x3F][index & 0x3FFFF]);
    }

    static void NativeCommandScriptFunctionBeginCall(rage::scrThread::NativeContext* ctx)
    {
        auto hash = ctx->m_Args[0].Int;
        auto pc = ctx->m_Args[1].Int;

        g_scrFunctionCallContext.BeginCall(hash, pc);
    }

    static void NativeCommandScriptFunctionAddParamInt(rage::scrThread::NativeContext* ctx)
    {
        auto value = ctx->m_Args[0].Int;

        g_scrFunctionCallContext.AddParam(value);
    }

    static void NativeCommandScriptFunctionAddParamFloat(rage::scrThread::NativeContext* ctx)
    {
        auto value = ctx->m_Args[0].Float;

        g_scrFunctionCallContext.AddParam(value);
    }

    static void NativeCommandScriptFunctionAddParamString(rage::scrThread::NativeContext* ctx)
    {
        auto value = ctx->m_Args[0].String;

        g_scrFunctionCallContext.AddParam(value);
    }

    static void NativeCommandScriptFunctionAddParamVector(rage::scrThread::NativeContext* ctx)
    {
        auto valueX = ctx->m_Args[0].Float;
        auto valueY = ctx->m_Args[1].Float;
        auto valueZ = ctx->m_Args[2].Float;

        g_scrFunctionCallContext.AddParam(valueX);
        g_scrFunctionCallContext.AddParam(valueY);
        g_scrFunctionCallContext.AddParam(valueZ);
    }

    static void NativeCommandScriptFunctionAddParamPointer(rage::scrThread::NativeContext* ctx)
    {
        auto value = ctx->m_Args[0].Any;

        g_scrFunctionCallContext.AddParam(reinterpret_cast<std::uint64_t*>(value));
    }

    static void NativeCommandScriptFunctionAddParamReference(rage::scrThread::NativeContext* ctx)
    {
        auto value = ctx->m_Args[0].Reference;

        g_scrFunctionCallContext.AddParam(value);
    }

    static void NativeCommandScriptFunctionEndCallProc(rage::scrThread::NativeContext* ctx)
    {
        g_scrFunctionCallContext.EndCall<int>(); // Return nothing.
    }

    static void NativeCommandScriptFunctionEndCallInt(rage::scrThread::NativeContext* ctx)
    {
        ctx->m_Args->Int = g_scrFunctionCallContext.EndCall<int>();
    }

    static void NativeCommandScriptFunctionEndCallFloat(rage::scrThread::NativeContext* ctx)
    {
        ctx->m_Args->Float = g_scrFunctionCallContext.EndCall<float>();
    }

    static void NativeCommandScriptFunctionEndCallString(rage::scrThread::NativeContext* ctx)
    {
        ctx->m_Args->String = g_scrFunctionCallContext.EndCall<const char*>();
    }

    static void NativeCommandScriptFunctionEndCallVector(rage::scrThread::NativeContext* ctx)
    {
        auto vector = g_scrFunctionCallContext.EndCall<rage::scrVector>();

        ctx->m_Args[0].Float = vector.x;
        ctx->m_Args[1].Float = vector.y;
        ctx->m_Args[2].Float = vector.z;
    }

    void RegisterNatives()
    {
        static auto RegisterNative = [](std::uint64_t hash, rage::scrThread::NativeContext::Handler handler) {
            g_Pointers.RegisterNativeCommand(g_Pointers.NativeRegistrationTable, hash, handler);
            LOGF(INFO, "Registered native command with hash 0x{:X}.", hash);
        };

        // clang-format off

        RegisterNative(LOG_TO_FILE,                                       NativeCommandLogToFile);
        RegisterNative(CLEAR_LOG_FILE,                                    NativeCommandClearLogFile);
        RegisterNative(REGISTER_INDIVIDUAL_FILE,                          NativeCommandRegisterIndividualFile);
        RegisterNative(INVALIDATE_INDIVIDUAL_FILE,                        NativeCommandInvalidateIndividualFile);

        RegisterNative(JSON_SET_INT,                                      NativeCommandJSONSetInt);
        RegisterNative(JSON_SET_BOOL,                                     NativeCommandJSONSetBool);
        RegisterNative(JSON_SET_FLOAT,                                    NativeCommandJSONSetFloat);
        RegisterNative(JSON_SET_STRING,                                   NativeCommandJSONSetString);
        RegisterNative(JSON_GET_INT,                                      NativeCommandJSONGetInt);
        RegisterNative(JSON_GET_BOOL,                                     NativeCommandJSONGetBool);
        RegisterNative(JSON_GET_FLOAT,                                    NativeCommandJSONGetFloat);
        RegisterNative(JSON_GET_STRING,                                   NativeCommandJSONGetString);

        RegisterNative(MEMORY_SCAN_PATTERN,                               NativeCommandMemoryScanPattern);
        RegisterNative(MEMORY_ADD,                                        NativeCommandMemoryAdd);
        RegisterNative(MEMORY_RIP,                                        NativeCommandMemoryRip);
        RegisterNative(MEMORY_DEREF,                                      NativeCommandMemoryDeref);
        RegisterNative(MEMORY_WRITE,                                      NativeCommandMemoryWrite);
        RegisterNative(MEMORY_READ,                                       NativeCommandMemoryRead);
        RegisterNative(MEMORY_WRITE_INT,                                  NativeCommandMemoryWriteInt);
        RegisterNative(MEMORY_READ_INT,                                   NativeCommandMemoryReadInt);
        RegisterNative(MEMORY_WRITE_FLOAT,                                NativeCommandMemoryWriteFloat);
        RegisterNative(MEMORY_READ_FLOAT,                                 NativeCommandMemoryReadFloat);
        RegisterNative(MEMORY_WRITE_STRING,                               NativeCommandMemoryWriteString);
        RegisterNative(MEMORY_READ_STRING,                                NativeCommandMemoryReadString);
        RegisterNative(MEMORY_WRITE_VECTOR,                               NativeCommandMemoryWriteVector);
        RegisterNative(MEMORY_READ_VECTOR,                                NativeCommandMemoryReadVector);

        RegisterNative(SET_CURRENT_SCRIPT_THREAD,                         NativeCommandSetCurrentScriptThread);
        RegisterNative(SET_SCRIPT_THREAD_STATE,                           NativeCommandSetScriptThreadState);

        RegisterNative(SCRIPT_STATIC_SET_INT,                             NativeCommandScriptStaticSetInt);
        RegisterNative(SCRIPT_STATIC_SET_FLOAT,                           NativeCommandScriptStaticSetFloat);
        RegisterNative(SCRIPT_STATIC_SET_STRING,                          NativeCommandScriptStaticSetString);
        RegisterNative(SCRIPT_STATIC_SET_TEXT_LABEL,                      NativeCommandScriptStaticSetTextLabel);
        RegisterNative(SCRIPT_STATIC_SET_VECTOR,                          NativeCommandScriptStaticSetVector);
        RegisterNative(SCRIPT_STATIC_GET_INT,                             NativeCommandScriptStaticGetInt);
        RegisterNative(SCRIPT_STATIC_GET_FLOAT,                           NativeCommandScriptStaticGetFloat);
        RegisterNative(SCRIPT_STATIC_GET_STRING,                          NativeCommandScriptStaticGetString);
        RegisterNative(SCRIPT_STATIC_GET_TEXT_LABEL,                      NativeCommandScriptStaticGetTextLabel);
        RegisterNative(SCRIPT_STATIC_GET_VECTOR,                          NativeCommandScriptStaticGetVector);
        RegisterNative(SCRIPT_STATIC_GET_POINTER,                         NativeCommandScriptStaticGetPointer);

        RegisterNative(SCRIPT_GLOBAL_SET_INT,                             NativeCommandScriptGlobalSetInt);
        RegisterNative(SCRIPT_GLOBAL_SET_FLOAT,                           NativeCommandScriptGlobalSetFloat);
        RegisterNative(SCRIPT_GLOBAL_SET_TEXT_LABEL,                      NativeCommandScriptGlobalSetTextLabel);
        RegisterNative(SCRIPT_GLOBAL_SET_VECTOR,                          NativeCommandScriptGlobalSetVector);
        RegisterNative(SCRIPT_GLOBAL_GET_INT,                             NativeCommandScriptGlobalGetInt);
        RegisterNative(SCRIPT_GLOBAL_GET_FLOAT,                           NativeCommandScriptGlobalGetFloat);
        RegisterNative(SCRIPT_GLOBAL_GET_TEXT_LABEL,                      NativeCommandScriptGlobalGetTextLabel);
        RegisterNative(SCRIPT_GLOBAL_GET_VECTOR,                          NativeCommandScriptGlobalGetVector);
        RegisterNative(SCRIPT_GLOBAL_GET_POINTER,                         NativeCommandScriptGlobalGetPointer);

        RegisterNative(SCRIPT_FUNCTION_BEGIN_CALL,                        NativeCommandScriptFunctionBeginCall);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_INT,                     NativeCommandScriptFunctionAddParamInt);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_FLOAT,                   NativeCommandScriptFunctionAddParamFloat);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_STRING,                  NativeCommandScriptFunctionAddParamString);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_VECTOR,                  NativeCommandScriptFunctionAddParamVector);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_POINTER,                 NativeCommandScriptFunctionAddParamPointer);
        // Register all ref parameters to the same handler.
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_INT,           NativeCommandScriptFunctionAddParamReference);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_FLOAT,         NativeCommandScriptFunctionAddParamReference);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_TEXT_LABEL_15, NativeCommandScriptFunctionAddParamReference);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_TEXT_LABEL_23, NativeCommandScriptFunctionAddParamReference);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_TEXT_LABEL_31, NativeCommandScriptFunctionAddParamReference);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_TEXT_LABEL_63, NativeCommandScriptFunctionAddParamReference);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_VECTOR,        NativeCommandScriptFunctionAddParamReference);
        RegisterNative(SCRIPT_FUNCTION_ADD_PARAM_REFERENCE_STRUCT,        NativeCommandScriptFunctionAddParamReference);
        RegisterNative(SCRIPT_FUNCTION_END_CALL_PROC,                     NativeCommandScriptFunctionEndCallProc);
        RegisterNative(SCRIPT_FUNCTION_END_CALL_INT,                      NativeCommandScriptFunctionEndCallInt);
        RegisterNative(SCRIPT_FUNCTION_END_CALL_FLOAT,                    NativeCommandScriptFunctionEndCallFloat);
        RegisterNative(SCRIPT_FUNCTION_END_CALL_STRING,                   NativeCommandScriptFunctionEndCallString);
        RegisterNative(SCRIPT_FUNCTION_END_CALL_VECTOR,                   NativeCommandScriptFunctionEndCallVector);

        // clang-format on
    }
}