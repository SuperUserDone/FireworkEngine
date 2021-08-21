#pragma once

#include "scene/components.hpp"

struct editor_camera
{
    glm::vec3 target = {0, 0, 0};
    glm::vec3 pos = {3, 0, 3};

    fw::component_camera cam;

    operator glm::mat4() { return glm::lookAt(pos, target, {0, 1, 0}); }

    void update(double dt) {}
};
