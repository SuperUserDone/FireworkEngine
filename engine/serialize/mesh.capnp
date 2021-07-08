@0x834b3a560d2b844f;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("fw::capnp");

using Vec = import "vector.capnp";
using Entity = import "entity.capnp";

struct Mesh
{
    verticies @0 :List(Vec.Vertex);
    indicies @1 :List(UInt32);
}