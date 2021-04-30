#pragma once

#include <string>

#include "keycodes.hpp"

namespace blood
{
namespace input
{

std::string code_to_string(key keycode);
std::string modifier_to_string(mod_keys keys);

bool is_key_down(key key_code);

bool check_shotcut(mod_keys modifier, key key_code);

} // namespace input
} // namespace blood
