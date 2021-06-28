@0x9e882892f14803b6;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("blood::capnp");

using Vec = import "vector.capnp";

struct ComponentCamera
{
    clipFar @0 :Float32;
    clipNear @1 :Float32;
    fov @2 :Float32;
}

struct ComponentMesh
{
    verticies @0 :List(Vec.Vertex);
    indicies @1 :List(UInt32);
}

struct ComponentTransform
{
    pos @0 :Vec.Vec3f;
    rot @1 :Vec.Vec4f;
    scale @2 :Vec.Vec3f;
}

struct Entity
{
    name @0 :Text;
    id @1 :UInt32;

    camera @2 :ComponentCamera;
    mesh @3 :ComponentMesh;
    transform @4 :ComponentTransform;
}