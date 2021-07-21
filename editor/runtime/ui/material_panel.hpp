#pragma once

#include "scene/scene_manager.hpp"

class material_panel
{
public:
    material_panel(fw::scene_manager *man);

    bool m_show = true;

    void update();

    ~material_panel();

private:
    fw::scene_manager *m_man;

    float m_size = 128.f;
};
