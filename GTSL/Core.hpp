#pragma once

#include <cstdint>

#pragma once

#if (_DEBUG)
extern void onAssert(bool condition, const char* text, int line, const char* file, const char* function);
#endif

#ifdef _DEBUG
#define GTSL_ASSERT(condition, text) if(!(condition)) [[unlikely]] { onAssert(condition, text, __LINE__, __FILE__, __FUNCTION__); __debugbreak(); }
#else
#define GTSL_ASSERT(condition, text)
#endif

namespace GTSL
{
    typedef std::int8_t byte;

    typedef std::int8_t int8;
    typedef std::int16_t int16;
    typedef std::int32_t int32;
    typedef std::int64_t int64;

    typedef std::uint8_t uint8;
    typedef std::uint16_t uint16;
    typedef std::uint32_t uint32;
    typedef std::uint64_t uint64;

    template<typename T>
    struct RemoveReference
    {
        using type = T;
        using const_type_ref = const T;
    };

    template<typename T>
    struct RemoveReference<T&>
    {
        using type = T;
        using const_type_ref = const T&;
    };

    template<typename T>
    struct RemoveReference<T&&>
    {
        using type = T;
        using const_type_ref = const T&&;
    };

    template<typename T>
    [[nodiscard]] constexpr T&& ForwardRef(typename RemoveReference<T>::type& args) noexcept { return static_cast<T&&>(args); }

    template<typename T>
    [[nodiscard]] constexpr typename RemoveReference<T>::type&& MoveRef(T&& args) noexcept { return static_cast<typename RemoveReference<T>::type&&>(args); }

    template<typename T, typename R>
    int64 RangeForIndex(const T& e, const R& range) { return &e - range.begin(); }
}
