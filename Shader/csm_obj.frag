#version 430 core

layout(std430, binding = 4) buffer splitVector
{
	vec2 splits[];
};
out vec4 FragColor;
in vec2 texCoord;
in vec3 normal;
in vec3 wspos;
in vec4 pos_shadowmap_space[16];
uniform int MAX_CASCADES;
uniform sampler2DShadow sm_light[16];
uniform vec2 one_over_aspect[16];
uniform vec2 aspect[16];
uniform vec3 sunCol;
uniform vec3 lightDir;
uniform	float overlap;

uniform int pcf_kernel;
uniform sampler2D tex_k_a;
uniform sampler2D tex_k_d;
uniform sampler2D tex_k_s;
uniform sampler2D tex_bump;
uniform bool useNormlaMap;
uniform bool useTextures;
uniform bool bbfit;
uniform vec3 k_a;
uniform vec3 k_d;
uniform vec3 k_s;

uniform bool showCasc;
uniform float near;
uniform float far;
float shadow_bias_base = 0.04;
float shadow_bias = shadow_bias_base;
float shadow_eps = 0.016;

float eps = 0.00000001;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	vec3 nn;
	if (useTextures && useNormlaMap)
		nn = normalize(texture(tex_bump, texCoord).rgb * 2.0 - 1.0);
	else
		nn = normalize(normal);

	vec4 tex;
	if (useTextures)
		tex = texture(tex_k_d, texCoord);
	else 
		tex = vec4(1.0);

	if (tex.a < 0.1)
		discard;
		
	float shadow = 0.0;
	float factor = 0.0;
	float shadow_bias = shadow_bias_base;

	float shadow2 = 0.0;
	float factor2 = 0.0;
	float shadow_bias2 = shadow_bias_base;

	float shadow3 = 0.0;
	float factor3 = 0.0;
	float shadow_bias3 = shadow_bias_base;
					
	vec3 uv;
	
	vec3 ccc = vec3(1.0);

	float dep = (LinearizeDepth(gl_FragCoord.z) - near) / (far - near);

	for (int i = 0; i < MAX_CASCADES; i++) {
		if (bbfit) {
			if (dep >= splits[i].x && dep < splits[i].y) {
				uv = (pos_shadowmap_space[i].xyz / pos_shadowmap_space[i].w + 1.0) / 2.0;
				shadow_bias *= pow(0.5, i+1);
				shadow_bias = max(shadow_bias, shadow_eps);
				
				for (int y = 0; y < pcf_kernel; y++) {
					for (int x = 0; x < pcf_kernel; x++) {
						vec2 ofs = vec2(one_over_aspect[i].x * float(x - pcf_kernel / 2 + 1 - 0.5), one_over_aspect[i].y * float(y - pcf_kernel / 2 + 1 - 0.5));
						factor +=  texture(sm_light[i], uv + vec3(ofs, -shadow_bias));
					}
				}

				shadow = factor / (pcf_kernel * pcf_kernel);

				if (showCasc) {
					ccc = vec3(sin((i - 1) * 1.14 * 3.0), sin((i + 1) * 3.14 * 1.3), cos(i * 3.14 * 4.0));
				}
			}
		}
		else {
			float overlap_lower = (i == 0) ? 0.0 : max(overlap * (splits[i - 1].y - splits[i - 1].x) - (splits[i].x - splits[i - 1].y), 0.0);
			float overlap_upper = (i == MAX_CASCADES - 1) ? 0.0 : max(overlap * (splits[i + 1].y - splits[i + 1].x) - (splits[i + 1].x - splits[i].y), 0.0);
			bool lower = dep >= splits[i].x + overlap_lower;
			bool upper = dep < splits[i].y - overlap_upper;
			
			if (lower && upper) {
				uv = (pos_shadowmap_space[i].xyz / pos_shadowmap_space[i].w + 1.0) / 2.0;
				shadow_bias *= pow(0.5, i+1);
				shadow_bias = max(shadow_bias, shadow_eps);
				
				//FragColor = vec4(-uv.zzz * 10000.0, 1.0);
				//return;

				for (int y = 0; y < pcf_kernel; y++) {
					for (int x = 0; x < pcf_kernel; x++) {
						vec2 ofs = vec2(one_over_aspect[i].x * float(x - pcf_kernel / 2 + 1 - 0.5), one_over_aspect[i].y * float(y - pcf_kernel / 2 + 1 - 0.5));
						factor +=  texture(sm_light[i], uv + vec3(ofs, -shadow_bias));
					}
				}

				shadow = factor / (pcf_kernel * pcf_kernel);

				if (showCasc) {
					ccc = vec3(sin((i - 1) * 1.14 * 3.0), sin((i + 1) * 3.14 * 1.3), cos(i * 3.14 * 4.0));
				}
			}
			if (overlap > 0.0) {
				if (i < MAX_CASCADES - 1) {
					if (dep >= splits[i].y - overlap_upper && dep < splits[i + 1].x + overlap * (splits[i].y - splits[i].x)) {
						uv = (pos_shadowmap_space[i].xyz / pos_shadowmap_space[i].w + 1.0) / 2.0;
						shadow_bias2 *= pow(0.5, i + 1);
						shadow_bias2 = max(shadow_bias2, shadow_eps);
						
						for (int y = 0; y < pcf_kernel; y++) {
							for (int x = 0; x < pcf_kernel; x++) {
								vec2 ofs = vec2(one_over_aspect[i].x * float(x - pcf_kernel / 2 + 1 - 0.5), one_over_aspect[i].y * float(y - pcf_kernel / 2 + 1 - 0.5));
						
								factor2 +=  texture(sm_light[i], uv + vec3(ofs, -shadow_bias2));
							}
						}
						
						shadow2 = factor2 / (pcf_kernel * pcf_kernel);

						uv = (pos_shadowmap_space[i + 1].xyz / pos_shadowmap_space[i + 1].w + 1.0) / 2.0;
						shadow_bias3 *= pow(0.5, i + 2);
						shadow_bias3 = max(shadow_bias3, shadow_eps);
							
						for (int y = 0; y < pcf_kernel; y++) {
							for (int x = 0; x < pcf_kernel; x++) {
								vec2 ofs = vec2(one_over_aspect[i + 1].x * float(x - pcf_kernel / 2 + 1 - 0.5), one_over_aspect[i + 1].y * float(y - pcf_kernel / 2 + 1 - 0.5));
								factor3 +=  texture(sm_light[i + 1], uv + vec3(ofs, -shadow_bias3));
							}
						}
						
						shadow3 = factor3 / (pcf_kernel * pcf_kernel);
						
						shadow = mix(shadow2, shadow3, (dep - splits[i].y + overlap_upper) / (overlap_upper + overlap * (splits[i].y - splits[i].x)));
						vec3 ccc2 = vec3(sin((i - 1) * 1.14 * 3.0), sin((i + 1) * 3.14 * 1.3), cos(i * 3.14 * 4.0));
						vec3 ccc3 = vec3(sin((i) * 1.14 * 3.0), sin((i + 2) * 3.14 * 1.3), cos((i + 1) * 3.14 * 4.0));
						if (showCasc) {
							ccc = mix(ccc2, ccc3, (dep - splits[i].y + overlap_upper) / (overlap_upper + overlap * (splits[i].y - splits[i].x)));
						}	
					}
				}
			}
		}
	}

	//int ind = int(uv.z / sunOffset);
	//float d = texture(sm_sun[ind], uv.xy).r;

	
	//FragColor = vec4(texture(sm_sun, texCo).rgb, 1.0);
	vec3 terrC = k_d * tex.rgb;

	float shade = min(0.8 * shadow + 0.21,  max(dot(nn, normalize(lightDir)), 0.2));
	FragColor = vec4(terrC * sunCol * ccc * shade, 1.0);
	//FragColor = vec4(ccc, 1.0);
	//FragColor = vec4(terrC * sunCol, 1.0);
	
	//FragColor = vec4(vec3((nn + 1.0) / 2.0), 1.0);
	//FragColor = vec4(vec3(lightDir), 1.0);
	//FragColor = vec4(vec3(depth), 1.0);
	//FragColor = vec4(vec3((gl_FragCoord.z - 0.5) * 2.0), 1.0);
} 