#include <cstdio>
#include <math.h>
#include <memory>

#include "runtime/engine.hpp"
#include "runtime/parse_args.hpp"

#ifdef main
#undef main
#endif

int main(int argc, const char *argv[])
{
    auto args = parse_args(argc, argv);

    if (args.project_location == "")
    {
        print_usage();
        return 0;
    }

    engine loop;

    auto smr = loop.get_scenemanager().lock();

    auto &&scene = smr->get_active_scene();

    loop.start();
    loop.block();

    return 0;
}
