#pragma once

#include "core/loop.hpp"
#include "editor_camera.hpp"
#include "editor_ui.hpp"
#include "render/renderer.hpp"
#include "scene/components.hpp"
#include "scene/scene_manager.hpp"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <thread>

class editor_loop : public blood::loop
{
public:
    /**
     * \brief Construct the loop
     *
     */
    editor_loop();
    ~editor_loop() override;

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
    editor_camera m_edit_cam;

    std::shared_ptr<editor_ui> m_ui;

protected:
    virtual void render(double frametime) override;
};
