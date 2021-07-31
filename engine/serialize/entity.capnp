@0x9e882892f14803b6;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("fw::capnp");

using Vec = import "vector.capnp";

struct ComponentCamera
{
    clipFar @0 :Float32;
    clipNear @1 :Float32;
    fov @2 :Float32;
}

struct ComponentMeshRenderer
{
    meshRefrence @0 :Text;
    matRefrence @1 :Text;
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
    meshRenderer @3 :ComponentMeshRenderer;
    transform @4 :ComponentTransform;
}