#include "Hooking.hpp"
#include "Loader.hpp"
#include "Pointers.hpp"
#include "rage/scrProgram.hpp"

namespace SCOL
{
    static LRESULT WndProcDetour(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
    {
        if (umsg == WM_KEYUP && wparam == g_Variables.ReloadKey)
        {
            Loader::ReloadScripts();
        }

        return Hooking::GetOriginal<decltype(&WndProcDetour)>("WndProcHook")(hwnd, umsg, wparam, lparam);
    }

    static void AllocateGlobalBlockDetour(rage::scrProgram* program)
    {
        const auto block = program->GetGlobalBlock();
        if (g_Pointers.ScriptGlobals[block] != nullptr)
        {
            LOGF(INFO, "Global block {} has already been allocated, freeing it.", block);

            g_Pointers.sysVirtualFree(g_Pointers.ScriptGlobals[block]);
            g_Pointers.ScriptGlobals[block] = nullptr;
        }

        Hooking::GetOriginal<decltype(&AllocateGlobalBlockDetour)>("AllocateGlobalBlockHook")(program);
    }

    static uint32_t StartNewGtaThreadDetour(uint32_t programHash, void* args, uint32_t argCount, uint32_t stackSize)
    {
        if (auto path = Loader::GetScriptOverridePath(programHash); !path.empty())
        {
            if (auto program = rage::scrProgram::GetProgram(programHash))
            {
                auto destructor = *(*reinterpret_cast<void***>(program) + 6);
                reinterpret_cast<void (*)(rage::scrProgram*, bool)>(destructor)(program, true); // Free the program loaded by natives first, LoadAndStartScriptObj will create a new one from the SCO
            }

            if (auto id = Loader::LoadScript(path.c_str(), args, argCount, stackSize))
            {
                LOGF(INFO, "Loaded script override from path '{}'.", path.c_str());
                return id;
            }
        }

        return Hooking::GetOriginal<decltype(&StartNewGtaThreadDetour)>("StartNewGtaThreadHook")(programHash, args, argCount, stackSize);
    }

    bool Hooking::Init()
    {
        if (MH_Initialize() != MH_OK)
            return false;

        AddHook("WndProcHook", g_Pointers.WndProc, WndProcDetour);
        AddHook("AllocateGlobalBlockHook", g_Pointers.AllocateGlobalBlock, AllocateGlobalBlockDetour);
        AddHook("StartNewGtaThreadHook", g_Pointers.StartNewGtaThread, StartNewGtaThreadDetour);

        bool success = true;

        for (auto& hook : m_Hooks)
        {
            if (auto result = MH_CreateHook(hook.m_Target, hook.m_Detour, &hook.m_Original); result != MH_OK)
            {
                LOGF(FATAL, "Failed to create hook for {} ({}).", hook.m_Name, MH_StatusToString(result));
                success = false;
                continue;
            }
            else
            {
                LOGF(INFO, "Created hook for {}.", hook.m_Name);
            }

            if (auto result = MH_EnableHook(hook.m_Target); result != MH_OK)
            {
                LOGF(FATAL, "Failed to enable hook for {} ({}).", hook.m_Name, MH_StatusToString(result));
                success = false;
            }
            else
            {
                LOGF(INFO, "Enabled hook for {}.", hook.m_Name);
            }
        }

        return success;
    }
}