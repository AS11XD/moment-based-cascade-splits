#version 330 core

out vec4 FragColor;  
uniform sampler2D tex;
uniform int textlength;
uniform int text[200];
uniform float discardAtYBottom;
uniform float discardAtYTop;
in vec2 texc;
in vec4 col;
in vec3 pos;

void main()
{
	if (pos.y > discardAtYTop || pos.y < discardAtYBottom)
		discard;

	int index;
	for (index = 0; index < textlength; index++) {
		if (texc.x > index && texc.x <= (index + 1))
			break;	
	}
	vec2 ntexc = vec2(texc.x - float(index), texc.y);
	int in_char = text[index];
	
	if (in_char == 46 || in_char == 58) {
		ntexc.x *= 0.4;
		ntexc.x += 0.3;
	}

	if (in_char == 105) {
		ntexc.x *= 0.6;
		ntexc.x += 0.2;
	}

	if (in_char == 106 || in_char == 108) {
		ntexc.x *= 0.7;
		ntexc.x += 0.15;
	}

	if (in_char == 73) {
		ntexc.x *= 0.8;
		ntexc.x += 0.1;
	}

	if (in_char == 102 || in_char == 37) {
		ntexc.x *= 0.9;
		ntexc.x += 0.05;
	}

	if (in_char >= 65 && in_char <= 90) {
		ntexc.x = mix((float(in_char - 65) / 26.0), (float(in_char - 64) / 26.0), ntexc.x);
		ntexc.y /= 3.0;
	}
	if (in_char >= 97 && in_char <= 122) {
		ntexc.x = mix((float(in_char - 97) / 26.0), (float(in_char - 96) / 26.0), ntexc.x);		
		ntexc.y /= 3.0; 
		ntexc.y += 1.0 / 3.0; 
	}
	if (in_char >= 48 && in_char <= 57) {
		ntexc.x = mix((float(in_char - 48) / 26.0), (float(in_char - 47) / 26.0), ntexc.x);		
		ntexc.y /= 3.0; 
		ntexc.y += 2.0 / 3.0; 
	}
	if (in_char == 46) { // dot
		ntexc.x = mix((10.0 / 26.0), (11.0 / 26.0), ntexc.x);		
		ntexc.y /= 3.0; 
		ntexc.y += 2.0 / 3.0; 
	}
	if (in_char == 58) { //colon
		ntexc.x = mix((11.0 / 26.0), (12.0 / 26.0), ntexc.x);		
		ntexc.y /= 3.0; 
		ntexc.y += 2.0 / 3.0; 
	}
	if (in_char == 37) { //percent
		ntexc.x = mix((12.0 / 26.0), (13.0 / 26.0), ntexc.x);		
		ntexc.y /= 3.0; 
		ntexc.y += 2.0 / 3.0; 
	}

	if (in_char == 32)
		discard;

	vec4 t = texture(tex, ntexc);
	if (t.a < 0.1)
		discard;

	FragColor = vec4(t) * col;
	//FragColor = vec4(vec3(ntexc.y), 1.0);
}