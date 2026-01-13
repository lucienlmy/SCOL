#pragma once
#include "rage/scrThread.hpp"

class CGameScriptHandler;
class CGameScriptHandlerNetComponent;

class GtaThread : public rage::scrThread
{
public:
    virtual ~GtaThread() = default;
    virtual void Reset(std::uint32_t programHash, void* args, std::uint32_t argCount) = 0;
    virtual rage::scrThreadState Run() = 0;
    virtual rage::scrThreadState Update() = 0;
    virtual void Kill() = 0;
    virtual void GetInfo(void* info) = 0;

    CGameScriptHandler* m_Handler;
    CGameScriptHandlerNetComponent* m_NetComponent;
    std::uint32_t m_ScriptHash2;
    std::uint32_t m_ForceCleanupPc;
    std::uint32_t m_ForceCleanupFp;
    std::uint32_t m_ForceCleanupSp;
    std::uint32_t m_ForceCleanupFilter;
    std::uint32_t m_ForceCleanupCause;
    std::int32_t m_InstanceId;
    char m_Pad3[0x04];
    std::uint8_t m_Flag1;
    bool m_SafeForNetworkGame;
    char m_Pad4[0x02];
    bool m_IsMinigameScript;
    char m_Pad5[0x02];
    bool m_CanBePaused;
    bool m_CanRemoveBlipsFromOtherScripts;
    char m_Pad6[0x02];
    std::uint8_t m_ForceCleanupState;
    char m_Pad7[0x0C];
};
static_assert(sizeof(GtaThread) == 0x1E0);