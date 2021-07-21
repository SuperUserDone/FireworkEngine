#pragma once

#include "imgui.h"

#include <functional>
#include <stdint.h>
#include <string>
#include <vector>

struct thumbnail
{
    uint32_t tex_id;
    std::string name;
    std::string glyph = "";
};

int draw_grid(const std::string &name,
              const std::vector<thumbnail> &thumbs,
              std::vector<std::string> &selections,
              float thumb_size,
              const std::string &filter,
              std::function<void(const std::string &)> ondoubleclick);