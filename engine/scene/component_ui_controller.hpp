#pragma once

#include <functional>
namespace blood
{
struct component_ui_controller
{
    std::function<void()> draw;
};
} // namespace blood
