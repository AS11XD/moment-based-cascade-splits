#version 430 core
layout(std430, binding = 3) buffer matVP
{
	mat4 vp[];
};
layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec3 NORMAL;
layout (location = 2) in vec2 TEX_COORD;
uniform mat4 MVP; 
uniform mat4 matM;
uniform mat3 nrmlMat;

uniform int MAX_CASCADES;

out vec2 texCoord;
out vec3 normal;
out vec3 wspos;
out vec4 pos_shadowmap_space[16];

void main()
{	
    gl_Position = MVP * vec4(POSITION, 1.0);
	wspos = vec3(matM * vec4(POSITION, 1.0));
	normal = nrmlMat * NORMAL;
	texCoord = TEX_COORD;
	for (int i = 0; i < MAX_CASCADES; i++) 
		pos_shadowmap_space[i] = vp[i] * vec4(wspos, 1.0);
}