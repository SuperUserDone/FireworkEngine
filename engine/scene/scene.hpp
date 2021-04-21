#pragma once

#include <entt/entt.hpp>

#include "entity.hpp"

namespace blood
{

class scene
{
public:
    scene();
    ~scene();

    entity create(const std::string &name);

private:
    entt::registry m_entt;
};
} // namespace blood
