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
    void stage_scene(scene &p_scene);

    bool staging_ready();

    void set_swap_on_ready();
    void swap();

    void swap_on_ready();

    scene &get_active_scene()
    {
        std::lock_guard<std::mutex> lck(m_acess_mutex);

        return m_active;
    }
    scene &get_staging_scene() { return m_staging; }

    ~scene_manager();

private:
    std::mutex m_acess_mutex;

    scene m_active;
    scene m_staging;

    std::atomic_bool m_swap_on_ready{false};

private:
    scene_manager(const scene_manager &other) = delete;
    scene_manager(scene_manager &&other) = delete;
    scene_manager &operator=(scene_manager &&other) = delete;
    scene_manager &operator=(scene_manager other) = delete;
};

} // namespace blood
