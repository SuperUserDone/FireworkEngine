#pragma once

#include "parts/modal.hpp"
#include "scene/scene_manager.hpp"

class material_panel
{
public:
    material_panel(fw::scene_manager *man);

    bool m_show = true;

    void update();
    void draw_edit();
    void draw_all();

    ~material_panel();

private:
    fw::scene_manager *m_man;

    std::string m_add_name = "";

    modal m_add;
    modal m_error;

    float m_size = 128.f;

    bool m_editing = false;

    fw::ref<fw::material> m_editing_ref = nullptr;
    std::string m_editing_name = "";
};
