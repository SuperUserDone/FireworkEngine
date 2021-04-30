#pragma once

#include "render/framebuffer.hpp"
#include "scene/scene.hpp"

bool draw_editor_ui(blood::scene *scene, blood::fb_handle &fb, glm::uvec2 &size);