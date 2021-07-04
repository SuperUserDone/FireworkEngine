#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace fw {

struct component_camera
{
    float fov = 90.f;
    float clip_near = 0.1f;
    float clip_far = 10000.f;

    glm::mat4 get_projection(int sizex, int sizey) const
    {
        return glm::perspective(
            glm::radians(fov), (float)sizex / (float)sizey, clip_near, clip_far);
    }
};

} // namespace fw
