#define GLAD_GL_IMPLEMENTATION

#include "core/logger.hpp"
#include "render_api/render_api.hpp"
#include "renderer.hpp"
#include "scene/components.hpp"
#include "scene/scene.hpp"

#define rapi blood::render_api_impl::get_api()

namespace blood {

renderer::renderer(const render_settings &p_render_settings)
{
    render_api_impl::init(p_render_settings);
    m_camera = new buffer(128, nullptr, MODE_DYNAMIC);
    m_camera->bind_uniform(0);
}

bool renderer::check_close() { return rapi->check_close(); }

void renderer::render(const scene *scene)
{
    rapi->begin(glm::vec4(scene->m_back_color, 1.f));

    auto size = rapi->getsize();

    auto cams = scene->m_entt.view<component_camera, component_transform>();
    for (auto c : cams) {
        auto [cam, trans] = cams.get(c);

        glm::mat4 arr[2];

        arr[0] = cam.get_projection(size.x, size.y);
        arr[1] = glm::inverse((glm::mat4)trans);

        m_camera->set_data(128, arr, MODE_DYNAMIC);
    }

    auto meshes = scene->m_entt.view<component_transform, component_mesh>();

    for (auto c : meshes) {
        auto [trans, mesh] = meshes.get(c);

        rapi->draw_elements(mesh.m_vao.get_id(),
                            mesh.m_index_buf.get_id(),
                            mesh.indicies.size(),
                            trans,
                            nullptr,
                            {});
    }

    rapi->end();
}

void renderer::render_editor(const scene *scene)
{
    rapi->begin();
    rapi->end();
}

renderer::~renderer() { delete m_camera; }

} // namespace blood