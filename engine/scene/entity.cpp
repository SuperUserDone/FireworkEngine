#include "entity.hpp"

#include "component_tag.hpp"
#include "core/util.hpp"

namespace blood
{
entity::entity(entt::registry *reg, const std::string &name) : m_reg(reg)
{
    m_id = m_reg->create((entt::entity)get_uuid());

    m_reg->emplace<component_tag>(m_id, component_tag{name});
}
entity::~entity()
{ // TODO
}

} // namespace blood