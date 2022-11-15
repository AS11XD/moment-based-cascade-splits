#version 430 core
layout(std430, binding = 3) buffer matVP
{
	mat4 vp[];
};
layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec3 NORMAL;
layout (location = 2) in vec2 TEX_COORD;
uniform mat4 matM; 
uniform int idx;

void main()
{
    gl_Position = vp[idx] * matM * vec4(POSITION, 1.0);
}    