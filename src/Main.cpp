#include "Hooking.hpp"
#include "Natives.hpp"
#include "Pointers.hpp"
#include "ScriptFiber.hpp"
#include "ScriptLoader.hpp"

namespace SCOL
{
    static void ScriptMain()
    {
        while (true)
        {
            // Initial load
            if (rage::scrThread::GetByHash("Startup"_J))
            {
                if (!g_StartupAvailable)
                {
                    ScriptLoader::LoadScripts();
                    g_StartupAvailable = true;
                }
            }
            else
            {
                g_StartupAvailable = false; // Reset when startup finishes so the next time it is available (e.g. re-entering the game after returning to the main menu), we can reload the scripts automatically.
            }

            if (g_LoadRequested)
            {
                if (*g_Pointers.LoadingScreenState == 0)
                    ScriptLoader::LoadScripts();
                g_LoadRequested = false;
            }

            ScriptFiber::Yield();
        }
    }

    static DWORD Main(PVOID)
    {
        Logging::Init("SCOL.log");
        LOGF(INFO, "Logging initialized.");

        Settings::Init("SCOL.json");
        LOGF(INFO, "Settings initialized.");

        if (!g_Pointers.Init())
        {
            MessageBoxA(0, "Some patterns could not be found.", "SCOL", MB_ICONERROR);
            return EXIT_FAILURE;
        }
        LOGF(INFO, "Pointers initialized.");

        if (!Hooking::Init())
        {
            MessageBoxA(0, "Failed to initialize hooking.", "SCOL", MB_ICONERROR);
            return EXIT_FAILURE;
        }
        LOGF(INFO, "Hooking initialized.");

        while (!g_Pointers.NativeRegistrationTable->m_Initialized)
            std::this_thread::sleep_for(100ms);
        Natives::RegisterNatives();
        LOGF(INFO, "Natives registered.");

        ScriptFiber::Add("ScriptMain", ScriptMain);
        LOGF(INFO, "Script fibers registered.");

        while (true)
        {
            Settings::Update();

            std::this_thread::yield();
        }

        return EXIT_SUCCESS;
    }
}

BOOL WINAPI DllMain(HINSTANCE dllInstance, DWORD reason, PVOID)
{
    using namespace SCOL;

    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(dllInstance);
        CreateThread(nullptr, 0, Main, nullptr, 0, nullptr);
    }

    return TRUE;
}