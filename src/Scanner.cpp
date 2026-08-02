#include "Scanner.hpp"

namespace SCOL
{
    static std::vector<std::optional<std::uint8_t>> ParsePattern(std::string_view pattern)
    {
        std::vector<std::optional<std::uint8_t>> bytes;
        bytes.reserve((pattern.size() + 1) / 3);

        auto Hex = [](char c) -> std::uint8_t {
            return c <= '9' ? c - '0' : ((c | 0x20) - 'a' + 0xA);
        };

        for (std::size_t i = 0; i < pattern.size();)
        {
            if (pattern[i] == ' ')
            {
                ++i;
                continue;
            }

            if (pattern[i] == '?')
                bytes.emplace_back(std::nullopt);
            else
                bytes.emplace_back(static_cast<std::uint8_t>((Hex(pattern[i]) << 4) | Hex(pattern[i + 1])));

            i += 2;
        }

        return bytes;
    }

    std::optional<Memory> Scanner::ScanPattern(const char* name, const char* pattern)
    {
        auto hash = Joaat(name);
        if (m_CachedResults.contains(hash))
            return m_CachedResults[hash];

        uint8_t* base = reinterpret_cast<uint8_t*>(GetModuleHandleA(0));

        PIMAGE_DOS_HEADER dos = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
        PIMAGE_NT_HEADERS nt = reinterpret_cast<PIMAGE_NT_HEADERS>(base + dos->e_lfanew);

        auto parsed = ParsePattern(pattern);

        size_t moduleSize = nt->OptionalHeader.SizeOfImage;
        size_t patternSize = parsed.size();

        for (size_t i = 0; i < moduleSize - patternSize; i++)
        {
            bool match = true;
            for (size_t j = 0; j < patternSize; j++)
            {
                if (parsed[j].has_value() && base[i + j] != parsed[j].value())
                {
                    match = false;
                    break;
                }
            }

            if (match)
            {
                auto result = Memory(base + i);
                auto rva = result.As<std::uintptr_t>() - Memory(base).As<std::uintptr_t>();
                LOGF(INFO, "Found pattern for {} at GTA5_Enhanced.exe+0x{:X}.", name, rva);
                m_CachedResults[hash] = result;
                return result;
            }
        }

        LOGF(FATAL, "Failed to find pattern for {}.", name);
        return std::nullopt;
    }

    void Scanner::Add(const char* name, const char* pattern, const ScanFunc& func)
    {
        m_Patterns.push_back({name, pattern, func});
    }

    bool Scanner::Scan()
    {
        bool success = true;

        for (auto& pattern : m_Patterns)
        {
            if (auto addr = ScanPattern(pattern.m_Name.c_str(), pattern.m_Pattern.c_str()))
                pattern.m_Func(*addr);
            else
                success = false;
        }

        return success;
    }
}