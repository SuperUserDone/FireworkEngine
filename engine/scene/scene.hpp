#pragma once

#include <entt/entt.hpp>
#include <mutex>
#include <vector>

#include "entity.hpp"
#include "render/deletion_helpers.hpp"

namespace blood
{

class scene
{
public:
    scene();
    scene(const std::string &filename);
    ~scene();

    void start_load();
    bool loading_done();

    void set_background_color(glm::vec3 color) { m_back_color = color; }

    entity create_entity(const std::string &name);

    std::mutex m_scene_mutex;

private:
    entt::registry m_entt;

    glm::vec3 m_back_color = {0.4, 0.6, 0.8};

    friend class renderer;
    friend class loop;

private:
    scene(const scene &other) = delete;
    scene(scene &&other) = delete;
    scene &operator=(scene other) = delete;
    scene &operator=(scene &&other) = delete;
};

} // namespace blood