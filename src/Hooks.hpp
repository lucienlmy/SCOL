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
        bool Init();
    };

    inline Hooks g_Hooks;
}