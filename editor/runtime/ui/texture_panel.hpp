#pragma once

#include "parts/modal.hpp"
#include "scene/scene_manager.hpp"

class texture_panel
{
public:
    texture_panel(fw::scene_manager *man);

    bool m_show = false;

    void update();

    ~texture_panel();

private:
    fw::scene_manager *m_man;

    std::string m_name;

    modal m_open;
    modal m_open_error;
};
