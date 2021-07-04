#pragma once

#include "entity.hpp"

#include <entt/entt.hpp>
#include <mutex>
#include <vector>

class editor_loop;

namespace fw {

class scene
{
public:
    scene();
    scene(scene &&other);
    scene &operator=(scene &&other);
    ~scene();

    void set_name(const std::string &name) { m_name = name; }
    std::string get_name() { return m_name; }

    bool loading_done();

    void set_background_color(glm::vec3 color) { m_back_color = color; }

    entity create_entity(const std::string &name);
    entity create_entity(const std::string &name, uint32_t id);

    entt::registry &get_registry() { return m_entt; }

private:
    mutable entt::registry m_entt;

    std::string m_name = "Scene";

    glm::vec3 m_back_color = {0.4, 0.6, 0.8};

    friend class renderer;
    friend class loop;
    friend class ::editor_loop;
    friend class scene_serializer;

private:
    scene(const scene &other) = delete;
    scene &operator=(const scene &other) = delete;
};

} // namespace fw