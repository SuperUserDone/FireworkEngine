#pragma once

#include <atomic>
#include <memory>
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
    void stage_scene(std::shared_ptr<scene> scene);

    bool staging_ready();

    void set_swap_on_ready();
    void swap();

    std::weak_ptr<scene> get_active_scene()
    {
        if (m_swap_on_ready && staging_ready())
            swap();

        return m_active;
    }
    std::weak_ptr<scene> get_staging_scene() { return m_staging; }

    ~scene_manager();

private:
    std::shared_ptr<scene> m_active;
    std::shared_ptr<scene> m_staging;

    std::atomic_bool m_swap_on_ready{false};

private:
    scene_manager(const scene_manager &other) = delete;
    scene_manager(scene_manager &&other) = delete;
    scene_manager &operator=(scene_manager &&other) = delete;
    scene_manager &operator=(scene_manager other) = delete;
};

} // namespace blood
