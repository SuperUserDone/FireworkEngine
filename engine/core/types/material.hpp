#pragma once

#include "core/util.hpp"
#include "editor_data.hpp"
#include "render/render_data/shader_program.hpp"
#include "render/shader_attribs.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

class material_panel;
namespace fw {

class material
{
public:
    material();

    bool save_to_file();
    bool load_from_file(const std::string &vfs_path);

    std::vector<attribute> &get_attribs() { return m_attribs; }

    ~material();

    std::string m_shader_named_ref = "";

private:
    std::string m_path = "";

    ref<shader_program> m_shader_ref;

    std::vector<attribute> m_attribs;

    ref<editor_data> m_edit_data = nullptr;

private:
    friend class renderer;
    friend class scene_serializer;
    friend class ::material_panel;
};

} // namespace fw