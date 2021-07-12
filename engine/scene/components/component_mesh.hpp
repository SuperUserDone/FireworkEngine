#pragma once

#include "core/types/mesh.hpp"
#include "core/util.hpp"

namespace fw {

struct component_mesh
{
    std::string named_ref;
    ref<mesh> mesh_ref;

    uint32_t lookup_freq = 60;
    uint32_t lookup_count = 0;
};

} // namespace fw