#pragma once

namespace SCOL
{
    class ScriptFunction
    {
    public:
        static void CallInternal(const joaat_t script, const std::uint32_t pc, const std::uint64_t* args, const std::uint32_t argCount, void* returnValue = nullptr, std::uint32_t returnSize = 0);

        template <typename Ret = void, typename... Args>
        static Ret Call(const joaat_t script, const std::uint32_t pc, Args... args)
        {
            std::uint32_t index{};
            std::array<std::uint64_t, sizeof...(Args)> params{};

            auto PushArg = [&](auto&& value) {
                *reinterpret_cast<std::remove_cv_t<std::remove_reference_t<decltype(value)>>*>(params.data() + index++) = std::forward<decltype(value)>(value);
            };

            (PushArg(std::forward<Args>(args)), ...);

            if constexpr (!std::is_same_v<Ret, void>)
            {
                Ret returnValue{};
                CallInternal(script, pc, params.data(), static_cast<std::uint32_t>(params.size()), &returnValue, sizeof(returnValue));
                return returnValue;
            }
            else
            {
                CallInternal(script, pc, params.data(), static_cast<std::uint32_t>(params.size()));
            }
        }
    };
}