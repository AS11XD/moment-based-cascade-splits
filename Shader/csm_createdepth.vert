#version 330 core
layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec3 NORMAL;
layout (location = 2) in vec2 TEX_COORD;
uniform mat4 MVP; 
uniform mat3 nrmlMat;
uniform bool checkDiffuse;
out vec3 normal;
out vec2 texCoord;

void main()
{
    gl_Position = MVP * vec4(POSITION, 1.0);
	if (checkDiffuse) {
		normal = nrmlMat * NORMAL;
		texCoord = TEX_COORD;
	} else {
		normal = NORMAL;
		texCoord = TEX_COORD;
	}
}    