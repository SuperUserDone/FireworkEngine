#pragma once

#include "render/render_api/render_api.hpp"

#include <cstring>

namespace fw {

class texture2d
{
public:
    texture2d() { m_id = render_api_impl::get_api()->alloc_texture2d(); }
    texture2d(uint32_t x, uint32_t y, const void *data, color_format format) : texture2d()
    {
        set_data(x, y, data, format);
    }
    texture2d(uint32_t x, uint32_t y, color_format format) : texture2d(x, y, nullptr, format) {}

    texture2d(texture2d &&other) { *this = std::move(other); }
    texture2d(const texture2d &other) { *this = other; }
    ~texture2d()
    {
        if (m_id != nullptr) fw::render_api_impl::get_api()->delete_texture2d(m_id);
    }

    texture2d &operator=(texture2d &&other)
    {
        if (this != &other) {
            m_x = other.m_x;
            m_y = other.m_y;
            m_id = other.m_id;
            m_mag = other.m_mag;
            m_min = other.m_min;
            m_mipmap = other.m_mipmap;
            m_has_data = other.m_has_data;
            m_immutable = other.m_immutable;

            other.m_id = nullptr;
        }
        return *this;
    }

    texture2d &operator=(const texture2d &other)
    {
        if (this != &other) {
            m_id = fw::render_api_impl::get_api()->alloc_texture2d();

            m_x = other.m_x;
            m_y = other.m_y;
            m_mag = other.m_mag;
            m_min = other.m_min;
            m_mipmap = other.m_mipmap;
            m_has_data = other.m_has_data;
            m_immutable = other.m_immutable;

            // TODO copy texture
        }
        return *this;
    }

    texture_id get_id() const { return m_id; }

    void set_mipmap(bool state) { m_mipmap = state; }
    bool get_mipmap() { return m_mipmap; }

    void set_min_filter(texture_filter filter) { m_min = filter; }
    void set_mag_filter(texture_filter filter) { m_mag = filter; }

    texture_filter get_min_filter() { return m_min; }
    texture_filter get_mag_filter() { return m_mag; }

    void set_data(int x, int y, const void *data, color_format format)
    {
        m_has_data = true;
        if (format == FORMAT_DEPTH24_STENCIL8 || m_immutable) {
            m_immutable = true;
            if (m_id != nullptr) render_api_impl::get_api()->delete_texture2d(m_id);
            m_id = render_api_impl::get_api()->alloc_texture2d();
        }

        texture_properties props;

        props.mag = m_min;
        props.min = m_mag;
        props.mipmap = m_mipmap;

        m_x = x;
        m_y = y;
        m_format = format;

        render_api_impl::get_api()->set_texture2d_data(m_id, m_x, m_y, m_format, data, props);
    }

public:
    std::string m_path = "";

private:
    texture_filter m_min = FILTER_LINEAR;
    texture_filter m_mag = FILTER_LINEAR;

    bool m_mipmap = false;
    bool m_has_data = false;
    bool m_immutable = false;

    color_format m_format;

    texture_id m_id;

    uint32_t m_x;
    uint32_t m_y;

private:
    friend class image2d;
    friend class scene_serializer;
};

}; // namespace fw