#pragma once

#include "imgui.h"

#include <string>

static inline const std::string utf8chr(int cp)
{
    char c[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    if (cp <= 0x7F) {
        c[0] = cp;
    } else if (cp <= 0x7FF) {
        c[0] = (cp >> 6) + 192;
        c[1] = (cp & 63) + 128;
    } else if (0xd800 <= cp && cp <= 0xdfff) {
    } // invalid block of utf8
    else if (cp <= 0xFFFF) {
        c[0] = (cp >> 12) + 224;
        c[1] = ((cp >> 6) & 63) + 128;
        c[2] = (cp & 63) + 128;
    } else if (cp <= 0x10FFFF) {
        c[0] = (cp >> 18) + 240;
        c[1] = ((cp >> 12) & 63) + 128;
        c[2] = ((cp >> 6) & 63) + 128;
        c[3] = (cp & 63) + 128;
    }
    return std::string(c);
}

static inline ImWchar range_all[] = {0xe000, 0xff00, 0x2300, 0x2f00, 0x0};

static inline ImVector<ImWchar> get_ranges()
{
    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(range_all);
    builder.BuildRanges(&ranges);

    return ranges;
}
static inline ImVector<ImWchar> get_large_ranges()
{
    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddChar(0xf6a6);
    builder.AddChar(0xf071);
    builder.BuildRanges(&ranges);

    return ranges;
}