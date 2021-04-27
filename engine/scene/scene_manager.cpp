#include "scene_manager.hpp"

#include "core/logger.hpp"
#include <mutex>

namespace blood
{

scene_manager::scene_manager() : m_active(new scene()) {}

void scene_manager::stage_scene(scene *p_scene) { m_staging = p_scene; }

void scene_manager::swap()
{
    std::lock_guard<std::mutex> lck(m_acess_mutex);

    if (m_staging != nullptr)
    {
        delete m_active;
        m_active = m_staging;
        m_staging = nullptr;
    }
    else
    {
        LOG_W("Trying to swap scene without initialising staging");
    }
}

scene_manager::~scene_manager()
{
    if (m_staging)
        delete m_staging;

    if (m_active)
        delete m_active;
}

} // namespace blood