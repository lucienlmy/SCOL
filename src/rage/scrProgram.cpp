#include "Pointers.hpp"

namespace rage
{
    scrProgram* scrProgram::GetByHash(std::uint32_t hash)
    {
        return SCOL::g_Pointers.ScriptProgramRegistry->Find(hash);
    }

    void scrProgram::Add(scrProgram* program)
    {
        SCOL::g_Pointers.ScriptProgramRegistry->Insert(program);
    }
}