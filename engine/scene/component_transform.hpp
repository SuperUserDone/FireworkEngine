#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct component_transform
{
    glm::vec3 pos = {0, 0, 0};
    glm::quat rot = glm::quat(glm::vec3(0, 0, 0));
    glm::vec3 scale = {1, 1, 1};

    operator glm::mat4()
    {
        glm::mat4 mat(1.f);

        mat = glm::scale(mat, scale);
        mat *= glm::toMat4(rot);
        mat = glm::translate(mat, pos);

        return mat;
    }
};
