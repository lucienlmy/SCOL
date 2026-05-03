#pragma once

namespace SCOL
{
    class ScriptLoader
    {
    public:
        static std::uint32_t LoadScript(const std::filesystem::path& path, void* args, std::uint32_t argCount, std::uint32_t stackSize)
        {
            return GetInstance().LoadScriptImpl(path, args, argCount, stackSize);
        }

        static void LoadScripts()
        {
            GetInstance().LoadScriptsImpl();
        }

        static std::filesystem::path GetScriptOverridePath(std::uint32_t hash)
        {
            return GetInstance().GetScriptOverridePathImpl(hash);
        }

    private:
        enum class ScriptType
        {
            INVALID,
            DEFAULT,
            STREAMED,
            STREAMED_FULL
        };

        static ScriptLoader& GetInstance()
        {
            static ScriptLoader instance{};
            return instance;
        }

        static std::string RemoveAllExtensions(const std::filesystem::path& path);
        static ScriptType GetScriptType(const std::filesystem::path& path);

        std::uint32_t LoadDefaultScript(const std::filesystem::path& path, void* args, std::uint32_t argCount, std::uint32_t stackSize);
        std::uint32_t LoadStreamedScript(const std::filesystem::path& path, void* args, std::uint32_t argCount, std::uint32_t stackSize);
        std::uint32_t LoadStreamedFullScript(const std::filesystem::path& path, void* args, std::uint32_t argCount, std::uint32_t stackSize);
        void CleanupScripts();
        std::uint32_t LoadScriptImpl(const std::filesystem::path& path, void* args, std::uint32_t argCount, std::uint32_t stackSize);
        void LoadScriptsImpl();
        std::filesystem::path GetScriptOverridePathImpl(std::uint32_t hash);

        std::vector<std::uint32_t> m_ThreadIds{};
        std::vector<std::pair<std::uint32_t, std::string>> m_strIndices{};
    };
}