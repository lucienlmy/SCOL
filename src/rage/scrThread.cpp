#include "Pointers.hpp"
#include "atArray.hpp"

namespace rage
{
    scrThread::TLS* scrThread::TLS::Get()
    {
        static TLS tls{};
        if (!tls.m_CurrentThread || !tls.m_CurrentThreadActive)
        {
            tls.m_CurrentThread = reinterpret_cast<scrThread**>(*reinterpret_cast<std::uintptr_t*>(__readgsqword(0x58)) + 0x7A0);
            tls.m_CurrentThreadActive = reinterpret_cast<bool*>(*reinterpret_cast<std::uintptr_t*>(__readgsqword(0x58)) + 0x7A8);
        }

        return &tls;
    }

    scrThread* scrThread::GetByHash(std::uint32_t hash)
    {
        for (auto& thread : *SCOL::g_Pointers.ScriptThreads)
        {
            if (thread && thread->m_Context.m_Id != 0 && thread->m_ScriptHash == hash)
                return thread;
        }

        return nullptr;
    }

    scrThread* scrThread::GetById(std::uint32_t id)
    {
        for (auto& thread : *SCOL::g_Pointers.ScriptThreads)
        {
            if (thread && thread->m_Context.m_Id == id)
                return thread;
        }

        return nullptr;
    }
}