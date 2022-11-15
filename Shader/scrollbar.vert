#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
uniform mat4 MVP;
uniform float offsetY;
uniform float percPos;
out vec2 texc;
out vec3 pos;

void main()
{
    gl_Position = MVP * vec4(position + vec3(0.0, offsetY + percPos, 0.0), 1.0);
	texc = texCoord;
	pos = position + vec3(0.0, offsetY + percPos, 0.0);
}    