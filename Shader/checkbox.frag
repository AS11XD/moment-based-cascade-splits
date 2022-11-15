#version 330

out vec4 FragColor;
in vec2 texc;
in vec3 pos;
uniform float discardAtYBottom;
uniform float discardAtYTop;
uniform bool activeBox;

void main() 
{
	if (!activeBox || pos.y > discardAtYTop || pos.y < discardAtYBottom)
		discard;

	FragColor = vec4(0.6, 0.6, 0.6, 0.6);
}