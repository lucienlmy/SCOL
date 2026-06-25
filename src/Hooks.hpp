#pragma once

namespace SCOL
{
    struct HookData
    {
        SafetyHookInline WndProc;
        SafetyHookInline UpdateScriptThreads;
        SafetyHookInline AllocateGlobalBlock;
        SafetyHookInline StartNewGtaThread;
    };

    struct Hooks : HookData
    {
        void Init();
    };

    inline Hooks g_Hooks;
}