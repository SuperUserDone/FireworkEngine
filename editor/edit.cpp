#include "runtime/editor_loop.hpp"
#include "runtime/parse_args.hpp"
#include "vfs/vfs.hpp"

#include <cstdio>
#include <math.h>
#include <memory>

#ifdef main
#undef main
#endif

int main(int argc, const char *argv[])
{
    auto args = parse_args(argc, argv);

    if (args.project_location == "") {
        print_usage();
        return 0;
    }

    fw::vfs::vfs_register(args.project_location, "root");

    editor_loop loop;

    loop.start();
    loop.tickloop();

    return 0;
}
