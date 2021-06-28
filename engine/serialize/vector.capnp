@0xcfe440017f3e59e5;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("blood::capnp");

struct Vec2f
{
    x @0 :Float32;
    y @1 :Float32;
}

struct Vec2i
{
    x @0 :Int32;
    y @1 :Int32;
}

struct Vec2u
{
    x @0 :UInt32;
    y @1 :UInt32;
}

struct Vec3f
{
    x @0 :Float32;
    y @1 :Float32;
    z @2 :Float32;
}

struct Vec3i
{
    x @0 :Int32;
    y @1 :Int32;
    z @2 :Int32;
}

struct Vec3u
{
    x @0 :UInt32;
    y @1 :UInt32;
    z @2 :UInt32;
}

struct Vec4f
{
    x @0 :Float32;
    y @1 :Float32;
    z @2 :Float32;
    w @3 :Float32;
}

struct Vec4i
{
    x @0 :Int32;
    y @1 :Int32;
    z @2 :Int32;
    w @3 :Int32;
}

struct Vec4u
{
    x @0 :UInt32;
    y @1 :UInt32;
    z @2 :UInt32;
    w @3 :UInt32;
}

struct Vertex
{
    pos @0 :Vec3f;
    norm @1 :Vec3f;
    tan @2 :Vec3f;
    uv @3 :Vec2f;
}