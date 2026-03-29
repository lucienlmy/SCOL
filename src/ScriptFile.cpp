#include "ScriptFile.hpp"

namespace SCOL
{
    static void FixPointer(std::uint64_t& ptr)
    {
        ptr &= 0xFFFFFF;
    }

    static void Rotl(std::uint64_t& val, std::int32_t rot)
    {
        rot %= 64;
        val = (val << rot | val >> (64 - rot));
    }

    ScriptFile::ScriptFile(const std::filesystem::path& path)
        : m_File(),
          m_IsValid(),
          m_Header(),
          m_Code(),
          m_Statics(),
          m_Globals(),
          m_Natives(),
          m_Name(),
          m_Strings()
    {
        m_File.open(path, std::ios::in | std::ios::binary);
        if (!m_File.is_open())
        {
            LOGF(FATAL, "Failed to open script file.");
            return;
        }

        m_File.seekg(0, std::ios_base::beg);

        char magic[4];
        m_File.read(magic, 4);
        if (m_File.gcount() != 4 || std::memcmp(magic, "RSC7", 4) == 0)
        {
            LOGF(FATAL, "Failed to open script file. It is compressed.");
            return;
        }

        m_File.seekg(0, std::ios_base::beg);

        m_File.read(reinterpret_cast<char*>(&m_Header.PageBase), 8);
        m_File.read(reinterpret_cast<char*>(&m_Header.PageMap), 8);
        m_File.read(reinterpret_cast<char*>(&m_Header.CodePagesOffset), 8);
        FixPointer(m_Header.CodePagesOffset);
        m_File.read(reinterpret_cast<char*>(&m_Header.GlobalVersion), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.CodeSize), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.ArgCount), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.StaticCount), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.GlobalCountAndBlock), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.NativeCount), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.StaticsOffset), 8);
        FixPointer(m_Header.StaticsOffset);
        m_File.read(reinterpret_cast<char*>(&m_Header.GlobalPagesOffset), 8);
        FixPointer(m_Header.GlobalPagesOffset);
        m_File.read(reinterpret_cast<char*>(&m_Header.NativesOffset), 8);
        FixPointer(m_Header.NativesOffset);
        m_File.read(reinterpret_cast<char*>(&m_Header.ProcCount), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.Pad), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.ProcNamesOffset), 8);
        m_File.read(reinterpret_cast<char*>(&m_Header.NameHash), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.RefCount), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.NameOffset), 8);
        FixPointer(m_Header.NameOffset);
        m_File.read(reinterpret_cast<char*>(&m_Header.StringPagesOffset), 8);
        FixPointer(m_Header.StringPagesOffset);
        m_File.read(reinterpret_cast<char*>(&m_Header.StringsSize), 4);
        m_File.read(reinterpret_cast<char*>(&m_Header.Breakpoints), 12);

        // Code
        {
            if (m_Header.CodeSize > 0)
            {
                m_Code.resize(m_Header.CodeSize);

                for (std::uint32_t i = 0; i < GetNumCodePages(); i++)
                {
                    std::uint32_t pageSize = GetCodePageSize(i);

                    std::uint64_t loc;
                    m_File.seekg(m_Header.CodePagesOffset + (i * sizeof(std::uint64_t)), std::ios_base::beg);
                    m_File.read(reinterpret_cast<char*>(&loc), 8);
                    FixPointer(loc);

                    m_File.seekg(loc, std::ios_base::beg);
                    m_File.read(reinterpret_cast<char*>(&m_Code[i * 0x4000]), pageSize);
                }
            }
        }

        // Statics
        {
            if (m_Header.StaticCount > 0)
            {
                m_Statics.resize(m_Header.StaticCount);

                m_File.seekg(m_Header.StaticsOffset, std::ios_base::beg);
                m_File.read(reinterpret_cast<char*>(&m_Statics[0]), m_Header.StaticCount * sizeof(std::uint64_t));
            }
        }

        // Globals
        {
            if ((m_Header.GlobalCountAndBlock & 0x3FFFF) > 0)
            {
                m_Globals.resize(m_Header.GlobalCountAndBlock & 0x3FFFF);

                for (std::uint32_t i = 0; i < GetNumGlobalPages(); i++)
                {
                    std::uint32_t pageSize = GetGlobalPageSize(i);

                    std::uint64_t loc;
                    m_File.seekg(m_Header.GlobalPagesOffset + (i * sizeof(std::uint64_t)), std::ios_base::beg);
                    m_File.read(reinterpret_cast<char*>(&loc), 8);
                    FixPointer(loc);

                    m_File.seekg(loc, std::ios_base::beg);
                    m_File.read(reinterpret_cast<char*>(&m_Globals[i * 0x4000]), pageSize * sizeof(std::uint64_t));
                }
            }
        }

        // Natives
        {
            if (m_Header.NativeCount > 0)
            {
                m_Natives.reserve(m_Header.NativeCount);

                for (std::uint32_t i = 0; i < m_Header.NativeCount; i++)
                {
                    m_File.seekg(m_Header.NativesOffset + (i * sizeof(std::uint64_t)), std::ios_base::beg);

                    std::uint64_t hash;
                    m_File.read(reinterpret_cast<char*>(&hash), 8);
                    Rotl(hash, m_Header.CodeSize + i);

                    m_Natives.push_back(hash);
                }
            }
        }

        // Name
        {
            m_File.seekg(m_Header.NameOffset, std::ios_base::beg);
            std::getline(m_File, m_Name, '\0');
        }

        // Strings
        {
            if (m_Header.StringsSize > 0)
            {
                for (std::uint32_t i = 0; i < GetNumStringPages(); i++)
                {
                    std::uint32_t pageSize = GetStringPageSize(i);
                    std::vector<char> temp(pageSize);

                    std::uint64_t loc;
                    m_File.seekg(m_Header.StringPagesOffset + (i * sizeof(std::uint64_t)), std::ios_base::beg);
                    m_File.read(reinterpret_cast<char*>(&loc), 8);
                    FixPointer(loc);

                    m_File.seekg(loc, std::ios_base::beg);
                    m_File.read(temp.data(), pageSize);

                    size_t index = 0;
                    while (index < pageSize)
                    {
                        std::string str(&temp[index]);
                        m_Strings.push_back(str);

                        index += str.size() + 1;
                    }
                }
            }
        }

        m_IsValid = true;
        m_File.close();
    }
}