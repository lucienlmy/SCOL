#pragma once

namespace SCOL::Loader
{
    uint32_t LoadScript(const char* path, void* args, uint32_t argCount, uint32_t stackSize);
    void LoadScripts();
    void ReloadScripts();
    std::string GetScriptOverridePath(uint32_t hash);
}