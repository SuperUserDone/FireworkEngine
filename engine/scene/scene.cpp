#include "scene.hpp"

namespace blood
{
scene::scene() {}

scene::~scene() {}

entity scene::create(const std::string &name) { return entity(m_entt, name); }
} // namespace blood