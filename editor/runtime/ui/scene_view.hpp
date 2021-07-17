#pragma once

#include "glm/glm.hpp"

class scene_view
{
public:
    scene_view();

    bool m_show = false;

    void update(glm::uvec2 &size, size_t id);

    ~scene_view();

private:
};