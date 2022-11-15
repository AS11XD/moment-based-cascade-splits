#version 430 core
layout(std430, binding = 3) buffer matVP
{
	mat4 vp[];
};
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 COLOR;
uniform mat4 MVP;
uniform int idx;

out vec3 color;

void main()
{	
    color = COLOR;
    gl_Position = MVP * inverse(vp[idx]) * vec4(position, 1.0);
}    