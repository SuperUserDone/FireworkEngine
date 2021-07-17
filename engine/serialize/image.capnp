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

    compressedDataDict @4: Data;

    enum ColorFormat
    {
        formatR @0;
        formatRg @1;
        formatRgb @2;
        formatRgba @3;
        formatSrgb @4;
        formatSrgba @5;
    } 

    enum Compression
    {
        compNone @0;
        compZstd @1;
    }

}