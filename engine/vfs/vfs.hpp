#pragma once

#include "core/logger.hpp"
#include <cstdio>
#include <filesystem>
#include <mutex>
#include <string>

#include <parallel_hashmap/phmap.h>

namespace blood
{
class vfs
{
public:
    static inline phmap::parallel_flat_hash_map<
        std::string,
        std::filesystem::path,
        phmap::priv::hash_default_hash<std::string>,
        phmap::priv::hash_default_eq<std::string>,
        phmap::priv::Allocator<phmap::priv::Pair<const std::string, std::filesystem::path>>,
        4,
        std::mutex>
        file_lookup;

    static inline void vfs_register(const std::string &path, const std::string &lookup)
    {
        if (!std::filesystem::exists(path))
            std::filesystem::create_directories(path);

        file_lookup.try_emplace_l(
            lookup,
            [path](std::filesystem::path &p) { p = std::filesystem::path(path); },
            std::filesystem::path(path));
    }

    static inline std::string vfs_resolve_path(const std::string &path)
    {
        size_t a = path.find("://");

        if (a == std::string::npos)
        {
            LOG_EF("Could not resolve path {}", path);
            return "";
        }

        std::string lookup = path.substr(0, a);
        std::string fspath = path.substr(a + 3);

        std::filesystem::path p_lookedup;

        if (!file_lookup.if_contains(lookup,
                                     [&p_lookedup](std::filesystem::path &p) { p_lookedup = p; }))

        {
            LOG_EF("Could not resolve path {}", path);
            return "";
        }

        auto location = p_lookedup / std::filesystem::path(fspath);

        return location.string();
    }

    static inline FILE *vfs_fopen(const std::string &path, const std::string &modes)
    {
        return fopen(vfs_resolve_path(path).c_str(), modes.c_str());
    }
};
} // namespace blood
