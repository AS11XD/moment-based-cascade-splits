#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 COLOR;
uniform mat4 MVP;
uniform mat4 invL_MVP;

out vec3 color;

void main()
{	
    color = COLOR;
    gl_Position = MVP * invL_MVP * vec4(position, 1.0);
}    