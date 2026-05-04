#pragma once
#include "scrValue.hpp"
#include "scrVector.hpp"
#include "sysObfuscated.hpp"

namespace rage
{
    class scrThread
    {
    public:
        enum class State : std::uint32_t
        {
            RUNNING = 0U,
            WAITING = 1U,
            KILLED = 2U,
            PAUSED = 3U
        };

        enum class Priority : std::uint32_t
        {
            HIGHEST = 0U,
            NORMAL = 1U,
            LOWEST = 2U,
            MANUAL_UPDATE = 100U
        };

        struct Context
        {
            std::uint32_t m_Id;            // 00000000
            char m_Pad1[4];                // 00000004
            std::uint64_t m_ProgramHash;   // 00000008
            State m_State;                 // 00000010
            std::uint32_t m_Pc;            // 00000014
            std::uint32_t m_Fp;            // 00000018
            std::uint32_t m_Sp;            // 0000001C
            float m_TimerA;                // 00000020
            float m_TimerB;                // 00000024
            float m_WaitTime;              // 00000028
            std::uint32_t m_MinPc;         // 0000002C (unused, debug-only)
            std::uint32_t m_MaxPc;         // 00000030 (unused, debug-only)
            char m_Pad2[4];                // 00000034
            scrValue m_Tls[4];             // 00000038 (unused)
            std::uint32_t m_StackSize;     // 00000058
            std::uint32_t m_CatchPc;       // 0000005C (unused, debug-only)
            std::uint32_t m_CatchFp;       // 00000060 (unused, debug-only)
            std::uint32_t m_CatchSp;       // 00000064 (unused, debug-only)
            Priority m_Priority;           // 00000068
            std::uint8_t m_CallDepth;      // 0000006C
            char m_Pad3[3];                // 0000006D
            std::uint32_t m_CallStack[16]; // 00000070
        };
        static_assert(sizeof(Context) == 0x000000B0);

        struct NativeContext
        {
            using Handler = void (*)(NativeContext*);

            scrValue* m_Rets;                // 00000000
            std::uint32_t m_ArgCount;        // 00000008
            scrValue* m_Args;                // 00000010
            std::uint32_t m_VectorRefCount;  // 00000018
            scrVector* m_VectorRefOutput[4]; // 00000020
            Vector3 m_VectorRefInput[4];     // 00000040
        };
        static_assert(sizeof(NativeContext) == 0x00000080);

        struct Snapshot
        {
            std::uint32_t m_Id;          // 00000000
            char m_Pad1[4];              // 00000004
            std::uint64_t m_ProgramHash; // 00000008
            std::uint32_t m_CreateTime;  // 00000010
            bool m_HasNetComponent;      // 00000014
            std::uint8_t m_HostBD[10];   // 00000015
            std::uint8_t m_PlayerBD[10]; // 0000001F
            bool m_IsSet;                // 00000029
            char m_Pad2[2];              // 0000002A
        };
        static_assert(sizeof(Snapshot) == 0x00000030);

        struct TLS
        {
            rage::scrThread** m_CurrentThread; // 000007A0
            bool* m_CurrentThreadActive;       // 000007A8

            static TLS* Get();
        };

        virtual ~scrThread() = default;                                                        // 00000000
        virtual void Reset(std::uint32_t programHash, void* args, std::uint32_t argCount) = 0; // 00000008
        virtual State Run(std::uint32_t insnCount) = 0;                                        // 00000010
        virtual State Update(std::uint32_t insnCount) = 0;                                     // 00000018
        virtual void Kill() = 0;                                                               // 00000020
        virtual void SetSnapshot(Snapshot* snapshot) = 0;                                      // 00000028

        Context m_Context;          // 00000008
        scrValue* m_Stack;          // 000000B8
        std::uint32_t m_InsnCount;  // 000000C0 (unused, debug-only)
        std::uint32_t m_ArgsSize;   // 000000C4
        std::uint32_t m_ArgsOffset; // 000000C8
        std::uint32_t m_CreateTime; // 000000CC (set by timeGetTime)
        char m_KillReason[128];     // 000000D0
        std::uint32_t m_ScriptHash; // 00000150
        char m_ScriptName[64];      // 00000154
        char m_Pad1[4];             // 00000194

        static scrThread* GetByHash(std::uint32_t hash);
        static scrThread* GetById(std::uint32_t id);
    };
    static_assert(sizeof(scrThread) == 0x00000198);

    class scrNativeRegistration
    {
    public:
        struct RegistrationNode
        {
            sysObfuscated<RegistrationNode*> m_Next;         // 00000000
            scrThread::NativeContext::Handler m_Handlers[7]; // 00000010
            sysObfuscated<std::uint32_t> m_NumEntries;       // 00000048
            sysObfuscated<std::uint64_t> m_Hashes[7];        // 00000054
            char m_Pad1[0x35];                               // 000000C4 (rest is unused)
        };
        static_assert(sizeof(RegistrationNode) == 0x00000100);

        RegistrationNode* m_Nodes[256]; // 00000000
        std::uint32_t m_Unk;            // 00000800 (unused)
        bool m_Initialized;             // 00000804
        char m_Pad1[0x03];              // 00000805
    };
    static_assert(sizeof(scrNativeRegistration) == 0x00000808);
}