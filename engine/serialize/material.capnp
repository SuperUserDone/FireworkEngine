@0x9558e6fb389c4d27;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("fw::capnp");

using Vec = import "vector.capnp";

struct Attribute
{
    bindId @0 :UInt32;
    name @1: Text;

    data :union 
    {
        int @2 :Int64;
        uint @3 :UInt64;

        float @4   :Float32;
        double @5 :Float64;
        
        texture @6 :Text;
        
        vec2f @7 :Vec.Vec2f;
        vec2i @8 :Vec.Vec2i;
        vec2u @9 :Vec.Vec2u;

        vec3f @10 :Vec.Vec3f;
        vec3i @11 :Vec.Vec3i;
        vec3u @12 :Vec.Vec3u;
        
        vec4f @13 :Vec.Vec4f;
        vec4i @14 :Vec.Vec4i;
        vec4u @15 :Vec.Vec4u;

        bool @16 :Bool;
    }

}

struct Material
{
    attributes @0 :List(Attribute);

    shaderRef @1 :Text;
}