#pragma once

#include "core/loop.hpp"
#include "core/util.hpp"
#include "editor_camera.hpp"

class editor_ui;

class editor_loop : public fw::loop
{
public:
    /**
     * \brief Construct the loop
     *
     */
    editor_loop();

    /**
     * \brief Start the update loop
     *
     */
    virtual void start() override;

    /**
     * \brief Pause the update loop
     *
     */
    virtual void stop() override;

    virtual void tickloop() override;

private:
    fw::ref<editor_ui> m_ui;

protected:
    virtual void render(double frametime) override;
};
