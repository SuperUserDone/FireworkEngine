#pragma once

#include "components.hpp"
#include "scene.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

namespace fw {

class scene_manager
{
public:
    scene_manager();

    void stage_scene_file(const std::string &scene_file);
    void stage_scene(scene *p_scene);

    void swap() { m_swap = true; }

    scene *get_active_scene()
    {
        if (m_swap) do_swap();
        return m_active;
    }
    scene *get_staging_scene() { return m_staging; }

    ~scene_manager();

private:
    scene *m_active;
    scene *m_staging = nullptr;

    std::atomic_bool m_swap{false};

    void do_swap();

private:
    scene_manager(const scene_manager &other) = delete;
    scene_manager(scene_manager &&other) = delete;
    scene_manager &operator=(scene_manager &&other) = delete;
    scene_manager &operator=(scene_manager other) = delete;
};

} // namespace fw
