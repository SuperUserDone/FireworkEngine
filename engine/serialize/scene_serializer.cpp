#include "core/types/image2d.hpp"
#include "scene.capnp.h"
#include "scene_serializer.hpp"
#include "vec_readers.hpp"
#include "vfs/loader.hpp"

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

namespace fw {

bool scene_serializer::serialize(scene *ptr, const std::string &vfs_path)
{
    FILE *fp = vfs::vfs_fopen(vfs_path, "wb");

    ::capnp::MallocMessageBuilder msg;

    capnp::Scene::Builder scene_ser = msg.initRoot<capnp::Scene>();

    auto &reg = ptr->get_registry();

    scene_ser.setName(ptr->m_name);
    auto entities = scene_ser.initEntities(reg.alive());

    int entities_count = 0;

    auto &textures = ptr->get_textures();
    auto &meshes = ptr->get_meshes();
    auto &materials = ptr->get_materials();

    auto smeshes = scene_ser.initMeshes().initEntries(meshes.size());
    auto stextures = scene_ser.initTextures().initEntries(textures.size());
    auto smaterials = scene_ser.initMaterials().initEntries(materials.size());

    int i = 0;
    for (auto &&mat : materials) {
        auto mat_ser = smaterials[i];

        auto &[key, value] = mat;

        ref<material> m = value;
        std::string k = key;

        if (m->m_path.empty()) {
            m->m_path = "root://" + k + ".fwmesh";
        }

        action act;

        act.async_action = [m, k]() { return m->save_to_file(); };

        act.load_priority = 0;

        loader::get_instance().queue_action(act);

        mat_ser.setKey(key);
        mat_ser.getValue().setPath(value->m_path);
        i++;
    }

    i = 0;
    for (auto &&mesh_data : meshes) {
        auto mesh_ser = smeshes[i];

        auto [key, value] = mesh_data;

        ref<mesh> m = value;
        std::string k = key;

        if (m->m_path.empty()) {
            m->m_path = "root://" + k + ".fwmesh";
        }

        action act;

        act.async_action = [m, k]() { return m->save_to_file(); };

        act.load_priority = 0;

        loader::get_instance().queue_action(act);

        mesh_ser.setKey(key);
        mesh_ser.getValue().setPath(value->m_path);
        i++;
    }

    i = 0;
    for (auto &&tex : textures) {
        auto tex_ser = stextures[i];
        auto &[key, value] = tex;

        tex_ser.setKey(key);

        if (value->m_path == "") {
            auto size = tex_ser.getValue().getSize();
            write_vec2(glm::uvec2{value->m_x, value->m_y}, size);
        } else {
            tex_ser.getValue().setPath(value->m_path);
        }
        i++;
    }

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

            if (reg.all_of<component_mesh_renderer>(entity)) {
                component_mesh_renderer &mesh = reg.get<component_mesh_renderer>(entity);

                auto mesh_ser = entities[entities_count].initMeshRenderer();
                mesh_ser.setMeshRefrence(mesh.mesh_named_ref);
                mesh_ser.setMatRefrence(mesh.mat_named_ref);

                mesh_ser.setUpdateFreq(mesh.lookup_freq);
            }
        }
        entities_count++;
    });

    ::capnp::writePackedMessageToFd(fileno(fp), msg);
    fclose(fp);
    return true;
}

bool scene_serializer::deserialize(scene *ptr, const std::string &vfs_path)
{
    FILE *fp = vfs::vfs_fopen(vfs_path, "rb");

    if (!fp) {
        LOG_FF("Could not open scene VFS file: {}", vfs_path);
        return false;
    }
    ::capnp::PackedFdMessageReader msg(fileno(fp));

    capnp::Scene::Reader scene_ser = msg.getRoot<capnp::Scene>();

    auto &reg = ptr->get_registry();

    auto meshes = scene_ser.getMeshes().getEntries();
    auto textures = scene_ser.getTextures().getEntries();
    auto materials = scene_ser.getMaterials().getEntries();

    for (auto m : materials) {
        std::string key = m.getKey().cStr();
        std::string path = m.getValue().getPath().cStr();

        auto n_mat = make_ref<material>();

        ptr->get_materials()[key] = n_mat;

        n_mat->load_from_file(path);
    }

    for (auto m : meshes) {
        std::string key = m.getKey().cStr();
        std::string path = m.getValue().getPath().cStr();

        auto n_mesh = make_ref<mesh>();

        ptr->get_meshes()[key] = n_mesh;

        action act;
        act.async_action = [n_mesh, path]() { return n_mesh->load_from_file(path); };
        act.post_sync_action = [n_mesh]() {
            n_mesh->update();
            return true;
        };
        act.load_priority = 32;

        loader::get_instance().queue_action(act);
    }

    for (auto t : textures) {
        std::string key = t.getKey().cStr();
        auto tex = t.getValue();

        if (tex.isPath()) {
            action act;
            std::string path = tex.getPath().cStr();

            ref<image2d> img = make_ref<image2d>();

            act.async_action = [path, img]() { return img->load_from_file(path); };
            act.post_sync_action = [ptr, key, img, path]() {
                auto tex = img->get_as_texture();
                tex->m_path = path;
                ptr->get_textures()[key] = tex;
                return true;
            };
            act.load_priority = 16;

            loader::get_instance().queue_action(act);

        } else {
            auto size_ser = tex.getSize();

            glm::uvec2 size;
            read_vec2(size, size_ser);
            ptr->get_textures()[key] = make_ref<texture2d>();
        }
    }

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

                entity_new.add_component<component_camera>(component_camera{c_fov, c_near, c_far});
            }

            if (a.hasTransform()) {
                auto trans = a.getTransform();
                auto &comp = entity_new.get_component<component_transform>();

                read_vec3(comp.pos, trans.getPos());
                read_vec4(comp.rot, trans.getRot());
                read_vec3(comp.scale, trans.getScale());
            }

            if (a.hasMeshRenderer()) {
                auto mesh = a.getMeshRenderer();
                auto &comp = entity_new.add_component<component_mesh_renderer>();

                comp.mesh_named_ref = mesh.getMeshRefrence().cStr();
                if (!comp.mesh_named_ref.empty())
                    comp.mesh_ref = ptr->m_meshes[comp.mesh_named_ref];

                comp.mat_named_ref = mesh.getMatRefrence().cStr();
                if (!comp.mat_named_ref.empty())
                    comp.material_ref = ptr->m_materials[comp.mat_named_ref];

                comp.lookup_freq = mesh.getUpdateFreq();
            }
        }
    }

    ptr->set_dirty();

    fclose(fp);
    return true;
}
} // namespace fw