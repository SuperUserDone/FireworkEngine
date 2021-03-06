#pragma once

#include "scene/entity.hpp"

class editor_loop;

namespace fw {

class native_script
{
public:
    native_script() {}
    virtual ~native_script() {}

    virtual void on_start() {}
    virtual void on_tick_update(double detlatime) {}
    virtual void on_render_update(double deltatime) {}
    virtual void on_destroy() {}

protected:
    entity m_entity;
    friend class loop;
    friend class ::editor_loop;
};

} // namespace fw