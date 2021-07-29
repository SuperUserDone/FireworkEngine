#pragma once

#include "parts/modal.hpp"
#include "scene/scene_manager.hpp"

class mesh_panel
{
public:
    mesh_panel(fw::scene_manager *man);

    bool m_show = true;

    void update();

    ~mesh_panel();

private:
    fw::scene_manager *m_man;

    modal m_open;
    modal m_open_error;

    std::string m_name = "";

    float m_size = 128.f;
};
