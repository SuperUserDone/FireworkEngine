#pragma once

#include "core/native_script.hpp"
#include "logger.hpp"
#include "rate_limiter.hpp"
#include "render/renderer.hpp"
#include "scene/components.hpp"
#include "scene/scene_manager.hpp"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <string>
#include <thread>

namespace blood {
/**
 * \brief The main game loop
 *
 */
class loop
{
public:
    /**
     * \brief Construct the loop
     *
     */
    loop();
    loop(const std::string &logname, const std::string &name);

    /**
     * \brief Destroy the loop
     *
     */
    virtual ~loop();

    /**
     * \brief Get the renderer
     *
     * \return renderer* pointer to blood::renderer
     */
    renderer *get_renderer() { return m_renderer; }

    /**
     * \brief Get the scenemanager
     *
     * \return std::weak_ptr<scene_manager> refrence to blood::scene_manager
     */
    std::weak_ptr<scene_manager> get_scenemanager() { return m_scene_manager; }
    // std::weak_ptr<void> get_physicsmanager() { return std::make_shared<void>(nullptr); }

    /**
     * \brief Start the update loop
     *
     */
    virtual void start();

    /**
     * \brief Pause the update loop
     *
     */
    virtual void stop();

    /**
     * \brief Set the fps limit for the render loop
     *
     * \param limit The target FPS
     */
    void set_fps_limit(uint16_t limit) { m_fps_target = limit; }

    /**
     * \brief Get the fps limit
     *
     * \return uint16_t the limit
     */
    uint16_t get_fps_limit() { return m_fps_target; }

    virtual void tickloop();

protected:
    virtual void update(double frametime);
    virtual void render(double frametime);

    virtual void destroy_scripts();
    // virtual void load_scripts();

    uint16_t m_fps_target = 0;
    float m_tps_strech = 1.0f;

    renderer *m_renderer = nullptr;
    std::shared_ptr<scene_manager> m_scene_manager = nullptr;

    bool m_stop = false;
    bool m_pause = false;

private:
    loop(const loop &other) = delete;
    loop(loop &&other) = delete;
    loop &operator=(loop &&other) = delete;
    loop &operator=(loop other) = delete;
};

} // namespace blood
