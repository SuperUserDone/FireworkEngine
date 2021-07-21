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
        m_fx_fbo.get_texture(), &DEFAULT_FRAMEBUFFER, m_shaders["fullscreen"]->get_id(), true);

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
    for (auto &&mat : scene->get_materials()) render_preview(mat.second);

    glm::uvec2 size = get_size();
    set_tex_id(*(uint32_t *)m_edit_fbo.get_texture().get_id());

    m_screen_size = size;

    rapi->viewport(size);

    m_fx_fbo.resize(size);
    m_edit_fbo.resize(size);
    m_fx_fbo.get_fbo().use();

    renderpass_geom(scene, camera_view, camera_proj);
    renderpass_postfx(m_fx_fbo.get_texture(),
                      m_edit_fbo.get_fbo().get_id(),
                      m_shaders["fullscreen"]->get_id(),
                      false);

    size = rapi->getsize();

    rapi->viewport(size);
    rapi->use_fbo(&DEFAULT_FRAMEBUFFER);
    rapi->set_srgb(true);
    rapi->draw_imgui(draw_ui);
    rapi->set_srgb(false);

    rapi->end();
    FrameMark;
}

void renderer::renderpass_geom(scene *scene, glm::mat4 camera_view, glm::mat4 camera_proj)
{
    ZoneScopedN("Geometry pass");
    rapi->clear(glm::vec4(scene->m_back_color, 1.f));

    glm::mat4 arr[2];

    arr[0] = camera_proj;
    arr[1] = camera_view;

    m_camera.set_data(128, arr, MODE_DYNAMIC);

    auto meshes = scene->m_entt.view<component_transform, component_mesh, component_material>();

    for (auto c : meshes) {
        auto [trans, mesh, mat] = meshes.get(c);

        do_lookup(scene, mesh, scene->m_dirty);
        do_lookup(scene, mat, scene->m_dirty);

        scene->m_dirty = false;

        shader_program_id m_shader;

        if (mesh.mesh_ref == nullptr) continue;

        std::vector<attribute> attrs;

        if (mat.material_ref == nullptr || mat.material_ref->m_shader_ref == nullptr) {
            m_shader = nullptr;
            attrs = {};
        } else {
            m_shader = mat.material_ref->m_shader_ref->get_id();
            attrs = mat.material_ref->m_attribs;
        }

        rapi->draw_elements(mesh.mesh_ref->m_vao.get_id(),
                            mesh.mesh_ref->m_index_buf.get_id(),
                            mesh.mesh_ref->indicies.size(),
                            trans,
                            m_shader,
                            attrs);
    }
}

void renderer::renderpass_postfx(texture2d &src,
                                 framebuffer_id dest,
                                 shader_program_id shader,
                                 bool srgb)
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

    rapi->set_srgb(srgb);
    rapi->draw_elements(m_quad_mesh.m_vao.get_id(),
                        m_quad_mesh.m_index_buf.get_id(),
                        m_quad_mesh.indicies.size(),
                        glm::mat4(1.f),
                        shader,
                        {attr});
    rapi->set_srgb(false);
}

void renderer::do_lookup(scene *scene, component_mesh &mesh, bool dirty)
{
    if (mesh.lookup_count >= mesh.lookup_freq || dirty) {
        ZoneScopedN("mesh lookup");
        if (scene->m_meshes.count(mesh.named_ref))
            mesh.mesh_ref = scene->m_meshes[mesh.named_ref];
        else
            mesh.mesh_ref = nullptr;
        mesh.lookup_count = 0;
    }
    mesh.lookup_count++;
}

void renderer::do_lookup(scene *scene, component_material &mat, bool dirty)
{
    if (mat.lookup_count >= mat.lookup_freq || dirty) {
        ZoneScopedN("Material lookup");
        if (scene->m_materials.count(mat.named_ref))
            mat.material_ref = scene->m_materials[mat.named_ref];
        else {
            mat.material_ref = nullptr;
            return;
        }

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
        } else
            mat.material_ref->m_shader_ref = nullptr;

        mat.lookup_count = 0;
    }
    mat.lookup_count++;
}

void renderer::render_preview(ref<material> mat)
{
    if (mat->m_edit_data == nullptr) mat->m_edit_data = make_ref<editor_data>();

    auto &data = mat->m_edit_data;

    if (data->dirty || data->preview_texture == nullptr) {

        data->dirty = false;
        glm::mat4 arr[2];

        // todo cam
        arr[0] = glm::mat4(1.f);
        arr[1] = glm::mat4(1.f);

        m_camera.set_data(128, arr, MODE_DYNAMIC);

        // Todo lights

        shader_program_id m_shader;

        std::vector<attribute> attrs;

        if (mat == nullptr || mat->m_shader_ref == nullptr) {
            return;
        } else {
            m_shader = mat->m_shader_ref->get_id();
            attrs = mat->m_attribs;
        }

        if (data->preview_texture == nullptr) data->preview_texture = make_ref<texture2d>();

        data->preview_texture->set_data(
            data->preview_size, data->preview_size, nullptr, FORMAT_RGB);

        framebuffer fbo;
        rapi->viewport(glm::uvec2(data->preview_size));
        fbo.attach_color({data->preview_texture->get_id()});
        fbo.use();
        rapi->clear(glm::vec4(1.f));

        rapi->draw_elements(m_quad_mesh.m_vao.get_id(),
                            m_quad_mesh.m_index_buf.get_id(),
                            m_quad_mesh.indicies.size(),
                            glm::mat4(1.f),
                            m_shader,
                            attrs);
    }
}

renderer::~renderer() {}

} // namespace fw