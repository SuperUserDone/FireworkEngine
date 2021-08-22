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

    ATTRIB_TYPE_INT,
    ATTRIB_TYPE_UINT,

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
    if (str == "int") return ATTRIB_TYPE_INT;
    if (str == "uint") return ATTRIB_TYPE_UINT;
    if (str == "float") return ATTRIB_TYPE_FLOAT;
    if (str == "double") return ATTRIB_TYPE_DOUBLE;

    if (str == "sampler2D") return ATTRIB_TYPE_TEXTURE;

    if (str == "vec2") return ATTRIB_TYPE_VEC2F;
    if (str == "ivec2") return ATTRIB_TYPE_VEC2I;
    if (str == "uvec2") return ATTRIB_TYPE_VEC2U;

    if (str == "vec3") return ATTRIB_TYPE_VEC3F;
    if (str == "ivec3") return ATTRIB_TYPE_VEC3I;
    if (str == "uvec3") return ATTRIB_TYPE_VEC3U;

    if (str == "vec4") return ATTRIB_TYPE_VEC4F;
    if (str == "ivec4") return ATTRIB_TYPE_VEC4I;
    if (str == "uvec4") return ATTRIB_TYPE_VEC4U;

    FIREWORK_ASSERTF(false, "Shader Type extraction error: {}", str);
    return ATTRIB_TYPE_BOOL;
}

union attribute_data
{
    bool d_bool;

    int64_t d_int;
    uint64_t d_uint;

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

    float stepsize = 1.f;
    float min = FLT_MIN;
    float max = FLT_MAX;

    std::string data_texture_name;

    std::string name;
};

} // namespace fw