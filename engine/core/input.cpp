#include "core/logger.hpp"
#include "input.hpp"

#include <SDL.h>
#include <algorithm>

static const Uint8 *keyboard_state = nullptr;

namespace fw { namespace input {

std::string code_to_string(key keycode)
{
    std::string out(SDL_GetKeyName(keycode));

    std::transform(out.begin(), out.end(), out.begin(), ::toupper);

    return out;
}

std::string modifier_to_string(mod_keys keys)
{
    std::string out;

    if (keys & MODKEY_CTRL) out += "Ctrl+";
    if (keys & MODKEY_ALT) out += "Alt+";
    if (keys & MODKEY_SHIFT) out += "Shft+";

    return out;
}

bool is_key_down(key key_code)
{
    if (!keyboard_state) keyboard_state = SDL_GetKeyboardState(nullptr);

    return keyboard_state[SDL_GetScancodeFromKey(key_code)];
}

bool check_shotcut(mod_keys modifier, key key_code)
{
    return (SDL_GetModState() & modifier) && is_key_down(key_code);
}

}} // namespace fw::input