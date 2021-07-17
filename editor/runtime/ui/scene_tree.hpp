#pragma once

#include "scene/scene_manager.hpp"

class scene_tree
{
public:
    scene_tree(fw::scene_manager *man);

    bool m_show = false;

    void update(uint32_t &curr);

    ~scene_tree();

private:
    fw::scene_manager *m_man;
};