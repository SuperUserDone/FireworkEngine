#pragma once

#include "core/logger.hpp"
#include <cstdio>
#include <filesystem>
#include <string>

namespace blood
{
namespace vfs
{

static inline std::unordered_map<std::string, std::filesystem::path> file_lookup;

inline void vfs_register(const std::string &path, const std::string &lookup)
{
    if (!std::filesystem::exists(path))
        std::filesystem::create_directories(path);

    file_lookup[lookup] = std::filesystem::path(path);
}

inline std::string vfs_resolve_path(const std::string &path)
{
    size_t a = path.find("://");

    if (a == std::string::npos)
    {
        LOG_EF("Could not resolve path {}", path);
        return "";
    }

    std::string lookup = path.substr(0, a);
    std::string fspath = path.substr(a + 3);

    auto location = file_lookup[lookup] / std::filesystem::path(fspath);

    return location.string();
}

inline FILE *vfs_fopen(const std::string &path, const std::string &modes)
{
    return fopen(vfs_resolve_path(path).c_str(), modes.c_str());
}

} // namespace vfs
} // namespace blood
