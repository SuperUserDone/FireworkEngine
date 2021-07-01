#pragma once

#include "capnp/message.h"
#include "capnp/serialize-packed.h"
#include "scene.capnp.h"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "vfs/vfs.hpp"

#include <iomanip>
#include <string>

namespace blood {

template <typename T, typename B>
static void write_vec3(const T &vec3, B &target)
{
    target.setX(vec3.x);
    target.setY(vec3.y);
    target.setZ(vec3.z);
}

template <typename T, typename B>
static void write_vec4(const T &vec3, B &target)
{
    target.setX(vec3.x);
    target.setY(vec3.y);
    target.setZ(vec3.z);
    target.setW(vec3.w);
}

template <typename T, typename B>
static void write_vec2(const T &vec3, B &target)
{
    target.setX(vec3.x);
    target.setY(vec3.y);
}

template <typename T, typename B>
static void read_vec3(T &vec3, const B &target)
{
    vec3.x = target.getX();
    vec3.y = target.getY();
    vec3.z = target.getZ();
}

template <typename T, typename B>
static void read_vec4(T &vec3, const B &target)
{
    vec3.x = target.getX();
    vec3.y = target.getY();
    vec3.z = target.getZ();
    vec3.w = target.getW();
}

template <typename T, typename B>
static void read_vec2(T &vec2, const B &target)
{
    vec2.x = target.getX();
    vec2.y = target.getY();
}

template <typename T, typename B>
static void write_vertex(const T &vert, B &target)
{
    auto pos = target.getPos();
    auto norm = target.getNorm();
    auto tan = target.getTan();
    auto uvs = target.getUv();

    write_vec3(vert.pos, pos);
    write_vec3(vert.norm, norm);
    write_vec3(vert.tan, tan);
    write_vec2(vert.uvs, uvs);
}

template <typename T, typename B>
static void read_vertex(T &vert, const B &target)
{
    auto pos = target.getPos();
    auto norm = target.getNorm();
    auto tan = target.getTan();
    auto uvs = target.getUv();

    read_vec3(vert.pos, pos);
    read_vec3(vert.norm, norm);
    read_vec3(vert.tan, tan);
    read_vec2(vert.uvs, uvs);
}

class scene_serializer
{
public:
    static inline bool serialize(scene *ptr, const std::string &vfs_path)
    {
        FILE *fp = vfs::vfs_fopen(vfs_path, "w");

        ::capnp::MallocMessageBuilder msg;

        capnp::Scene::Builder scene_ser = msg.initRoot<capnp::Scene>();

        auto &reg = ptr->get_registry();

        scene_ser.setName(ptr->m_name);
        auto entities = scene_ser.initEntities(reg.alive());

        int entities_count = 0;

        reg.each([&](auto entity) {
            if (reg.all_of<component_tag>(entity)) {
                auto &tag = reg.get<component_tag>(entity);

                entities[entities_count].setId((uint32_t)entity);
                entities[entities_count].setName(tag.name);

                if (reg.all_of<component_camera>(entity)) {
                    auto &cam = reg.get<component_camera>(entity);

                    auto cam_ser = entities[entities_count].initCamera();

                    cam_ser.setClipFar(cam.clip_far);
                    cam_ser.setClipNear(cam.clip_near);
                    cam_ser.setFov(cam.fov);
                }

                if (reg.all_of<component_transform>(entity)) {
                    auto &trans = reg.get<component_transform>(entity);

                    auto trans_ser = entities[entities_count].initTransform();

                    auto pos = trans_ser.getPos();
                    auto rot = trans_ser.getRot();
                    auto scale = trans_ser.getScale();

                    write_vec3(trans.pos, pos);
                    write_vec4(trans.rot, rot);
                    write_vec3(trans.scale, scale);
                }

                if (reg.all_of<component_mesh>(entity)) {
                    component_mesh &mesh = reg.get<component_mesh>(entity);

                    auto mesh_ser = entities[entities_count].initMesh();

                    auto vert = mesh_ser.initVerticies(mesh.verticies.size());
                    auto ind = mesh_ser.initIndicies(mesh.indicies.size());

                    int i = 0;

                    for (auto &a : mesh.verticies) {
                        auto vertex = vert[i];

                        write_vertex(a, vertex);
                        i++;
                    }

                    i = 0;
                    for (auto &a : mesh.indicies) {
                        ind.set(i, a);

                        i++;
                    }
                }
            }
            entities_count++;
        });

        ::capnp::writePackedMessageToFd(fileno(fp), msg);
        fclose(fp);
        return true;
    }

    static inline bool deserialize(scene *ptr, const std::string &vfs_path)
    {
        FILE *fp = vfs::vfs_fopen(vfs_path, "r");

        if (!fp) return false;

        ::capnp::PackedFdMessageReader msg(fileno(fp));

        capnp::Scene::Reader scene_ser = msg.getRoot<capnp::Scene>();

        auto &reg = ptr->get_registry();

        if (scene_ser.hasName()) ptr->set_name(scene_ser.getName());
        if (scene_ser.hasEntities()) {
            auto ent = scene_ser.getEntities();

            for (auto a : ent) {
                a.getName();
                auto entity_new = ptr->create_entity(a.getName(), a.getId());

                if (a.hasCamera()) {
                    auto cam = a.getCamera();
                    float c_far = cam.getClipFar();
                    float c_near = cam.getClipNear();
                    float c_fov = std::clamp(cam.getFov(), 45.f, 120.f);

                    entity_new.add_component<component_camera>(
                        component_camera{c_fov, c_near, c_far});
                }

                if (a.hasTransform()) {
                    auto trans = a.getTransform();
                    auto &comp = entity_new.add_component<component_transform>();

                    read_vec3(comp.pos, trans.getPos());
                    read_vec4(comp.rot, trans.getRot());
                    read_vec3(comp.scale, trans.getScale());
                }

                if (a.hasMesh()) {
                    auto mesh = a.getMesh();
                    auto &comp = entity_new.add_component<component_mesh>();

                    auto indicies = mesh.getIndicies();
                    auto verticies = mesh.getVerticies();

                    comp.indicies.reserve(indicies.size());
                    comp.verticies.reserve(verticies.size());

                    for (uint32_t ind : indicies) {
                        comp.indicies.push_back(ind);
                    }

                    for (auto n_vertex : verticies) {
                        blood::vertex vertex_a;
                        read_vertex(vertex_a, n_vertex);
                        comp.verticies.push_back(vertex_a);
                    }

                    comp.update();
                }
            }
        }

        fclose(fp);
        return true;
    }

private:
};
} // namespace blood
