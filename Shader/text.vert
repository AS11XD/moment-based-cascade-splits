#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec4 color;
uniform mat4 MVP;
uniform float offsetX;
out vec2 texc;
out vec4 col;
out vec3 pos;

void main()
{
    gl_Position = MVP * vec4(position - vec3(offsetX, 0.0, 0.0), 1.0);
	texc = texCoord;
	col = color;
	pos = position - vec3(offsetX, 0.0, 0.0);
}    