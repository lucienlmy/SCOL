#include "ScriptFunction.hpp"
#include "Pointers.hpp"
#include "ScriptFiber.hpp"

namespace SCOL
{
    void ScriptFunction::CallInternal(const joaat_t script, const std::uint32_t pc, const std::uint64_t* args, const std::uint32_t argCount, void* returnValue, std::uint32_t returnSize)
    {
        auto thread = rage::scrThread::GetByHash(script);
        auto program = rage::scrProgram::GetByHash(script);

        if (!thread || !program || pc == 0)
            return;

        auto tls = rage::scrThread::TLS::Get();
        auto stack = thread->m_Stack;
        auto ogThread = *tls->m_CurrentThread;
        auto ogState = thread->m_Context.m_State;

        *tls->m_CurrentThread = thread;
        *tls->m_CurrentThreadActive = true;

        auto ctx = thread->m_Context;
        auto topStack = ctx.m_Sp;

        for (std::uint32_t i = 0; i < argCount; i++)
            stack[ctx.m_Sp++].Any = args[i];

        stack[ctx.m_Sp++].Any = 0;
        ctx.m_Pc = pc;
        ctx.m_State = rage::scrThread::State::RUNNING;

        while (g_Pointers.RunScriptThread(stack, g_Pointers.ScriptGlobals, program, &ctx) == rage::scrThread::State::PAUSED)
            ScriptFiber::Yield(std::chrono::seconds(static_cast<std::uint32_t>(ctx.m_WaitTime))); // WAIT converts ms to sec

        *tls->m_CurrentThread = ogThread;
        *tls->m_CurrentThreadActive = ogThread != nullptr;

        if (thread->m_Context.m_State != ogState && thread->m_Context.m_State != rage::scrThread::State::KILLED)
            thread->m_Context.m_State = ogState;

        if (returnValue && returnSize != 0)
            std::memcpy(returnValue, stack + topStack, returnSize);
    }
}