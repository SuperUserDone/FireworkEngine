#pragma once

#include "scene/scene_manager.hpp"

class scene_tree
{
public:
    scene_tree(fw::scene_manager *man);

    bool m_show = true;

    void update(uint64_t &curr);

    ~scene_tree();

private:
    void add_entity();
    void remove_entity(uint64_t &id);

    fw::scene_manager *m_man;
};