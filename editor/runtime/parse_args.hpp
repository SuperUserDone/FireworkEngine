#pragma once

#include <string>

struct program_args
{
    std::string project_location;
};

inline void print_usage() { puts("Usage:\nmarrow-editor -p/--project <project>\n"); }

inline program_args parse_args(int argc, const char **argv)
{
    program_args args;

    for (size_t i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "--project" || std::string(argv[i]) == "-p")
        {
            args.project_location = argv[i + 1];
            i++;
        }
    }

    return args;
}
