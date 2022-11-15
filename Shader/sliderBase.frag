#version 330

out vec4 FragColor;
in vec2 texc;
in vec3 pos;
uniform float discardAtYBottom;
uniform float discardAtYTop;

void main() 
{
	if (pos.y > discardAtYTop || pos.y < discardAtYBottom)
		discard;

	FragColor = vec4(1.0, 1.0 ,1.0, 0.2);
}