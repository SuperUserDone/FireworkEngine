#pragma once

#include "keycodes.hpp"

#include <string>

namespace fw { namespace input {

std::string code_to_string(key keycode);
std::string modifier_to_string(mod_keys keys);

void showcursor(bool show = true);

bool is_key_down(key key_code);

bool check_shotcut(mod_keys modifier, key key_code);

}} // namespace fw::input
