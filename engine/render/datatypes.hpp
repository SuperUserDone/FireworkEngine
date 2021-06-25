#pragma once

#include <stdint.h>

namespace blood {

enum layout_types
{
    LAYOUT_FLOAT,
    LAYOUT_FLOAT2,
    LAYOUT_FLOAT3,
    LAYOUT_FLOAT4,
    LAYOUT_FLOAT_MAT4,
    LAYOUT_INT,
    LAYOUT_INT2,
    LAYOUT_INT3,
    LAYOUT_INT4,
    LAYOUT_UINT,
    LAYOUT_UINT2,
    LAYOUT_UINT3,
    LAYOUT_UINT4
};

enum alloc_mode : uint8_t
{
    MODE_DYNAMIC,
    MODE_STATIC
};

enum shader_type
{
    SHADER_VERTEX,
    SHADER_FRAGMENT,
};

enum color_format
{
    FORMAT_R,
    FORMAT_RG,
    FORMAT_RGB,
    FORMAT_RGBA,
    FORMAT_DEPTH24_STENCIL8,
    FORMAT_DEPTH24,
};

enum texture_filter
{
    FILTER_NEAREST,
    FILTER_LINEAR
};

enum renderbuffer_format
{
    RENDERBUFFER_DEPTH24_STENCIL8,
    RENDERBUFFER_DEPTH24,
};

enum depth_test_mode
{
    TEST_NEVER,
    TEST_LESS,
    TEST_EQUAL,
    TEST_LEQUAL,
    TEST_GREATER,
    TEST_NOTEQUAL,
    TEST_GEQUAL,
    TEST_ALWAYS
};

struct vao_element
{
    layout_types type;
    uint16_t vao_attrib = 0;
    uint16_t size = 0;
    uint16_t offset = 0;
    uint16_t stride = 0;
    bool normalized = false;
};

struct texture_properties
{
    texture_filter min;
    texture_filter mag;
    bool mipmap = false;
};

typedef void *buffer_id;
typedef void *vao_id;
typedef void *framebuffer_id;
typedef void *shader_id;
typedef void *shader_program_id;
typedef void *texture_id;
typedef void *renderbuffer_id;

static constexpr uint32_t get_datatype_size(layout_types type)
{
    switch (type) {
    case LAYOUT_FLOAT:
    case LAYOUT_INT:
    case LAYOUT_UINT:
        return 1;
        break;
    case LAYOUT_FLOAT2:
    case LAYOUT_INT2:
    case LAYOUT_UINT2:
        return 2;
        break;
    case LAYOUT_FLOAT3:
    case LAYOUT_INT3:
    case LAYOUT_UINT3:
        return 3;
        break;
    case LAYOUT_FLOAT4:
    case LAYOUT_INT4:
    case LAYOUT_UINT4:
        return 4;
        break;
    case LAYOUT_FLOAT_MAT4:
        return 4;
        break;
    }
    return 0;
}

}; // namespace blood