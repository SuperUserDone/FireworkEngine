#pragma once

#include "imgui.h"

#include <stdint.h>
#include <string>
#include <vector>

void draw_grid(const std::vector<std::pair<uint32_t, std::string>> &thumbs,
               std::vector<std::string> &selections,
               float thumb_size);