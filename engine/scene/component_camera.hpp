#pragma once

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/matrix.hpp"
#include <glm/ext/matrix_transform.hpp>

namespace blood
{

struct component_camera
{
    glm::vec3 pos = {0, 0, 3};
    glm::quat rot = glm::quat({0, 0, 0});

    float fov = 90.f;
    float clip_near = 0.1f;
    float clip_far = 10000.f;

    glm::mat4 get_projection(int sizex, int sizey)
    {
        return glm::perspective(
            glm::radians(fov), (float)sizex / (float)sizey, clip_near, clip_far);
    }

    operator glm::mat4()
    {
        glm::mat4 mat(1.f);

        mat *= glm::toMat4(rot);
        mat = glm::translate(mat, pos);

        return glm::inverse(mat);
    }
};

} // namespace blood
