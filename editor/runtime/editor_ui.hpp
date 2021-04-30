#pragma once

#include "render/framebuffer.hpp"
#include "scene/scene.hpp"
#include "scene/scene_manager.hpp"
#include "serialize/scene_serializer.hpp"

bool draw_editor_ui(blood::scene_manager *man, blood::framebuffer &fb, glm::uvec2 &size);