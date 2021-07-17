#pragma once

#include "scene/scene_manager.hpp"

class material_panel
{
public:
    material_panel(fw::scene_manager *man);

    bool m_show = false;

    void update();

    ~material_panel();

private:
    fw::scene_manager *m_man;
};
