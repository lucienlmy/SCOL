#pragma once
#include "atRangeArray.hpp"
#include "pgBase.hpp"
#include "scrValue.hpp"

namespace rage
{
    class scrProgram : public pgBase
    {
    public:
        std::uint8_t** m_CodePages;
        std::uint32_t m_GlobalVersion;
        std::uint32_t m_CodeSize;
        std::uint32_t m_ArgCount;
        std::uint32_t m_StaticCount;
        std::uint32_t m_GlobalCountAndBlock;
        std::uint32_t m_NativeCount;
        scrValue* m_Statics;
        scrValue** m_GlobalPages;
        std::uint64_t* m_Natives;
        std::uint32_t m_ProcCount;
        char m_Pad1[0x04];
        const char** m_ProcNames;
        std::uint32_t m_NameHash;
        std::uint32_t m_RefCount;
        const char* m_Name;
        const char** m_StringPages;
        std::uint32_t m_StringsSize;
        bool m_Breakpoints[12];

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
                return &m_GlobalPages[index >> 0x12 & 0x3F][index & 0x3FFFF];

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

        static scrProgram* GetProgram(std::uint32_t hash);
        static void InsertProgram(scrProgram* program);
    };
    static_assert(sizeof(scrProgram) == 0x80);

    class scrProgramRegistry
    {
    public:
        std::uint8_t m_NextFreeSlot;
        atRangeArray<std::uint8_t, 32> m_BucketHeads;
        atRangeArray<std::uint8_t, 176> m_NextInChain;
        atRangeArray<scrProgram*, 176> m_Programs;

        scrProgram* Find(std::uint32_t hash)
        {
            auto index = m_BucketHeads.m_Data[hash & 0x1F];

            while (index)
            {
                if (m_Programs.m_Data[index]->m_NameHash == hash)
                    return m_Programs.m_Data[index];

                if (!m_NextInChain.m_Data[index])
                    break;

                index = m_NextInChain.m_Data[index];
            }

            return nullptr;
        }

        void Insert(scrProgram* program)
        {
            auto bucket = program->m_NameHash & 0x1F;
            auto next = m_NextInChain.m_Data[m_NextFreeSlot];

            m_NextInChain.m_Data[m_NextFreeSlot] = m_BucketHeads.m_Data[bucket];
            m_BucketHeads.m_Data[bucket] = m_NextFreeSlot;
            m_Programs.m_Data[m_NextFreeSlot] = program;
            m_NextFreeSlot = next;
        }
    };
    static_assert(sizeof(scrProgramRegistry) == 0x658);
}