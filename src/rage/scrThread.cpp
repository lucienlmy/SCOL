#include "scrThread.hpp"
#include "Pointers.hpp"
#include "atArray.hpp"

namespace rage
{
    scrThread* scrThread::GetThread(std::uint32_t hash)
    {
        for (auto& thread : *SCOL::g_Pointers.ScriptThreads)
        {
            if (thread && thread->m_Context.m_ThreadId && thread->m_ScriptHash == hash)
            {
                return thread;
            }
        }

        return nullptr;
    }

    scrThread* scrThread::GetThreadById(std::uint32_t id)
    {
        for (auto& thread : *SCOL::g_Pointers.ScriptThreads)
        {
            if (thread && thread->m_Context.m_ThreadId == id)
            {
                return thread;
            }
        }

        return nullptr;
    }
}