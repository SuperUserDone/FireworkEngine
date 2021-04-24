#include "render/deletion_helpers.hpp"
#include "scene.hpp"
#include "scene/component_mesh.hpp"

namespace blood
{
scene::scene()
{
    // TODO
}

scene::~scene()
{
    auto view = m_entt.view<component_mesh>();

    for (auto entity : view)
    {
        auto mesh = view.get<component_mesh>(entity);

        mesh.queue_delete();
    }
}

void scene::start_load()
{
    // TODO
}

bool scene::loading_done()
{
    // TODO
    return true;
}

entity scene::create_entity(const std::string &name) { return entity(m_entt, name); }
} // namespace blood