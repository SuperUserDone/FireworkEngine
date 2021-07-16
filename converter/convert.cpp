#include "argh.h"
#include "process_image.hpp"
#include "process_mesh.hpp"
#include "serialize/image.capnp.c++"
#include "serialize/mesh.capnp.c++"
#include "serialize/vector.capnp.c++"

int main(int argc, char *argv[])
{
    argh::parser cmdl(argc, argv);

    if (cmdl.size() < 3) {
        std::cerr << "Too litte cmd args\n";

        return -1;
    }

    if (cmdl[1] == "image") {
        process_image(cmdl[2].c_str(), cmdl[3].c_str());
    } else if (cmdl[1] == "scene") {
        process_scene(cmdl[2].c_str(), cmdl[3].c_str());
    } else {
        return -1;
    }

    return 0;
}
