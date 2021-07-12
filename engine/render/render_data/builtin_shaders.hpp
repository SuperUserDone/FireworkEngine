#pragma once

static inline const char *vertex_screen = R"glsl(
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTexCoords;

out vec2 TexCoords;
void main()
{
    gl_Position = vec4(aPos, 1.0); 
    TexCoords = aTexCoords;
})glsl";

static inline const char *fragment_screen = R"glsl(
#version 450 core
out vec4 FragColor;
in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;
void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
})glsl";

static inline const char *vertex_std = R"glsl(
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
)glsl";

static inline const char *fragment_texmap = R"glsl(
#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

// Export
layout(binding = 0) uniform sampler2D Texture;

// Export 
layout(location = 1) uniform vec4 color = vec4(1.0,1.0,1.0,1.0); 

void main()
{ 
    FragColor = texture(Texture, TexCoords) * color;
})glsl";

static inline const char *fragment_err = R"glsl(
#version 450 core
out vec4 FragColor;

void main() { 
    FragColor = vec4(1.0, 0.0, 1.0, 1.0); 
}
)glsl";