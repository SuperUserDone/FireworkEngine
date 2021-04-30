#pragma once

#include <json.hpp>
#include <string>

#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "vfs/vfs.hpp"

namespace glm
{
using namespace nlohmann;
static void to_json(json &j, const glm::vec3 &p) { j = json::array({p.x, p.y, p.z}); }
static void from_json(const json &j, glm::vec3 &p) { p = {j[0], j[1], j[2]}; }
static void to_json(json &j, const glm::vec2 &p) { j = json::array({p.x, p.y}); }
static void from_json(const json &j, glm::vec2 &p) { p = {j[0], j[1]}; }
static void to_json(json &j, const glm::quat &p) { j = json::array({p.x, p.y, p.z, p.w}); }
static void from_json(const json &j, glm::quat &p) { p = glm::quat(j[3], j[0], j[1], j[2]); }
} // namespace glm

namespace blood
{
using namespace nlohmann;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vertex, pos, norm, tan, uvs);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(component_transform, pos, rot, scale);

static void to_json(json &j, const component_camera &p)
{
    j["clipf"] = p.clip_far;
    j["clipn"] = p.clip_near;
    j["fov"] = p.fov;
}
static void from_json(const json &j, component_camera &p)
{
    p.clip_far = j.at("clipf");
    p.clip_near = j.at("clipn");
    p.fov = std::clamp((float)j.at("fov"), 30.f, 120.f);
}
static void to_json(json &j, const component_mesh &p)
{
    j["verticies"] = p.verticies;
    j["indicies"] = p.indicies;
}
static void from_json(const json &j, component_mesh &p)
{
    j.at("verticies").get_to(p.verticies);
    j.at("indicies").get_to(p.indicies);
}

class scene_serializer
{
public:
    static inline bool serialize(scene *ptr, const std::string &vfs_path)
    {
        using namespace nlohmann;
        json j;

        j["scene"]["name"] = "untitled";
        auto &scene = j["scene"];

        auto &reg = ptr->get_registry();

        auto view = reg.view<blood::component_tag>();
        for (auto entity : view)
        {
            auto &tag = view.get<blood::component_tag>(entity);

            json ent;
            ent["name"] = tag.name;
            ent["id"] = entity;

            blood::entity entity_obj(&reg, entity);

            if (entity_obj.has_component<blood::component_transform>())
            {
                auto &trans = entity_obj.get_component<blood::component_transform>();

                ent["components"]["transform"] = trans;
            }
            if (entity_obj.has_component<blood::component_camera>())
            {
                auto &comp = entity_obj.get_component<blood::component_camera>();

                ent["components"]["camera"] = comp;
            }
            if (entity_obj.has_component<blood::component_mesh>())
            {
                auto &comp = entity_obj.get_component<blood::component_mesh>();

                ent["components"]["mesh"] = comp;
            }

            scene["entities"].push_back(ent);
        }

        LOG_IF("Saving Scene to {}({})", vfs_path, vfs::vfs_resolve_path(vfs_path));

        std::ofstream o(vfs::vfs_resolve_path(vfs_path));
        o << std::setw(4) << j;
        o.close();

        return true;
    }

    static inline bool deserialize(scene *ptr, const std::string &vfs_path)
    {
        LOG_IF("Loading Scene from {}({})", vfs_path, vfs::vfs_resolve_path(vfs_path));
        std::ifstream i(vfs::vfs_resolve_path(vfs_path));
        json j;
        i >> j;
        i.close();

        auto &entities = j["scene"]["entities"];

        for (auto &&ent : entities)
        {
            if (ptr->get_registry().valid((entt::entity)ent["id"]))
                return false;

            auto entity = ptr->create_entity(ent["name"], ent["id"]);
            auto &&comp = ent["components"];
            if (!comp["camera"].is_null())
            {
                entity.add_component<blood::component_camera>(
                    (blood::component_camera)comp["camera"]);
            }
            if (!comp["transform"].is_null())
            {
                entity.add_component<blood::component_transform>(
                    (blood::component_transform)comp["transform"]);
            }
            if (!comp["mesh"].is_null())
            {
                entity.add_component<blood::component_mesh>((blood::component_mesh)comp["mesh"]);
            }
        }

        return true;
    }

private:
};
} // namespace blood
