#version 330

out vec4 FragColor;
in vec2 texc;
in vec3 pos;
uniform float discardAtYBottom;
uniform float discardAtYTop;
uniform bool hover;
uniform bool myHold;

void main() 
{
	if (pos.y > discardAtYTop || pos.y < discardAtYBottom)
		discard;

	if (myHold) {
		FragColor = vec4(0.6, 0.9, 0.9, 0.9);
	}
	else if (hover) {
		FragColor = vec4(0.6, 0.75, 0.75, 0.75);
	}
	else {
		FragColor = vec4(0.6, 0.6, 0.6, 0.6);
	}
}