#pragma once

#include "core/logger.hpp"
#include "render/datatypes.hpp"

#include <functional>
#include <glm/glm.hpp>
#include <string>

namespace fw {

enum attribute_types
{
    ATTRIB_TYPE_BOOL,

    ATTRIB_TYPE_INT8,
    ATTRIB_TYPE_INT16,
    ATTRIB_TYPE_INT32,
    ATTRIB_TYPE_INT64,

    ATTRIB_TYPE_UINT8,
    ATTRIB_TYPE_UINT16,
    ATTRIB_TYPE_UINT32,
    ATTRIB_TYPE_UINT64,

    ATTRIB_TYPE_FLOAT,
    ATTRIB_TYPE_DOUBLE,

    ATTRIB_TYPE_TEXTURE,

    ATTRIB_TYPE_VEC2F,
    ATTRIB_TYPE_VEC2I,
    ATTRIB_TYPE_VEC2U,

    ATTRIB_TYPE_VEC3F,
    ATTRIB_TYPE_VEC3I,
    ATTRIB_TYPE_VEC3U,

    ATTRIB_TYPE_VEC4F,
    ATTRIB_TYPE_VEC4I,
    ATTRIB_TYPE_VEC4U,
};

static attribute_types glsl_to_typename(const std::string &str)
{
    if (str == "bool") return ATTRIB_TYPE_BOOL;
    if (str == "int") return ATTRIB_TYPE_INT32;
    if (str == "uint") return ATTRIB_TYPE_UINT32;
    if (str == "float") return ATTRIB_TYPE_FLOAT;
    if (str == "double") return ATTRIB_TYPE_DOUBLE;

    if (str == "sampler2D") return ATTRIB_TYPE_TEXTURE;

    if (str == "vec2") return ATTRIB_TYPE_VEC2F;
    if (str == "ivec2") return ATTRIB_TYPE_VEC2I;
    if (str == "uvec2") return ATTRIB_TYPE_VEC2U;

    if (str == "vec4") return ATTRIB_TYPE_VEC4F;
    if (str == "ivec4") return ATTRIB_TYPE_VEC4I;
    if (str == "uvec4") return ATTRIB_TYPE_VEC4U;

    if (str == "vec4") return ATTRIB_TYPE_VEC4F;
    if (str == "ivec4") return ATTRIB_TYPE_VEC4I;
    if (str == "uvec4") return ATTRIB_TYPE_VEC4U;

    FIREWORK_ASSERTF(false, "Shader Type extraction error: {}", str);
    return ATTRIB_TYPE_BOOL;
}

union attribute_data
{
    bool d_bool;

    int8_t d_int8;
    int16_t d_int16;
    int32_t d_int32;
    int64_t d_int64;

    uint8_t d_uint8;
    uint16_t d_uint16;
    uint32_t d_uint32;
    uint64_t d_uint64;

    float d_float;
    double d_double;

    texture_id d_texture;

    glm::vec2 d_vec2f;
    glm::ivec2 d_vec2i;
    glm::uvec2 d_vec2u;

    glm::vec3 d_vec3f;
    glm::ivec3 d_vec3i;
    glm::uvec3 d_vec3u;

    glm::vec4 d_vec4f;
    glm::ivec4 d_vec4i;
    glm::uvec4 d_vec4u;
};

struct attribute
{
    attribute_types type;
    uint32_t bind_id;
    attribute_data data;

    std::string data_texture_name;

    std::string name;
};

} // namespace fw