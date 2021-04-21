#include "entity.hpp"

#include "component_tag.hpp"

namespace blood
{
entity::entity(entt::registry &reg, const std::string &name) : m_reg(reg)
{
    m_id = m_reg.create();

    m_reg.emplace<component_tag>(m_id, component_tag{name, 0});
}

template <typename T, typename... Args>
T &entity::add_component(Args &&...args)
{
    if (has_component<T>())
        throw;

    return m_reg.emplace<T>(m_id, std::forward(args...));
}

template <typename T>
T &entity::get_component()
{
    return m_reg.get<T>(m_id);
}

entity::~entity() {}
} // namespace blood