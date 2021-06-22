#include "opengl/render_api_opengl.hpp"
#include "render_api.hpp"
#include "render_api_none.hpp"

namespace blood {

render_api *render_api_impl::m_api = nullptr;

render_api *render_api_impl::init(render_settings &settings)
{
    switch (settings.renderer_api) {
    case RENDERER_NONE:
        m_api = new render_api(settings);
        break;
    case RENDERER_OPENGL33:
        m_api = new render_api_opengl(settings);
        break;
    }

    return m_api;
}

render_api *render_api_impl::get_api()
{
    BLOODENGINE_ASSERT(m_api, "API does not exist");
    return m_api;
}

void render_api_impl::delete_api()
{
    BLOODENGINE_ASSERT(m_api, "API does not exist");
    switch (m_api->get_impl()) {
    case RENDERER_NONE:
        delete (render_api *)m_api;
        break;
    case RENDERER_OPENGL33:
        delete (render_api_opengl *)m_api;
        break;
    }
}

} // namespace blood