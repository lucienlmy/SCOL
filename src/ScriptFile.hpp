#pragma once

namespace SCOL
{
    class ScriptFile
    {
    public:
        struct Header
        {
            std::uint64_t PageBase;            // 0x00
            std::uint64_t PageMap;             // 0x08
            std::uint64_t CodePagesOffset;     // 0x10
            std::uint32_t GlobalVersion;       // 0x18
            std::uint32_t CodeSize;            // 0x1C
            std::uint32_t ArgCount;            // 0x20
            std::uint32_t StaticCount;         // 0x24
            std::uint32_t GlobalCountAndBlock; // 0x28
            std::uint32_t NativeCount;         // 0x2C
            std::uint64_t StaticsOffset;       // 0x30
            std::uint64_t GlobalPagesOffset;   // 0x38
            std::uint64_t NativesOffset;       // 0x40
            std::uint32_t ProcCount;           // 0x48 (always 0)
            char Pad[4];                       // 0x4C (always 0)
            std::uint64_t ProcNamesOffset;     // 0x50 (always 0)
            std::uint32_t NameHash;            // 0x58
            std::uint32_t RefCount;            // 0x5C
            std::uint64_t NameOffset;          // 0x60
            std::uint64_t StringPagesOffset;   // 0x68
            std::uint32_t StringsSize;         // 0x70
            char Breakpoints[12];              // 0x74 (always 0)
        };
        static_assert(sizeof(Header) == 0x80);

        ScriptFile(const std::filesystem::path& path);

        bool IsValid() const
        {
            return m_IsValid;
        }

        const Header& GetHeader() const
        {
            return m_Header;
        }

        const std::vector<std::uint8_t>& GetCode() const
        {
            return m_Code;
        }

        const std::vector<std::uint64_t>& GetStatics() const
        {
            return m_Statics;
        }

        const std::vector<std::uint64_t>& GetGlobals() const
        {
            return m_Globals;
        }

        const std::vector<std::uint64_t>& GetNatives() const
        {
            return m_Natives;
        }

        const std::string& GetName() const
        {
            return m_Name;
        }

        const std::vector<std::string>& GetStrings() const
        {
            return m_Strings;
        }

    private:
        std::uint32_t GetNumCodePages() const
        {
            return (m_Header.CodeSize + 0x3FFF) >> 14;
        }

        std::uint32_t GetCodePageSize(std::uint32_t page) const
        {
            std::uint32_t num = GetNumCodePages();
            if (page < num)
            {
                if (page == num - 1)
                    return m_Header.CodeSize & 0x3FFF;

                return 0x4000;
            }

            return 0;
        }

        std::uint32_t GetNumGlobalPages() const
        {
            return ((m_Header.GlobalCountAndBlock & 0x3FFFF) + 0x3FFF) >> 14;
        }

        std::uint32_t GetGlobalPageSize(std::uint32_t page) const
        {
            std::uint32_t num = GetNumGlobalPages();
            if (page < num)
            {
                if (page == num - 1)
                    return (m_Header.GlobalCountAndBlock & 0x3FFFF) - (page << 14);

                return 0x4000;
            }

            return 0;
        }

        std::uint32_t GetNumStringPages() const
        {
            return (m_Header.StringsSize + 0x3FFF) >> 14;
        }

        std::uint32_t GetStringPageSize(std::uint32_t page) const
        {
            std::uint32_t num = GetNumStringPages();
            if (page < num)
            {
                if (page == num - 1)
                    return m_Header.StringsSize & 0x3FFF;

                return 0x4000;
            }

            return 0;
        }

        bool m_IsValid;
        Header m_Header;
        std::vector<std::uint8_t> m_Code;
        std::vector<std::uint64_t> m_Statics;
        std::vector<std::uint64_t> m_Globals;
        std::vector<std::uint64_t> m_Natives;
        std::string m_Name;
        std::vector<std::string> m_Strings;
    };
}