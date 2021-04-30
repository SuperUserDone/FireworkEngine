#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "core/logger.hpp"

namespace blood
{

class entity
{
public:
    entity() {}
    entity(entt::registry *reg, const std::string &name);
    entity(entt::registry *reg, const std::string &name, uint32_t id);
    entity(entt::registry *reg, entt::entity id) : m_reg(reg), m_id(id) {}

    entity(entity &&other) = default;
    entity &operator=(entity &&other) = default;

    ~entity();

    template <typename T, typename... Args>
    T &add_component(Args &&...args)
    {
        BLOODENGINE_ASSERT(!has_component<T>(), "Entity already has component.");

        return m_reg->emplace<T>(m_id, std::forward<Args>(args)...);
    }

    template <typename T>
    T &get_component()
    {
        BLOODENGINE_ASSERT(has_component<T>(), "Entity does have component.");

        return m_reg->get<T>(m_id);
    }

    template <typename T>
    bool has_component() const
    {
        return m_reg->all_of<T>(m_id);
    }

private:
    entt::registry *m_reg = nullptr;
    entt::entity m_id{entt::null};

private:
    entity(const entity &other) = delete;
    entity &operator=(const entity &other) = delete;
};

} // namespace blood
