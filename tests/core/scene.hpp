#pragma once

#include "scene/scene.hpp"
#include "vfs/loader.hpp"
#include "vfs/vfs.hpp"

#include <gtest/gtest.h>
#include <random>

static std::random_device dev;
static std::mt19937 engine;

static std::uniform_real_distribution<double> dist_d;
static std::uniform_int_distribution<uint64_t> dist_u;
static std::uniform_int_distribution<int64_t> dist_i;

static inline void setup_random()
{
    engine.seed(dev());

    dist_d = std::uniform_real_distribution<double>(-1.0, 1.0);
    dist_u = std::uniform_int_distribution<uint64_t>(0, ULLONG_MAX);
    dist_i = std::uniform_int_distribution<int64_t>(LLONG_MIN, LLONG_MAX);
}

static inline double random_double() { return dist_d(engine); }
static inline uint64_t random_uint() { return dist_u(engine); }
static inline int64_t random_int() { return dist_i(engine); }

static inline void random_value(fw::attribute_data &d, fw::attribute_types type)
{
    switch (type) {
    case fw::ATTRIB_TYPE_BOOL:
        d.d_bool = (bool)random_uint() % 2;
        break;
    case fw::ATTRIB_TYPE_INT:
        d.d_int = random_int();
        break;
    case fw::ATTRIB_TYPE_UINT:
        d.d_uint = random_uint();
        break;
    case fw::ATTRIB_TYPE_FLOAT:
        d.d_float = random_double();
        break;
    case fw::ATTRIB_TYPE_DOUBLE:
        d.d_double = random_double();
        break;
    case fw::ATTRIB_TYPE_TEXTURE:
        break;
    case fw::ATTRIB_TYPE_VEC2F:
        d.d_vec2f = {random_double(), random_double()};
        break;
    case fw::ATTRIB_TYPE_VEC2I:
        d.d_vec2i = {random_int(), random_int()};
        break;
    case fw::ATTRIB_TYPE_VEC2U:
        d.d_vec2u = {random_uint(), random_uint()};
        break;
    case fw::ATTRIB_TYPE_VEC3F:
        d.d_vec3f = {random_double(), random_double(), random_double()};
        break;
    case fw::ATTRIB_TYPE_VEC3I:
        d.d_vec3i = {random_int(), random_int(), random_int()};
        break;
    case fw::ATTRIB_TYPE_VEC3U:
        d.d_vec3u = {random_uint(), random_uint(), random_uint()};
        break;
    case fw::ATTRIB_TYPE_VEC4F:
        d.d_vec4f = {random_double(), random_double(), random_double(), random_double()};
        break;
    case fw::ATTRIB_TYPE_VEC4I:
        d.d_vec4i = {random_int(), random_int(), random_int(), random_int()};
        break;
    case fw::ATTRIB_TYPE_VEC4U:
        d.d_vec4u = {random_uint(), random_uint(), random_uint(), random_uint()};
        break;
    }
}

class scene_test : public ::testing::Test
{

protected:
    void SetUp() override
    {
        setup_random();
        fw::vfs::vfs_register("./test_data/ignore", "root");

        fw::render_settings set;
        set.renderer_api = fw::RENDERER_NONE;

        fw::render_api_impl::init(set);

        auto entone = one.create_entity("Ent1");
        auto enttwo = one.create_entity("Ent2");

        auto &entone_render = entone.add_component<fw::component_mesh_renderer>();
        entone_render.mesh_named_ref = "Mesh1";
        entone_render.mat_named_ref = "Mat1";

        auto &entone_cam = entone.add_component<fw::component_camera>();
        entone_cam.clip_far = 100.f;
        entone_cam.clip_near = 0.005f;
        entone_cam.fov = 68.f;

        auto &enttwo_render = enttwo.add_component<fw::component_mesh_renderer>();
        enttwo_render.mesh_named_ref = "Mesh2";
        enttwo_render.mat_named_ref = "Mat2";
        enttwo_render.lookup_freq = 10;

        auto &enttwo_cam = enttwo.add_component<fw::component_camera>();
        enttwo_cam.clip_far = 1000.f;
        enttwo_cam.clip_near = 0.05f;
        enttwo_cam.fov = 67.f;

        auto &transone = entone.get_component<fw::component_transform>();
        transone.pos = {3.f, 3.f, 3.f};
        transone.rot = glm::quat(glm::vec3(30.f, 30.f, 30.f));
        transone.scale = {0.5f, 0.5f, 0.5f};

        one.get_materials()["Mat1"] = fw::make_ref<fw::material>();
        one.get_materials()["Mat2"] = fw::make_ref<fw::material>();

        fw::attribute attr;

        for (int i = 0; i < (fw::attribute_types::ATTRIB_TYPE_VEC4U + 1); i++) {

            attr.bind_id = random_uint();
            attr.min = random_double();
            attr.max = random_double();
            attr.stepsize = random_double();
            attr.type = (fw::attribute_types)i;

            random_value(attr.data, attr.type);
            one.get_materials()["Mat1"]->get_attribs().push_back(attr);
        }

        one.get_meshes()["Mesh1"] = fw::make_ref<fw::mesh>();
        one.get_meshes()["Mesh2"] = fw::make_ref<fw::mesh>();
    }

    void TearDown() override { fw::loader::get_instance().de_init(); }

    fw::scene one;
    fw::scene two;
};