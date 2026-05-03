#pragma once

namespace rage
{
    template <typename T>
    class sysObfuscated
    {
    public:
        std::uint32_t m_Unk1[sizeof(T) / sizeof(std::uint32_t)]; // 00000000
        std::uint32_t m_Unk2;                                    // 00000004
        std::uint32_t m_Unk3;                                    // 00000008
    };
    static_assert(sizeof(sysObfuscated<std::uint32_t>) == 0x0000000C);
}