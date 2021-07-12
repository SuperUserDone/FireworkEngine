#define GLAD_GL_IMPLEMENTATION

#include "core/logger.hpp"
#include "render_api/render_api.hpp"
#include "render_data/builtin_primitives.hpp"
#include "render_data/builtin_shaders.hpp"
#include "renderer.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"
#include "vfs/loader.hpp"

#include <Tracy.hpp>

#define rapi fw::render_api_impl::get_api()

namespace fw {

renderer::renderer(render_settings &p_render_settings)
    : m_settings(p_render_settings), m_camera(128, nullptr, MODE_DYNAMIC), m_edit_fbo({1, 1}),
      m_fx_fbo({800, 600})
{
    m_quad_mesh.verticies = quad_vertex_data;
    m_quad_mesh.indicies = quad_index_data;

    m_quad_mesh.update();

    m_shaders["fullscreen"] = make_ref<shader_program>(vertex_screen, fragment_screen);
    m_shaders["fullscreen"]->compile();

    m_shaders["flat_texmap"] = make_ref<shader_program>(vertex_std, fragment_texmap);
    m_shaders["flat_texmap"]->compile();

    m_camera.bind_uniform(0);
}

bool renderer::check_close() { return rapi->check_close(); }

void renderer::render(scene *scene)
{
    rapi->begin(glm::vec4(scene->m_back_color, 1.f));

    auto size = rapi->getsize();
    m_fx_fbo.resize(size);
    m_fx_fbo.get_fbo().use();

    m_screen_size = size;

    glm::mat4 view;
    glm::mat4 proj;

    auto cams = scene->m_entt.view<component_camera, component_transform>();
    for (auto c : cams) {
        auto [cam, trans] = cams.get(c);
        proj = cam.get_projection(m_screen_size.x, m_screen_size.y);
        view = glm::inverse((glm::mat4)trans);
    }

    renderpass_geom(scene, view, proj);
    renderpass_postfx(
        m_fx_fbo.get_texture(), &DEFAULT_FRAMEBUFFER, m_shaders["fullscreen"]->get_id());

    rapi->end();
    FrameMark;
}

void renderer::render_editor(scene *scene,
                             glm::mat4 camera_view,
                             glm::mat4 camera_proj,
                             std::function<bool(void)> draw_ui,
                             std::function<glm::uvec2(void)> get_size,
                             std::function<void(size_t)> set_tex_id)
{
    rapi->begin();

    glm::uvec2 size = get_size();
    set_tex_id(*(uint32_t *)m_edit_fbo.get_texture().get_id());

    m_screen_size = size;

    rapi->viewport(size);

    m_fx_fbo.resize(size);
    m_edit_fbo.resize(size);
    m_fx_fbo.get_fbo().use();

    renderpass_geom(scene, camera_view, camera_proj);
    renderpass_postfx(
        m_fx_fbo.get_texture(), m_edit_fbo.get_fbo().get_id(), m_shaders["fullscreen"]->get_id());

    size = rapi->getsize();

    rapi->viewport(size);
    rapi->use_fbo(&DEFAULT_FRAMEBUFFER);
    rapi->draw_imgui(draw_ui);

    rapi->end();
    FrameMark;
}

void renderer::renderpass_geom(scene *scene, glm::mat4 camera_view, glm::mat4 camera_proj)
{
    loader::get_instance().update();

    ZoneScopedN("Geometry pass");
    rapi->clear(glm::vec4(scene->m_back_color, 1.f));

    glm::mat4 arr[2];

    arr[0] = camera_proj;
    arr[1] = camera_view;

    m_camera.set_data(128, arr, MODE_DYNAMIC);

    auto meshes = scene->m_entt.view<component_transform, component_mesh, component_material>();

    for (auto c : meshes) {
        auto [trans, mesh, mat] = meshes.get(c);

        do_lookup(scene, mesh);
        do_lookup(scene, mat);

        shader_program_id m_shader;

        if (mesh.mesh_ref == nullptr) continue;
        if (mat.material_ref == nullptr || mat.material_ref->m_shader_ref == nullptr)
            m_shader = nullptr;
        else
            m_shader = mat.material_ref->m_shader_ref->get_id();

        rapi->draw_elements(mesh.mesh_ref->m_vao.get_id(),
                            mesh.mesh_ref->m_index_buf.get_id(),
                            mesh.mesh_ref->indicies.size(),
                            trans,
                            m_shader,
                            mat.material_ref->m_attribs);
    }
}

void renderer::renderpass_postfx(texture2d &src, framebuffer_id dest, shader_program_id shader)
{
    ZoneScopedN("FX Pass");
    rapi->use_fbo(dest);
    rapi->clear({0, 0, 0, 1});

    glm::mat4 arr[2];

    arr[0] = glm::mat4(1.f);
    arr[1] = glm::mat4(1.f);

    m_camera.set_data(128, arr, MODE_DYNAMIC);

    attribute attr;
    attr.bind_id = 0;
    attr.data.d_texture = src.get_id();
    attr.type = ATTRIB_TYPE_TEXTURE;

    rapi->draw_elements(m_quad_mesh.m_vao.get_id(),
                        m_quad_mesh.m_index_buf.get_id(),
                        m_quad_mesh.indicies.size(),
                        glm::mat4(1.f),
                        shader,
                        {attr});
}

void renderer::do_lookup(scene *scene, component_mesh &mesh)
{
    if (mesh.lookup_count >= mesh.lookup_freq) {
        if (scene->m_meshes.count(mesh.named_ref)) mesh.mesh_ref = scene->m_meshes[mesh.named_ref];
        mesh.lookup_count = 0;
    }
    mesh.lookup_count++;
}

void renderer::do_lookup(scene *scene, component_material &mat)
{
    if (mat.lookup_count >= mat.lookup_freq) {

        if (scene->m_materials.count(mat.named_ref))
            mat.material_ref = scene->m_materials[mat.named_ref];

        if (m_shaders.count(mat.material_ref->m_shader_named_ref)) {
            mat.material_ref->m_shader_ref = m_shaders[mat.material_ref->m_shader_named_ref];

            if (mat.material_ref->m_attribs.size() !=
                mat.material_ref->m_shader_ref->get_attribs().size())
                mat.material_ref->m_attribs = mat.material_ref->m_shader_ref->get_attribs();

            for (auto &&attr : mat.material_ref->m_attribs) {
                if (attr.type == ATTRIB_TYPE_TEXTURE) {
                    if (scene->m_textures.count(attr.data_texture_name))
                        attr.data.d_texture =
                            scene->get_textures()[attr.data_texture_name]->get_id();
                    else
                        attr.data.d_texture = nullptr;
                }
            }
        }
        mat.lookup_count = 0;
    }
    mat.lookup_count++;
}

renderer::~renderer() {}

} // namespace fw