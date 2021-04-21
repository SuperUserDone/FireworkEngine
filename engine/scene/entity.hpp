#pragma once

#include <entt/entt.hpp>

namespace blood
{

class entity
{
public:
    entity(entt::registry &reg, const std::string &name);

    template <typename T, typename... Args>
    T &add_component(Args &&...args);

    template <typename T>
    T &get_component();

    template <typename T>
    bool &has_component();

    ~entity();

private:
    entt::registry &m_reg;
    entt::entity m_id;
};

} // namespace blood
