#pragma once

#include <entt/entt.hpp>
#include <mutex>
#include <vector>

#include "entity.hpp"
#include "render/deletion_helpers.hpp"

class editor_loop;

namespace blood
{

class scene
{
public:
    scene();
    scene(scene &&other);
    scene &operator=(scene &&other);
    ~scene();

    bool loading_done();

    void set_background_color(glm::vec3 color) { m_back_color = color; }

    entity create_entity(const std::string &name);

    std::mutex m_scene_mutex;

    entt::registry &get_registry() { return m_entt; }

private:
    entt::registry m_entt;

    glm::vec3 m_back_color = {0.4, 0.6, 0.8};

    friend class renderer;
    friend class loop;
    friend class ::editor_loop;

private:
    scene(const scene &other) = delete;
    scene &operator=(const scene &other) = delete;
};

} // namespace blood