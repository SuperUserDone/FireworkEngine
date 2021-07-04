#include "core/logger.hpp"
#include "scene_manager.hpp"

#include <mutex>

namespace fw {

scene_manager::scene_manager() : m_active(new scene()) {}

void scene_manager::stage_scene(scene *p_scene)
{
    if (m_staging) {
        delete m_staging;
    }
    m_staging = p_scene;
}

void scene_manager::do_swap()
{
    if (m_staging != nullptr) {
        scene *temp = m_active;
        m_active = m_staging;
        m_staging = temp;
    } else {
        LOG_W("Trying to swap scene without initialising staging");
    }

    m_swap = false;
}

scene_manager::~scene_manager()
{
    if (m_staging) delete m_staging;

    if (m_active) delete m_active;
}

} // namespace fw