#version 460 core
out vec4 FragColor;  
uniform int MAX_CASCADES;
uniform sampler2D sm_light_vis[16];
in vec2 texC;

void main()
{
	float depth = 1000.0;
	int ind = 0;

	//for (int i = MAX_CASCADES - 1; i >= 0; i--) {
	//	float tempdepth = texture(sm[i], texC).x + offset * i;
		//float tempdepth = texture(sm[i], texC).x;
	//	if (tempdepth > 0.0 && tempdepth < 1.0) {
	//		depth = tempdepth;//  * (offset * (i + 1)) / far;
	//		ind = i;
	//	}
	//}
	//depth = texture(sm[0], texC).r;

	vec2 uv = texC * 2.0;

	if (uv.y < 1.0) {
		if (uv.x < 1.0) {
			depth = texture(sm_light_vis[0], uv).r;
		} else {
			uv -= vec2(1.0, 0.0);
			depth = texture(sm_light_vis[1], uv).r;
		}
	} else {
		if (uv.x < 1.0) {
			uv -= vec2(0.0, 1.0);
			depth = texture(sm_light_vis[2], uv).r;
		} else {
			uv -= vec2(1.0, 1.0);
			depth = texture(sm_light_vis[3], uv).r;
		}
	}

	if (depth >= 1.0)
		discard;

	FragColor = vec4(vec3(depth), 1.0);
} 