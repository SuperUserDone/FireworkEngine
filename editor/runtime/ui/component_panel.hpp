#pragma once

#include "scene/scene_manager.hpp"

class component_panel
{
public:
    component_panel(fw::scene_manager *man);

    bool m_show = false;

    void update(const uint32_t &curr);

    ~component_panel();

private:
    fw::scene_manager *m_manager;
};