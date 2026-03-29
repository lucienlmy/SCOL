#include "scrProgram.hpp"
#include "Pointers.hpp"

namespace rage
{
    scrProgram* scrProgram::GetProgram(std::uint32_t hash)
    {
        return SCOL::g_Pointers.ScriptProgramRegistry->Find(hash);
    }

    void scrProgram::InsertProgram(scrProgram* program)
    {
        SCOL::g_Pointers.ScriptProgramRegistry->Insert(program);
    }
}