#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

#include "component_mesh.hpp"
#include "scene.hpp"

namespace blood
{

class scene_manager
{
public:
    scene_manager();

    void stage_scene_file(const std::string &scene_file);
    void stage_scene(scene *p_scene);

    void swap();

    scene *get_active_scene() { return m_active; }
    scene *get_staging_scene() { return m_staging; }

    ~scene_manager();

private:
    scene *m_active;
    scene *m_staging = nullptr;

private:
    scene_manager(const scene_manager &other) = delete;
    scene_manager(scene_manager &&other) = delete;
    scene_manager &operator=(scene_manager &&other) = delete;
    scene_manager &operator=(scene_manager other) = delete;
};

} // namespace blood
