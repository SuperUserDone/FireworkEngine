#include "core/logger.hpp"
#include "shader_program.hpp"

#include <sstream>

namespace fw {

static bool read_bool(const char *str)
{
    int val = 0;

    if (sscanf(str, "%d", &val)) return val;
    if (strcmp(str, "false")) return false;
    if (strcmp(str, "true")) return true;

    FIREWORK_ASSERTF(false, "Could not parse bool value {}", str);
    return false;
}

static void set_attr_data(attribute &attr, char *str)
{
    switch (attr.type) {
    case ATTRIB_TYPE_BOOL:
        attr.data.d_bool = read_bool(str);
        break;

    case ATTRIB_TYPE_INT8:
        FIREWORK_ASSERTF(sscanf(str, "%hhd", &attr.data.d_int8), "Could not parse value {}", str);
        break;
    case ATTRIB_TYPE_INT16:
        FIREWORK_ASSERTF(sscanf(str, "%hd", &attr.data.d_int16), "Could not parse value {}", str);
        break;
    case ATTRIB_TYPE_INT32:
        FIREWORK_ASSERTF(sscanf(str, "%d", &attr.data.d_int32), "Could not parse value {}", str);
        break;
    case ATTRIB_TYPE_INT64:
        FIREWORK_ASSERTF(sscanf(str, "%ld", &attr.data.d_int64), "Could not parse value {}", str);
        break;

    case ATTRIB_TYPE_UINT8:
        FIREWORK_ASSERTF(sscanf(str, "%hhu", &attr.data.d_uint8), "Could not parse value {}", str);
        break;
    case ATTRIB_TYPE_UINT16:
        FIREWORK_ASSERTF(sscanf(str, "%hu", &attr.data.d_uint16), "Could not parse value {}", str);
        break;
    case ATTRIB_TYPE_UINT32:
        FIREWORK_ASSERTF(sscanf(str, "%u", &attr.data.d_uint32), "Could not parse value {}", str);
        break;
    case ATTRIB_TYPE_UINT64:
        FIREWORK_ASSERTF(sscanf(str, "%lu", &attr.data.d_uint64), "Could not parse value {}", str);
        break;

    case ATTRIB_TYPE_FLOAT:
        FIREWORK_ASSERTF(sscanf(str, "%f", &attr.data.d_float), "Could not parse value {}", str);
    case ATTRIB_TYPE_DOUBLE:
        FIREWORK_ASSERTF(sscanf(str, "%lf", &attr.data.d_double), "Could not parse value {}", str);
        break;

    case ATTRIB_TYPE_TEXTURE:
        attr.data.d_texture = nullptr;
        break;

    case ATTRIB_TYPE_VEC2F:
        FIREWORK_ASSERTF(
            sscanf(str, "vec2(%f,%f)", &attr.data.d_vec2f.x, &attr.data.d_vec2f.y) == 2,
            "Could not parse value {}",
            str);
        break;
    case ATTRIB_TYPE_VEC2I:
        FIREWORK_ASSERTF(
            sscanf(str, "ivec2(%d,%d)", &attr.data.d_vec2i.x, &attr.data.d_vec2i.y) == 2,
            "Could not parse value {}",
            str);
        break;
    case ATTRIB_TYPE_VEC2U:
        FIREWORK_ASSERTF(
            sscanf(str, "uvec2(%u,%u)", &attr.data.d_vec2u.x, &attr.data.d_vec2u.y) == 2,
            "Could not parse value {}",
            str);
        break;

    case ATTRIB_TYPE_VEC3F:
        FIREWORK_ASSERTF(sscanf(str,
                                "vec3(%f, %f, %f)",
                                &attr.data.d_vec3f.x,
                                &attr.data.d_vec3f.y,
                                &attr.data.d_vec3f.z) == 3,
                         "Could not parse value {}",
                         str);
        break;
    case ATTRIB_TYPE_VEC3I:
        FIREWORK_ASSERTF(sscanf(str,
                                "ivec3(%d, %d, %d)",
                                &attr.data.d_vec3i.x,
                                &attr.data.d_vec3i.y,
                                &attr.data.d_vec3i.z) == 3,
                         "Could not parse value {}",
                         str);
        break;
    case ATTRIB_TYPE_VEC3U:
        FIREWORK_ASSERTF(sscanf(str,
                                "uvec3(%u, %u, %u)",
                                &attr.data.d_vec3u.x,
                                &attr.data.d_vec3u.y,
                                &attr.data.d_vec3u.z) == 3,
                         "Could not parse value {}",
                         str);
        break;

    case ATTRIB_TYPE_VEC4F:
        FIREWORK_ASSERTF(sscanf(str,
                                "vec4(%f, %f, %f, %f)",
                                &attr.data.d_vec4f.x,
                                &attr.data.d_vec4f.y,
                                &attr.data.d_vec4f.z,
                                &attr.data.d_vec4f.w) == 4,
                         "Could not parse value {}",
                         str);
        break;
    case ATTRIB_TYPE_VEC4I:
        FIREWORK_ASSERTF(sscanf(str,
                                "ivec4(%i, %i, %i, %i)",
                                &attr.data.d_vec4i.x,
                                &attr.data.d_vec4i.y,
                                &attr.data.d_vec4i.z,
                                &attr.data.d_vec4i.w) == 4,
                         "Could not parse value {}",
                         str);
        break;
    case ATTRIB_TYPE_VEC4U:
        FIREWORK_ASSERTF(sscanf(str,
                                "uvec4(%u, %u, %u, %u)",
                                &attr.data.d_vec4u.x,
                                &attr.data.d_vec4u.y,
                                &attr.data.d_vec4u.z,
                                &attr.data.d_vec4u.w) == 4,
                         "Could not parse value {}",
                         str);
        break;
    }
}

void shader_program::compile()
{
    shader_id vert = render_api_impl::get_api()->alloc_shader(SHADER_VERTEX);
    shader_id frag = render_api_impl::get_api()->alloc_shader(SHADER_FRAGMENT);

    preprocess(m_vert_shader_src);
    preprocess(m_frag_shader_src);

    render_api_impl::get_api()->compile_shader(vert, m_vert_shader_src.c_str());
    render_api_impl::get_api()->compile_shader(frag, m_frag_shader_src.c_str());

    render_api_impl::get_api()->add_shader_to_program(m_id, vert);
    render_api_impl::get_api()->add_shader_to_program(m_id, frag);

    render_api_impl::get_api()->link_shader_program(m_id);

    render_api_impl::get_api()->delete_shader(frag);
    render_api_impl::get_api()->delete_shader(vert);
}

void shader_program::preprocess(const std::string &src)
{
    std::stringstream ss(src);

    for (std::string text; std::getline(ss, text);) {
        if (text.substr(0, 9) == "// Export") {
            std::getline(ss, text);

            char str[65];
            char name[65];
            char default_val[129];
            uint32_t id;

            if (sscanf(text.c_str(), "layout(binding = %u) uniform %64s %64s", &id, str, name) ==
                    3 ||
                sscanf(text.c_str(),
                       "layout(location = %u) uniform %64s %64s = %128[^;]s",
                       &id,
                       str,
                       name,
                       default_val) == 4) {

                attribute attr;
                attr.bind_id = id;
                attr.type = glsl_to_typename(str);

                if (default_val[0] == 0 && attr.type != ATTRIB_TYPE_TEXTURE)
                    LOG_WF("Adding a default value is required for exproting a value! {}", text);

                if (name[strlen(name) - 1] == ';') name[strlen(name) - 1] = 0x0;

                set_attr_data(attr, default_val);

                attr.name = std::string(name);

                m_attribs.push_back(attr);

                LOG_DF("Added shader property: {} {} {}", name, id, str);
            } else {
                LOG_WF("Could not parse shader prop line: {}", text);
            }
        }
    }
}

} // namespace fw