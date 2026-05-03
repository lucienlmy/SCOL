#pragma once
#include "atMap.hpp"
#include "pgBase.hpp"
#include "scrValue.hpp"

namespace rage
{
    class scrProgram : public pgBase
    {
    public:
        std::uint8_t** m_CodePages;                // 00000010
        std::uint32_t m_GlobalVersion;             // 00000018
        std::uint32_t m_CodeSize;                  // 0000001C
        std::uint32_t m_ArgCount;                  // 00000020
        std::uint32_t m_StaticCount;               // 00000024
        std::uint32_t m_GlobalCountAndBlock;       // 00000028
        std::uint32_t m_NativeCount;               // 0000002C
        scrValue* m_Statics;                       // 00000030
        scrValue** m_GlobalPages;                  // 00000038
        std::uint64_t* m_Natives;                  // 00000040
        std::uint32_t m_ProcCount;                 // 00000048
        char m_Pad1[0x04];                         // 0000004C
        const char** m_ProcNames;                  // 00000050
        std::uint32_t m_NameHash;                  // 00000058
        std::uint32_t m_RefCount;                  // 0000005C
        const char* m_Name;                        // 00000060
        const char** m_StringPages;                // 00000068
        std::uint32_t m_StringsSize;               // 00000070
        char m_Pad2[0x04];                         // 00000074
        atMap<std::uint32_t, bool>* m_Breakpoints; // 00000078

        std::uint32_t GetNumCodePages() const
        {
            return (m_CodeSize + 0x3FFF) >> 14;
        }

        std::uint32_t GetCodePageSize(std::uint32_t page) const
        {
            auto num = GetNumCodePages();
            if (page < num)
            {
                if (page == num - 1)
                    return (m_CodeSize & 0x3FFF);

                return 0x4000;
            }

            return 0;
        }

        std::uint8_t* GetCode(std::uint32_t index) const
        {
            if (index < m_CodeSize)
                return &m_CodePages[index >> 14][index & 0x3FFF];

            return nullptr;
        }

        std::uint32_t GetGlobalCount() const
        {
            return (m_GlobalCountAndBlock & 0x3FFFF);
        }

        std::uint32_t GetGlobalBlock() const
        {
            return (m_GlobalCountAndBlock >> 0x12);
        }

        std::uint32_t GetNumGlobalPages() const
        {
            return ((m_GlobalCountAndBlock & 0x3FFFF) + 0x3FFF) >> 14;
        }

        std::uint32_t GetGlobalPageSize(std::uint32_t page) const
        {
            auto num = GetNumGlobalPages();
            if (page < num)
            {
                if (page == num - 1)
                    return (m_GlobalCountAndBlock & 0x3FFFF) - (page << 14);

                return 0x4000;
            }

            return 0;
        }

        scrValue* GetGlobal(std::uint32_t index) const
        {
            if (index < GetGlobalCount())
                return &m_GlobalPages[index >> 14][index & 0x3FFF];

            return nullptr;
        }

        std::uint32_t GetNumStringPages() const
        {
            return (m_StringsSize + 0x3FFF) >> 14;
        }

        std::uint32_t GetStringPageSize(std::uint32_t page) const
        {
            auto num = GetNumStringPages();
            if (page < num)
            {
                if (page == num - 1)
                    return (m_StringsSize & 0x3FFF);

                return 0x4000;
            }

            return 0;
        }

        const char* GetString(std::uint32_t index) const
        {
            if (index < m_StringsSize)
                return &m_StringPages[index >> 14][index & 0x3FFF];

            return nullptr;
        }

        static scrProgram* GetByHash(std::uint32_t hash);
        static void Add(scrProgram* program);
    };
    static_assert(sizeof(scrProgram) == 0x00000080);

    struct scrProgramRegistry
    {
        std::uint8_t m_NextFreeSlot;     // 00000000
        std::uint8_t m_BucketHeads[32];  // 00000001
        std::uint8_t m_NextInChain[176]; // 00000021
        char m_Pad1[0x07];               // 000000D1
        scrProgram* m_Programs[176];     // 000000D8

        scrProgram* Find(std::uint32_t hash)
        {
            auto index = m_BucketHeads[hash & 0x1F];

            while (index)
            {
                if (m_Programs[index]->m_NameHash == hash)
                    return m_Programs[index];

                if (!m_NextInChain[index])
                    break;

                index = m_NextInChain[index];
            }

            return nullptr;
        }

        void Insert(scrProgram* program)
        {
            auto bucket = program->m_NameHash & 0x1F;
            auto next = m_NextInChain[m_NextFreeSlot];

            m_NextInChain[m_NextFreeSlot] = m_BucketHeads[bucket];
            m_BucketHeads[bucket] = m_NextFreeSlot;
            m_Programs[m_NextFreeSlot] = program;
            m_NextFreeSlot = next;
        }
    };
    static_assert(sizeof(scrProgramRegistry) == 0x00000658);
}