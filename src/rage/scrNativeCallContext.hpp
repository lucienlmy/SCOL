#pragma once
#include "scrValue.hpp"
#include "scrVector.hpp"

namespace rage
{
    struct scrNativeCallContext
    {
        scrValue* m_ReturnValue;
        std::uint32_t m_ArgCount;
        scrValue* m_Args;
        std::int32_t m_NumVectorRefs;
        scrVector* m_VectorRefTargets[4];
        Vector3 m_VectorRefSources[4];
    };
    static_assert(sizeof(scrNativeCallContext) == 0x80);

    using scrNativeHash = std::uint64_t;
    using scrNativeHandler = void (*)(scrNativeCallContext*);
}