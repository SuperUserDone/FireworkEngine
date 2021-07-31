#pragma once

#include "core/types/material.hpp"
#include "core/types/mesh.hpp"
#include "core/util.hpp"
#include "render/render_data/texture2d.hpp"

namespace fw {

struct component_mesh_renderer
{
    std::string mesh_named_ref = "";
    ref<mesh> mesh_ref;

    std::string mat_named_ref = "";
    ref<material> material_ref;

    uint32_t lookup_freq = 60;
    uint32_t lookup_count = 0;
};

} // namespace fw