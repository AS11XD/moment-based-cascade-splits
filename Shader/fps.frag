#version 330 core

out vec4 FragColor;  
uniform sampler2D tex;
uniform float fps;
in vec2 texc;

void main()
{
	float mFps = fps;
	float offset = 1.0 / 11.0;
	vec2 textcoord = texc;
	if (texc.x < 1.0) {
		int num = int(mFps) / 1000;
		if (num == 0)
			discard;
		textcoord.x *= offset;
		textcoord.x += float(num) * offset;
	} else if (texc.x < 2.0) {
		int check = int(mFps) / 1000;
		int num = (int(mFps) % 1000) / 100;
		if (check == 0 && num == 0)
			discard;
		textcoord.x -= 1.0;
		textcoord.x *= offset;
		textcoord.x += float(num) * offset;
	} else if (texc.x < 3.0) {
		int check = int(mFps) / 100;
		int num = (int(mFps) % 100) / 10;
		if (check == 0 && num == 0)
			discard;
		textcoord.x -= 2.0;
		textcoord.x *= offset;
		textcoord.x += float(num) * offset;
	} else if (texc.x < 4.0) {
		int num = (int(mFps) % 10);
		textcoord.x -= 3.0;
		textcoord.x *= offset;
		textcoord.x += float(num) * offset;
	} else if (texc.x < 5.0) {
		// DOT
		textcoord.x -= 4.0;
		textcoord.x *= offset;
		textcoord.x += 10.0 * offset;
	} else if (texc.x < 6.0) {
		int num = int(mFps * 10.0) % 10;
		textcoord.x -= 5.0;
		textcoord.x *= offset;
		textcoord.x += float(num) * offset;
	} else if (texc.x < 7.0) {
		int check = int(mFps * 100.0) % 10;
		check += int(mFps * 1000.0) % 10;
		check += int(mFps * 10000.0) % 10;
		int num = int (mFps * 100.0) % 10;
		if (check == 0 && num == 0)
			discard;
		textcoord.x -= 6.0;
		textcoord.x *= offset;
		textcoord.x += float(num) * offset;
	} else if (texc.x < 8.0) {
		int check = int(mFps * 1000.0) % 10;
		check += int(mFps * 10000.0) % 10;
		int num = int (mFps * 1000.0) % 10;
		if (check == 0 && num == 0)
			discard;
		textcoord.x -= 7.0;
		textcoord.x *= offset;
		textcoord.x += float(num) * offset;
	} else {	
		int num = int (mFps * 10000.0) % 10;
		if (num == 0)
			discard;
		textcoord.x -= 8.0;
		textcoord.x *= offset;
		textcoord.x += float(num) * offset;
	}

	vec4 text = texture(tex, textcoord);
	if (text.a <= 0.0) 
		discard;
	FragColor = vec4(0.0, text.g, 0.0, 1.0);
} 