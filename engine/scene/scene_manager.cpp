#include "scene_manager.hpp"

#include "core/logger.hpp"

namespace blood
{

scene_manager::scene_manager() { m_active = std::make_shared<scene>(); }

void scene_manager::stage_scene_file(const std::string &scene_file)
{
    // TODO
}

void scene_manager::stage_scene(std::shared_ptr<scene> scene)
{
    m_staging = scene;

    m_staging->start_load();
}

bool scene_manager::staging_ready()
{

    if (m_staging)
        return m_staging->loading_done();
    else
    {
        LOG_W("Checking null scene.");
        return false;
    }
}

void scene_manager::swap_on_ready()
{
    if (staging_ready())
        swap();
}

void scene_manager::swap()
{
    m_active = m_staging;
    m_staging = nullptr;
}

scene_manager::~scene_manager()
{
    // TODO
}

} // namespace blood