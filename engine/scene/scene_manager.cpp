#include "scene_manager.hpp"

#include "core/logger.hpp"

namespace blood
{

scene_manager::scene_manager() : m_active() {}

void scene_manager::stage_scene_file(const std::string &scene_file)
{
    // TODO
}

void scene_manager::stage_scene(scene &p_scene)
{
    m_staging = std::move(p_scene);

    m_staging.start_load();
}

bool scene_manager::staging_ready() { return m_staging.loading_done(); }

void scene_manager::set_swap_on_ready() { m_swap_on_ready = true; }

void scene_manager::swap()
{
    m_active = std::move(m_staging);
    m_staging = scene();
}

scene_manager::~scene_manager()
{
    // TODO
}

} // namespace blood