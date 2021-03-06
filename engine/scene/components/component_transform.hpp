#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace fw {

struct component_transform
{
    glm::vec3 pos = {0, 0, 0};
    glm::quat rot = glm::quat(glm::vec3(0, 0, 0));
    glm::vec3 scale = {1, 1, 1};

    void set_matrix(const glm::mat4 &mat)
    {
        glm::vec3 p;
        glm::vec4 q;
        glm::decompose(mat, scale, rot, pos, p, q);
    }

    operator glm::mat4() const
    {
        glm::mat4 mat(1.f);

        mat = glm::translate(glm::mat4(1.f), pos) * glm::toMat4(rot) *
              glm::scale(glm::mat4(1.f), scale);

        return mat;
    }
};

} // namespace fw
