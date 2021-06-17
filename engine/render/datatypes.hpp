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

static constexpr uint32_t get_datatype_size(layout_types type)
{
    switch (type) {
    case LAYOUT_FLOAT:
    case LAYOUT_INT:
    case LAYOUT_UINT:
        return 4;
        break;
    case LAYOUT_FLOAT2:
    case LAYOUT_INT2:
    case LAYOUT_UINT2:
        return 4 * 2;
        break;
    case LAYOUT_FLOAT3:
    case LAYOUT_INT3:
    case LAYOUT_UINT3:
        return 4 * 3;
        break;
    case LAYOUT_FLOAT4:
    case LAYOUT_INT4:
    case LAYOUT_UINT4:
        return 4 * 4;
        break;
    case LAYOUT_FLOAT_MAT4:
        return 4 * 16;
        break;
    }
    return 0;
}

}; // namespace blood