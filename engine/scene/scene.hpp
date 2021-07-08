#pragma once

#include "components.hpp"
#include "core/util.hpp"
#include "entity.hpp"
#include "parallel_hashmap/phmap.h"
#include "render/datatypes.hpp"
#include "render/render_data/texture2d.hpp"

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

    phmap::parallel_flat_hash_map<std::string, ref<texture2d>> &get_textures() { return textures; }
    phmap::parallel_flat_hash_map<std::string, ref<mesh>> &get_meshes() { return meshes; }

    void rebuild_refs();

    void set_background_color(glm::vec3 color) { m_back_color = color; }

    entity create_entity(const std::string &name);
    entity create_entity(const std::string &name, uint32_t id);

    entt::registry &get_registry() { return m_entt; }

private:
    mutable entt::registry m_entt;

    std::string m_name = "Scene";
    glm::vec3 m_back_color = {0.4, 0.6, 0.8};

    phmap::parallel_flat_hash_map<std::string, ref<texture2d>> textures;
    phmap::parallel_flat_hash_map<std::string, ref<mesh>> meshes;
    // TODO materials

private:
    friend class renderer;
    friend class loop;
    friend class ::editor_loop;
    friend class scene_serializer;

    scene(const scene &other) = delete;
    scene &operator=(const scene &other) = delete;
};

} // namespace fw