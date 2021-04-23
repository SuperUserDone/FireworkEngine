#include "scene.hpp"

namespace blood
{
scene::scene()
{
    // TODO
}

scene::~scene()
{
    // TODO
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

void scene::unload()
{
    // TODO
}

entity scene::create_entity(const std::string &name) { return entity(m_entt, name); }
} // namespace blood