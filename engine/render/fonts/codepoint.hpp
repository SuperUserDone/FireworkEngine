#pragma once

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

#define CODEPOINT_SETUI_BEGIN       0xe5fa
#define CODEPOINT_SETUI_END         0xe62b
#define CODEPOINT_DEVICONS_BEGIN    0xe700
#define CODEPOINT_DEVICONS_END      0xe7c5
#define CODEPOINT_FONTAWESOME_BEGIN 0xf000
#define CODEPOINT_FONTAWESOME_END   0xf2e0
#define CODEPOINT_FONTAWESOME_EX_BEGIN
#define CODEPOINT_FONTAWESOME_EX_END
#define CODEPOINT_MATERIAL_DESIGN_BEGIN
#define CODEPOINT_MATERIAL_DESIGN_END
#define CODEPOINT_WEATHER_BEGIN
#define CODEPOINT_WEATHER_END
#define CODEPOINT_OCTICONS_BEGIN
#define CODEPOINT_OCTICONS_END
#define CODEPOINT_OCTICONS_ADD_0
#define CODEPOINT_OCTICONS_ADD_1
#define CODEPOINT_OCTICONS_ADD_2
#define CODEPOINT__BEGIN
#define CODEPOINT__END
