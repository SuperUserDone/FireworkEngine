#pragma once

#include "entity/entity.hpp"
#include <entt/entt.hpp>

#include <glm/glm.hpp>

namespace blood
{

class entity
{
public:
    entity(entt::registry &reg, const std::string &name);
    ~entity();

    template <typename T, typename... Args>
    T &add_component(Args &&...args);

    template <typename T>
    T &get_component();

    template <typename T>
    bool &has_component() const;

private:
    entt::registry &m_reg;
    entt::entity m_id{entt::null};

private:
    entity() = delete;
    entity(const entity &other) = delete;
    entity(entity &&other) = delete;
    entity &operator=(entity &&other) = delete;
    entity &operator=(entity other) = delete;
};

} // namespace blood
