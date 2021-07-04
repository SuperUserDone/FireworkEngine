#pragma once

#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "vfs/vfs.hpp"

#include <iomanip>
#include <string>

namespace fw {

class scene_serializer
{
public:
    static bool serialize(scene *ptr, const std::string &vfs_path);

    static bool deserialize(scene *ptr, const std::string &vfs_path);

private:
};
} // namespace fw
