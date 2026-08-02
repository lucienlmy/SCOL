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
        : m_IsValid(),
          m_Header(),
          m_Code(),
          m_Statics(),
          m_Globals(),
          m_Natives(),
          m_Name(),
          m_Strings()
    {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file)
        {
            LOGF(FATAL, "Failed to open script file.");
            return;
        }

        char magic[4];
        file.read(magic, 4);
        if (file.gcount() != 4 || std::memcmp(magic, "RSC7", 4) == 0)
        {
            LOGF(FATAL, "Failed to read script file. It is compressed.");
            return;
        }

        file.seekg(0, std::ios_base::beg);
        file.read(reinterpret_cast<char*>(&m_Header), sizeof(Header));
        if (file.gcount() != sizeof(Header))
        {
            LOGF(FATAL, "Failed to read script header.");
            return;
        }

        FixPointer(m_Header.CodePagesOffset);
        FixPointer(m_Header.StaticsOffset);
        FixPointer(m_Header.GlobalPagesOffset);
        FixPointer(m_Header.NativesOffset);
        FixPointer(m_Header.NameOffset);
        FixPointer(m_Header.StringPagesOffset);

        // Code
        {
            if (m_Header.CodeSize > 0)
            {
                m_Code.resize(m_Header.CodeSize);

                for (std::uint32_t i = 0; i < GetNumCodePages(); i++)
                {
                    std::uint32_t pageSize = GetCodePageSize(i);

                    std::uint64_t loc;
                    file.seekg(m_Header.CodePagesOffset + (i * sizeof(std::uint64_t)), std::ios_base::beg);
                    file.read(reinterpret_cast<char*>(&loc), 8);
                    FixPointer(loc);

                    file.seekg(loc, std::ios_base::beg);
                    file.read(reinterpret_cast<char*>(&m_Code[i * 0x4000]), pageSize);
                }
            }
        }

        // Statics
        {
            if (m_Header.StaticCount > 0)
            {
                m_Statics.resize(m_Header.StaticCount);

                file.seekg(m_Header.StaticsOffset, std::ios_base::beg);
                file.read(reinterpret_cast<char*>(&m_Statics[0]), m_Header.StaticCount * sizeof(std::uint64_t));
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
                    file.seekg(m_Header.GlobalPagesOffset + (i * sizeof(std::uint64_t)), std::ios_base::beg);
                    file.read(reinterpret_cast<char*>(&loc), 8);
                    FixPointer(loc);

                    file.seekg(loc, std::ios_base::beg);
                    file.read(reinterpret_cast<char*>(&m_Globals[i * 0x4000]), pageSize * sizeof(std::uint64_t));
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
                    file.seekg(m_Header.NativesOffset + (i * sizeof(std::uint64_t)), std::ios_base::beg);

                    std::uint64_t hash;
                    file.read(reinterpret_cast<char*>(&hash), 8);
                    Rotl(hash, m_Header.CodeSize + i);

                    m_Natives.push_back(hash);
                }
            }
        }

        // Name
        {
            file.seekg(m_Header.NameOffset, std::ios_base::beg);
            std::getline(file, m_Name, '\0');
            if (size_t pos = m_Name.find_last_of('.'); pos != std::string::npos)
                m_Name.erase(pos);
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
                    file.seekg(m_Header.StringPagesOffset + (i * sizeof(std::uint64_t)), std::ios_base::beg);
                    file.read(reinterpret_cast<char*>(&loc), 8);
                    FixPointer(loc);

                    file.seekg(loc, std::ios_base::beg);
                    file.read(temp.data(), pageSize);

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
    }
}