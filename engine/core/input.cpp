#include "input.hpp"

#include <SDL.h>

static const Uint8 *keyboard_state = nullptr;

namespace blood
{
namespace input
{
bool is_key_down(key key_code)
{
    if (!keyboard_state)
        keyboard_state = SDL_GetKeyboardState(nullptr);

    return keyboard_state[SDL_GetScancodeFromKey(key_code)];
}
} // namespace input
} // namespace blood