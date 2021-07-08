@0xef02fd4e83bde293;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("fw::capnp");

using Vec = import "vector.capnp";

struct Image
{
    size @0: Vec.Vec2u;
    compressedData @1: Data;

    colorFormat @2: ColorFormat;
    compression @3: Compression;

    enum ColorFormat
    {
        formatR @0;
        formatRg @1;
        formatRgb @2;
        formatRgba @3;
    } 

    enum Compression
    {
        compNone @0;
        compLz4 @1;
    }

}