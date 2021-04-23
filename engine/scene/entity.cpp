#include "entity.hpp"

#include "component_tag.hpp"

#include "core/logger.hpp"

namespace blood
{
entity::entity(entt::registry &reg, const std::string &name) : m_reg(reg)
{
    m_id = m_reg.create();

    m_reg.emplace<component_tag>(m_id, component_tag{name});
}
entity::~entity()
{ // TODO
}

template <typename T, typename... Args>
T &entity::add_component(Args &&...args)
{
    BLOODENGINE_ASSERT(!has_component<T>(), "Entity already has component.");

    return m_reg.emplace<T>(m_id, std::forward(args...));
}

template <typename T>
T &entity::get_component()
{
    BLOODENGINE_ASSERT(has_component<T>(), "Entity does have component.");

    return m_reg.get<T>(m_id);
}

template <typename T>
bool &entity::has_component() const
{
    return m_reg.all_of<T>(m_id);
}

} // namespace blood