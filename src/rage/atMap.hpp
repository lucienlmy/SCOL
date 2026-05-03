#pragma once

namespace rage
{
    template <typename K, typename V>
    class atMap
    {
    public:
        struct Entry
        {
            K m_Key;
            V m_Data;
            Entry* m_Next;
        };

        Entry** m_Entries;        // 00000000
        std::uint16_t m_Size;     // 00000008
        std::uint16_t m_Capacity; // 0000000A
        char m_Pad[0x04];         // 0000000C
    };
    static_assert(sizeof(atMap<std::uint32_t, std::uint32_t>) == 0x00000010);
}