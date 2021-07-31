#include "components.hpp"
#include "core/util.hpp"
#include "entity.hpp"

namespace fw {
entity::entity(entt::registry *reg, const std::string &name) : entity(reg, name, get_uuid()) {}

entity::entity(entt::registry *reg, const std::string &name, uint32_t id) : m_reg(reg)
{
    m_id = m_reg->create((entt::entity)id);

    m_reg->emplace<component_tag>(m_id, component_tag{name});
    m_reg->emplace<component_transform>(m_id);
}
entity::~entity() {}

} // namespace fw