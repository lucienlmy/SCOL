#pragma once

namespace rage
{
    class pgBase
    {
    public:
        virtual ~pgBase() = default; // 00000000

        void* m_Map; // 00000008
    };
    static_assert(sizeof(pgBase) == 0x00000010);
}