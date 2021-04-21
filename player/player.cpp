#include <cstdio>

#include "core/loop.hpp"

int main(int argc, char const *argv[])
{
    blood::loop loop;

    loop.start();
    loop.block();

    return 0;
}
