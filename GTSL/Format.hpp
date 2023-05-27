#pragma once

#include <format>
#include "Math/Math.hpp"
#include "Math/AxisAngle.hpp"

template<>
struct std::formatter<GTSL::AxisAngle>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const GTSL::AxisAngle& p, FormatContext& ctx)
    {
        return format_to(ctx.out(), "<X:{} Y:{} Z:{} Angle:{}>", p.X, p.Y, p.Z, p.Angle);

    }
};

template<>
struct std::formatter<GTSL::Vector2>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const GTSL::Vector2& p, FormatContext& ctx)
    {
        return format_to(ctx.out(), "<X:{} Y:{}>", p.X(), p.Y());

    }
};

template<>
struct std::formatter<GTSL::Vector3>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const GTSL::Vector3& p, FormatContext& ctx)
    {
        return format_to(ctx.out(), "<X:{} Y:{} Z:{}>", p.X(), p.Y(),p.Z());

    }
};

template<>
struct std::formatter<GTSL::Rotator>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const GTSL::Rotator& p, FormatContext& ctx)
    {
        return format_to(ctx.out(), "<X:{} Y:{} Z:{}>", p.X, p.Y, p.Z);

    }
};

template<>
struct std::formatter<GTSL::Vector4>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const GTSL::Vector4& p, FormatContext& ctx)
    {
        return format_to(ctx.out(), "<X:{} Y:{} Z:{} W:{}>", p.X(), p.Y(), p.Z(), p.W());

    }
};

template<>
struct std::formatter<GTSL::Quaternion>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const GTSL::Quaternion& p, FormatContext& ctx)
    {
        return format_to(ctx.out(), "<X:{} Y:{} Z:{} W:{}>", p.X(), p.Y(), p.Z(), p.W());

    }
};

template<>
struct std::formatter<GTSL::Extent2D>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const GTSL::Extent2D& p, FormatContext& ctx)
    {
        return format_to(ctx.out(), "<Width:{} Height:{} AspectRatio:{}>", p.Width,p.Height,p.GetAspectRatio());

    }
};

template<>
struct std::formatter<GTSL::Extent3D>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const GTSL::Extent3D& p, FormatContext& ctx)
    {
        return format_to(ctx.out(), "<Width:{} Height:{} Depth:{}>", p.Width, p.Height, p.Depth);

    }
};