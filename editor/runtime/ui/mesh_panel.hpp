#pragma once

#include "scene/scene_manager.hpp"

class mesh_panel
{
public:
    mesh_panel(fw::scene_manager *man);

    bool m_show = false;

    void update();

    ~mesh_panel();

private:
    fw::scene_manager *m_man;
};
