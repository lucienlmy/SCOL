#pragma once

namespace SCOL::Loader
{
    std::uint32_t LoadScript(const std::filesystem::path& path, void* args, std::uint32_t argCount, std::uint32_t stackSize);
    void LoadScripts();
    void ReloadScripts();
    std::filesystem::path GetScriptOverridePath(std::uint32_t hash);
}