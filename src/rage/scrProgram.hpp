#pragma once
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

        std::uint8_t* GetCodePage(std::uint32_t page) const
        {
            if (page < GetNumCodePages())
                return m_CodePages[page];

            return nullptr;
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

        scrValue* GetGlobalBlock(std::uint32_t page) const
        {
            if (page < GetNumGlobalPages())
                return m_GlobalPages[page];

            return nullptr;
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

        const char* GetStringPage(std::uint32_t page) const
        {
            if (page < GetNumStringPages())
                return m_StringPages[page];

            return nullptr;
        }

        const char* GetString(std::uint32_t index) const
        {
            if (index < m_StringsSize)
                return &m_StringPages[index >> 14][index & 0x3FFF];

            return nullptr;
        }

        static scrProgram* GetProgram(std::uint32_t hash);
    };
    static_assert(sizeof(scrProgram) == 0x80);
}