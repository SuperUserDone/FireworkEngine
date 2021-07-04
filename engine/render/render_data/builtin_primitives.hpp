#pragma once
#include "scene/components.hpp"

static const inline std::vector<fw::vertex> quad_vertex_data = {{{
                                                                        1.f,
                                                                        1.f,
                                                                        0.0f,
                                                                    },
                                                                    glm::vec3(0.f),
                                                                    glm::vec3(0.f),
                                                                    {1, 1}},
                                                                   {
                                                                       {
                                                                           1.f,
                                                                           -1.0f,
                                                                           0.0f,
                                                                       },
                                                                       glm::vec3(0.f),
                                                                       glm::vec3(0.f),
                                                                       {1, 0},
                                                                   },
                                                                   {
                                                                       {
                                                                           -1.f,
                                                                           -1.f,
                                                                           0.0f, // bottom left
                                                                       },
                                                                       glm::vec3(0.f),
                                                                       glm::vec3(0.f),
                                                                       {0, 0},
                                                                   },
                                                                   {
                                                                       {
                                                                           -1.f,
                                                                           1.f,
                                                                           0.0f // top left
                                                                       },
                                                                       glm::vec3(0.f),
                                                                       glm::vec3(0.f),
                                                                       {0, 1},
                                                                   }};
static const inline std::vector<uint32_t> quad_index_data = {0, 1, 3, 1, 2, 3};