#pragma once

#include "imgui.h"

#include <functional>
#include <stdint.h>
#include <string>
#include <vector>

int draw_grid(const std::string &name,
              const std::vector<std::pair<uint32_t, std::string>> &thumbs,
              std::vector<std::string> &selections,
              float thumb_size,
              const std::string &filter,
              std::function<void(const std::string &)> ondoubleclick);