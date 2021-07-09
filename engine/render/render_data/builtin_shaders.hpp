#pragma once

static inline const char *vertex_screen = R"(
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTexCoords;

out vec2 TexCoords;
void main()
{
    gl_Position = vec4(aPos, 1.0); 
    TexCoords = aTexCoords;
})";

static inline const char *fragment_screen = R"(
#version 450 core
out vec4 FragColor;
in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;
void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
})";

static inline const char *vertex_std = R"(
#version 450 core
layout(location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTexCoords;

out vec2 TexCoords;

layout(std140, binding = 0) uniform Matrices{
    mat4 projection;
    mat4 view;
};

layout(location = 0) uniform mat4 model;

void main() { 
    gl_Position = projection * view * model * vec4(aPos, 1.0); 
    TexCoords = aTexCoords;
}
)";

static inline const char *fragment_texmap = R"(
#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
})";

static inline const char *fragment_err = R"(
#version 450 core
out vec4 FragColor;

void main() { 
    FragColor = vec4(1.0, 0.0, 1.0, 1.0); 
}
)";