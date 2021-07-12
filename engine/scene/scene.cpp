#include "render/renderer.hpp"
#include "scene.hpp"
#include "scene/components.hpp"

namespace fw {
scene::scene() {}

scene::scene(scene &&other) : m_back_color(other.m_back_color), m_entt(std::move(other.m_entt)) {}

scene &scene::operator=(scene &&other)
{
    if (this != &other) {
        m_entt = std::move(other.m_entt);
        m_back_color = other.m_back_color;
    }

    return *this;
}

scene::~scene() {}

void scene::rebuild_refs() { FIREWORK_ASSERT(false, "not implemented"); }

entity scene::create_entity(const std::string &name) { return entity(&m_entt, name); }
entity scene::create_entity(const std::string &name, uint32_t id)
{
    return entity(&m_entt, name, id);
}
} // namespace fw