#pragma once

#include "core/settings.hpp"
#include "render_api_none.hpp"

//#include <opengl/render_api_opengl.hpp>

// class render_api;

namespace blood {

class render_api_impl
{
public:
    static render_api *init(render_settings &m_settings);
    static render_api *get_api();
    static void delete_api();

private:
    static render_api *m_api;
};

} // namespace blood