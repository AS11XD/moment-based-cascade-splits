#version 330 core

layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec3 COLOR;
layout (location = 2) in vec2 TEX_COORD;
uniform mat4 MVP; 
uniform vec3 pos;

out vec2 texCoord;
out vec3 color;

void main()
{
    gl_Position = MVP * vec4(POSITION + pos, 1.0);
	texCoord = TEX_COORD;
	color = COLOR;
}