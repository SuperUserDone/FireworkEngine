#pragma once

#include "scene/component_camera.hpp"
#include "scene/component_transform.hpp"

struct editor_camera
{
    glm::vec3 target = {0, 0, 0};
    glm::vec3 pos = {5, 5, 5};

    blood::component_camera cam;

    operator glm::mat4() { return glm::lookAt(pos, target, {0, 1, 0}); }

    void update(double dt) {}
};