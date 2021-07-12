@0x9558e6fb389c4d27;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("fw::capnp");

using Vec = import "vector.capnp";

struct Attribute
{
    bindId @0 :UInt32;

    data :union 
    {
        int8 @1  :Int8;
        int16 @2 :Int16;
        int32 @3 :Int32;
        int64 @4 :Int64;

        uint8 @5  :UInt8;
        uint16 @6 :UInt16;
        uint32 @7 :UInt32;
        uint64 @8 :UInt64;

        float @9   :Float32;
        double @10 :Float64;
        
        texture @11 :Text;
        
        vec2f @12 :Vec.Vec2f;
        vec2i @13 :Vec.Vec2i;
        vec2u @14 :Vec.Vec2u;

        vec3f @15 :Vec.Vec3f;
        vec3i @16 :Vec.Vec3i;
        vec3u @17 :Vec.Vec3u;
        
        vec4f @18 :Vec.Vec4f;
        vec4i @19 :Vec.Vec4i;
        vec4u @20 :Vec.Vec4u;

        bool @21 :Bool;
    }

}

struct Material
{
    attributes @0 :List(Attribute);

    shaderRef @1 :Text;
}