#include "../scene.hpp"
#include "serialize/scene_serializer.hpp"

static testing::AssertionResult is_attr_equal(fw::attribute &lhs, fw::attribute &rhs)
{
    switch (lhs.type) {
    case fw::ATTRIB_TYPE_BOOL:
        if (lhs.data.d_bool == rhs.data.d_bool) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(bool)";
    case fw::ATTRIB_TYPE_INT:
        if (lhs.data.d_int == rhs.data.d_int) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(int)";
    case fw::ATTRIB_TYPE_UINT:
        if (lhs.data.d_uint == rhs.data.d_uint) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(uint)";
    case fw::ATTRIB_TYPE_FLOAT:
        if (lhs.data.d_float == rhs.data.d_float) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(float)";
    case fw::ATTRIB_TYPE_DOUBLE:
        if (lhs.data.d_double == rhs.data.d_double) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(double)";
    case fw::ATTRIB_TYPE_TEXTURE:
        if (lhs.data_texture_name == rhs.data_texture_name) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(Texture)";
    case fw::ATTRIB_TYPE_VEC2F:
        if (lhs.data.d_vec2f == rhs.data.d_vec2f) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(vec3f)";
    case fw::ATTRIB_TYPE_VEC2I:
        if (lhs.data.d_vec2i == rhs.data.d_vec2i) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(vec2i)";
    case fw::ATTRIB_TYPE_VEC2U:
        if (lhs.data.d_vec2u == rhs.data.d_vec2u) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(vec2u)";
    case fw::ATTRIB_TYPE_VEC3F:
        if (lhs.data.d_vec3f == rhs.data.d_vec3f) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(vec3f)";
    case fw::ATTRIB_TYPE_VEC3I:
        if (lhs.data.d_vec3i == rhs.data.d_vec3i) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(vec3i)";
    case fw::ATTRIB_TYPE_VEC3U:
        if (lhs.data.d_vec3u == rhs.data.d_vec3u) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(vec3u)";
    case fw::ATTRIB_TYPE_VEC4F:
        if (lhs.data.d_vec4f == rhs.data.d_vec4f) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(vec4f)";
    case fw::ATTRIB_TYPE_VEC4I:
        if (lhs.data.d_vec4i == rhs.data.d_vec4i) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(vec4i)";
    case fw::ATTRIB_TYPE_VEC4U:
        if (lhs.data.d_vec4u == rhs.data.d_vec4u) return testing::AssertionSuccess();
        return testing::AssertionFailure() << "(vec4u)";
        break;
    }
}

TEST_F(scene_test, SceneSerializeDeserialize)
{
    fw::scene_serializer::serialize(&one, "root://save.fwscn");

    fw::scene_serializer::deserialize(&two, "root://save.fwscn");

    while (!fw::loader::get_instance().is_empty()) {
        fw::loader::get_instance().update();
    }

    EXPECT_STREQ(two.get_name().c_str(), one.get_name().c_str());

    EXPECT_EQ(two.get_materials().size(), one.get_materials().size());
    EXPECT_EQ(two.get_meshes().size(), one.get_meshes().size());

    for (auto &a : one.get_meshes()) {
        auto &[k, v] = a;

        EXPECT_TRUE(two.get_meshes().count(k));
    }

    for (auto &a : one.get_materials()) {
        auto &[k, v] = a;

        EXPECT_TRUE(two.get_materials().count(k));

        auto &v2 = two.get_materials()[k];

        EXPECT_EQ(v2->get_attribs().size(), v->get_attribs().size());
        EXPECT_STREQ(v2->m_shader_named_ref.c_str(), v->m_shader_named_ref.c_str());

        for (int i = 0; i < v2->get_attribs().size(); i++) {
            EXPECT_FLOAT_EQ(v2->get_attribs()[i].stepsize, v->get_attribs()[i].stepsize);
            EXPECT_FLOAT_EQ(v2->get_attribs()[i].bind_id, v->get_attribs()[i].bind_id);
            EXPECT_EQ(v2->get_attribs()[i].type, v->get_attribs()[i].type);
            EXPECT_FLOAT_EQ(v2->get_attribs()[i].max, v->get_attribs()[i].max);
            EXPECT_FLOAT_EQ(v2->get_attribs()[i].min, v->get_attribs()[i].min);
            EXPECT_EQ(v2->get_attribs()[i].name, v->get_attribs()[i].name);

            EXPECT_TRUE(is_attr_equal(v2->get_attribs()[i], v->get_attribs()[i]));
        }
    }
    // TODO Textures

    auto &reg1 = one.get_registry();
    auto &reg2 = two.get_registry();

    EXPECT_EQ(reg2.size(), reg1.size());

    reg1.each([&reg1, &reg2](entt::entity ent) {
        auto &tag1 = reg1.get<fw::component_tag>(ent);
        auto &tag2 = reg2.get<fw::component_tag>(ent);
        EXPECT_EQ(tag1.name, tag2.name);

        auto &trans1 = reg1.get<fw::component_transform>(ent);
        auto &trans2 = reg2.get<fw::component_transform>(ent);

        EXPECT_EQ(trans2.pos, trans1.pos);
        EXPECT_EQ(trans2.rot, trans1.rot);
        EXPECT_EQ(trans2.scale, trans1.scale);

        auto &cam1 = reg1.get<fw::component_camera>(ent);
        auto &cam2 = reg2.get<fw::component_camera>(ent);

        EXPECT_EQ(cam2.clip_far, cam1.clip_far);
        EXPECT_EQ(cam2.clip_near, cam1.clip_near);
        EXPECT_EQ(cam2.fov, cam1.fov);

        auto &render1 = reg1.get<fw::component_mesh_renderer>(ent);
        auto &render2 = reg2.get<fw::component_mesh_renderer>(ent);

        EXPECT_EQ(render2.mesh_named_ref, render1.mesh_named_ref);
        EXPECT_EQ(render2.mat_named_ref, render1.mat_named_ref);
        EXPECT_EQ(render2.lookup_freq, render1.lookup_freq);
    });
}