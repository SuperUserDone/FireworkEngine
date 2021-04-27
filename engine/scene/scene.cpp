#include "render/deletion_helpers.hpp"
#include "scene.hpp"
#include "scene/component_mesh.hpp"

namespace blood
{
scene::scene()
{
    // TODO
}

scene::scene(scene &&other) : m_back_color(other.m_back_color), m_entt(std::move(other.m_entt)) {}

scene &scene::operator=(scene &&other)
{
    if (this != &other)
    {
        m_entt = std::move(other.m_entt);
        m_back_color = other.m_back_color;
    }

    return *this;
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

bool scene::loading_done()
{
    // TODO
    return true;
}

entity scene::create_entity(const std::string &name) { return entity(&m_entt, name); }
} // namespace blood