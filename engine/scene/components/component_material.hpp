#pragma once

#include "core/util.hpp"
#include "render/render_data/texture2d.hpp"

namespace fw {

struct component_material
{
    std::string named_ref = "";
    // ref<material> material_ref;

    // FIXME TEMP
    std::string texture_named_ref = "";
    ref<texture2d> texture_ref = nullptr;
    // FIXME TEMP
};

} // namespace fw