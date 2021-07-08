#pragma once

namespace fw {

template <typename T, typename B>
static void convert_vec3(const T &vec3, B &target)
{
    target.x = vec3.x;
    target.y = vec3.y;
    target.z = vec3.z;
}

template <typename T, typename B>
static void convert_vec4(const T &vec3, B &target)
{
    target.x = vec3.x;
    target.y = vec3.y;
    target.z = vec3.z;
    target.w = vec3.w;
}

template <typename T, typename B>
static void convert_vec2(const T &vec3, B &target)
{
    target.x = vec3.x;
    target.y = vec3.y;
}

template <typename T, typename B>
static void write_vec3(const T &vec3, B &target)
{
    target.setX(vec3.x);
    target.setY(vec3.y);
    target.setZ(vec3.z);
}

template <typename T, typename B>
static void write_vec4(const T &vec3, B &target)
{
    target.setX(vec3.x);
    target.setY(vec3.y);
    target.setZ(vec3.z);
    target.setW(vec3.w);
}

template <typename T, typename B>
static void write_vec2(const T &vec3, B &target)
{
    target.setX(vec3.x);
    target.setY(vec3.y);
}

template <typename T, typename B>
static void read_vec3(T &vec3, const B &target)
{
    vec3.x = target.getX();
    vec3.y = target.getY();
    vec3.z = target.getZ();
}

template <typename T, typename B>
static void read_vec4(T &vec3, const B &target)
{
    vec3.x = target.getX();
    vec3.y = target.getY();
    vec3.z = target.getZ();
    vec3.w = target.getW();
}

template <typename T, typename B>
static void read_vec2(T &vec2, const B &target)
{
    vec2.x = target.getX();
    vec2.y = target.getY();
}

template <typename T, typename B>
static void write_vertex(const T &vert, B &target)
{
    auto pos = target.getPos();
    auto norm = target.getNorm();
    auto tan = target.getTan();
    auto uvs = target.getUv();

    write_vec3(vert.pos, pos);
    write_vec3(vert.norm, norm);
    write_vec3(vert.tan, tan);
    write_vec2(vert.uvs, uvs);
}

template <typename T, typename B>
static void read_vertex(T &vert, const B &target)
{
    auto pos = target.getPos();
    auto norm = target.getNorm();
    auto tan = target.getTan();
    auto uvs = target.getUv();

    read_vec3(vert.pos, pos);
    read_vec3(vert.norm, norm);
    read_vec3(vert.tan, tan);
    read_vec2(vert.uvs, uvs);
}
} // namespace fw