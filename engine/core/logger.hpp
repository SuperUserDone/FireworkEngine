#pragma once

#include "glm/glm.hpp"

#include <assert.h>
#include <fmt/format.h>
#include <fstream>
#include <mutex>

#ifdef __linux__
// Regular text
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

// Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

#define COLRS "\e[0m"

#else

#define BLK   ""
#define RED   ""
#define GRN   ""
#define YEL   ""
#define BLU   ""
#define MAG   ""
#define CYN   ""
#define WHT   ""

// Regular bold text
#define BBLK  ""
#define BRED  ""
#define BGRN  ""
#define BYEL  ""
#define BBLU  ""
#define BMAG  ""
#define BCYN  ""
#define BWHT  ""

// Reset
#define COLRS ""
#endif

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_D(msg) Logger(0, __FILENAME__, __LINE__, msg);
#define LOG_I(msg) Logger(1, __FILENAME__, __LINE__, msg);
#define LOG_W(msg) Logger(2, __FILENAME__, __LINE__, msg);
#define LOG_E(msg) Logger(3, __FILENAME__, __LINE__, msg);
#define LOG_F(msg) Logger(4, __FILENAME__, __LINE__, msg);

#define LOG_DF(msg, ...) Logger(0, __FILENAME__, __LINE__, fmt::format(msg, __VA_ARGS__));
#define LOG_IF(msg, ...) Logger(1, __FILENAME__, __LINE__, fmt::format(msg, __VA_ARGS__));
#define LOG_WF(msg, ...) Logger(2, __FILENAME__, __LINE__, fmt::format(msg, __VA_ARGS__));
#define LOG_EF(msg, ...) Logger(3, __FILENAME__, __LINE__, fmt::format(msg, __VA_ARGS__));
#define LOG_FF(msg, ...) Logger(4, __FILENAME__, __LINE__, fmt::format(msg, __VA_ARGS__));

#define LOG_RUNTIME_ERROR(msg)                                                                     \
    {                                                                                              \
        LOG_F(msg);                                                                                \
        throw std::runtime_error(msg);                                                             \
    }

#define LOG_RUNTIME_ERRORF(msg, ...)                                                               \
    {                                                                                              \
        LOG_FF(msg, __VA_ARGS__);                                                                  \
        throw std::runtime_error(msg);                                                             \
    }

#ifndef NDEBUG
#define FIREWORK_ASSERT(c, s)                                                                      \
    if (!(c)) LOG_F(s);                                                                            \
    assert(c);

#define FIREWORK_ASSERTF(c, s, ...)                                                                \
    if (!(c)) LOG_FF(s, __VA_ARGS__);                                                              \
    assert(c);

#else
#define FIREWORK_ASSERT(c, s) ;

#define FIREWORK_ASSERTF(c, s, ...) ;

#endif

template <>
struct fmt::formatter<glm::vec3>
{
    char presentation = 'f';

    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;
        if (it != end && *it != '}') throw format_error("invalid format");
        return it;
    }

    template <typename FormatContext>
    auto format(const glm::vec3 &p, FormatContext &ctx) -> decltype(ctx.out())
    {
        // ctx.out() is an output iterator to write to.
        return format_to(
            ctx.out(),
            presentation == 'f' ? "({:.4f}, {:.4f}, {:.4f})" : "({:.4e}, {:.4e},  {:.4e})",
            p.x,
            p.y,
            p.z);
    }
};
template <>
struct fmt::formatter<glm::vec4>
{
    char presentation = 'f';

    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;
        if (it != end && *it != '}') throw format_error("invalid format");
        return it;
    }

    template <typename FormatContext>
    auto format(const glm::vec4 &p, FormatContext &ctx) -> decltype(ctx.out())
    {
        // ctx.out() is an output iterator to write to.
        return format_to(ctx.out(),
                         presentation == 'f' ? "({:.4f}, {:.4f}, {:.4f}, {:.4f})"
                                             : "({:.4e}, {:.4e}, {:.4e}, {:.ef})",
                         p.x,
                         p.y,
                         p.z,
                         p.w);
    }
};
class Logger
{
public:
    static void init_logger(int level, const std::string &file_location)
    {
        m_level = level;
        if (file_location != "")
            m_file.open(file_location, std::fstream::out | std::fstream::trunc);

        LOG_IF("Opening logger at level {} and file {}.", level, file_location);
    }

    std::string get_level_col(int level)
    {
        switch (level) {
        case 0:
            return std::string(WHT) + "[DEBUG]";
            break;

        case 1:
            return std::string(GRN) + "[ INFO]";
            break;

        case 2:
            return std::string(YEL) + "[ WARN]";
            break;

        case 3:
            return std::string(RED) + "[ERROR]";
            break;

        case 4:
            return std::string(BRED) + "[FATAL]";
            break;

        default:
            return std::string(COLRS) + "[ NONE]";
            break;
        }
    }

    std::string get_level(int level)
    {
        switch (level) {
        case 0:
            return "[DEBUG]";
            break;

        case 1:
            return "[ INFO]";
            break;

        case 2:
            return "[ WARN]";
            break;

        case 3:
            return "[ERROR]";
            break;

        case 4:
            return "[FATAL]";
            break;

        default:
            return "[ NONE]";
            break;
        }
    }

    Logger(int level, const std::string &file, int line, const std::string &message)
    {
        if (m_file) {
            m_file << fmt::format(
                "{} [{}:{}] {}\n", get_level(level), file, std::to_string(line), message);

            if (level >= 2) m_file.flush();
        }

        if (m_level > level) return;

#ifndef WIN32
        fmt::print(
            "{} [{}:{}]\e[0m {}\n", get_level_col(level), file, std::to_string(line), message);
#else
        fmt::print("{} [{}:{}] {}\n", get_level_col(level), file, std::to_string(line), message);
#endif
    }

    static void close_file()
    {
        LOG_I("Shutting down logger");
        m_file.close();
    }

private:
    static inline std::ofstream m_file;
    static inline int m_level = 0;
};