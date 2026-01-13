#pragma once

namespace rage
{
    class pgBase
    {
    public:
        virtual ~pgBase() = default;

        std::uint32_t m_MapSize;
    };
    static_assert(sizeof(pgBase) == 0x10);
}