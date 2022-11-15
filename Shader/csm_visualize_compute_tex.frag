#version 460 core
#define FIXED_SPLITS 0
#define MIN_MAX_SDSM 1
#define MOMENT_BASED_SDSM 2
out vec4 FragColor;  
uniform sampler2D imgTest;
uniform sampler2D imgTest2;
uniform sampler2D imgTestMinMaxOnes;
uniform bool test;
uniform bool picture_test;
uniform bool second;
uniform vec2 aspect;
uniform int alg;
uniform float far;
uniform float near;
uniform float height;
in vec2 texC;

float geometricSeries(float a, float q, int n) {
	if (q == 1.0) 
		return n * a;
	return a * (1.0 - pow(q, n + 1)) / (1.0 - q);
}

float LinearizeDepth(float depth) 
{
	if (depth >= 1.0)
		return 1.0;
    if (depth <= 0.0)
		return 0.0;
	float z = depth * 2.0 - 1.0; // back to NDC 
    return ((2.0 * near * far) / (far + near - z * (far - near)) - near) / (far - near);	
}

void main()
{
	vec4 val = texture(imgTest, texC).rgba;
	vec4 val2 = texture(imgTest2, texC).rgba;

	if (test) {
		if (!picture_test) {
			if (alg == MIN_MAX_SDSM) {
				val.r = abs(val.r -0.1) * 100.0;
				val.g = abs(val.g -0.8) * 100.0;
				val.b = 0.0;
			} else if (alg == MOMENT_BASED_SDSM) {
				val.r = abs(val.r * int(aspect.x * aspect.y) - geometricSeries(2.0, 0.9, int(aspect.x * aspect.y))) * 1000.0;
				val.g = abs(val.g * int(aspect.x * aspect.y) - geometricSeries(4.0, 0.9 * 0.9, int(aspect.x * aspect.y))) * 1000.0;
				val.b = abs(val.b * int(aspect.x * aspect.y) - geometricSeries(8.0, 0.9 * 0.9 * 0.9, int(aspect.x * aspect.y))) * 1000.0;
				val.r += abs(val.a * int(aspect.x * aspect.y) - geometricSeries(16.0, 0.9 * 0.9 * 0.9 * 0.9, int(aspect.x * aspect.y))) * 1000.0;
				val.g += abs(val2.r * int(aspect.x * aspect.y) - geometricSeries(32.0, 0.9 * 0.9 * 0.9 * 0.9 * 0.9, int(aspect.x * aspect.y))) * 1000.0;
				val.b += abs(val2.g * int(aspect.x * aspect.y) - geometricSeries(64.0, 0.9 * 0.9 * 0.9 * 0.9 * 0.9 * 0.9, int(aspect.x * aspect.y))) * 1000.0;
				val.r += abs(val2.b * int(aspect.x * aspect.y) - geometricSeries(128.0, 0.9 * 0.9 * 0.9 * 0.9 * 0.9 * 0.9 * 0.9, int(aspect.x * aspect.y))) * 1000.0;
				val.g += abs(val2.a * int(aspect.x * aspect.y) - geometricSeries(256.0, 0.9 * 0.9 * 0.9 * 0.9 * 0.9 * 0.9 * 0.9 * 0.9, int(aspect.x * aspect.y))) * 1000.0;
				//val.r = abs(val.r * int(aspect.x * aspect.y) - geometricSeries(2.0, 0.9, int(aspect.x * aspect.y))) * 1000.0;
				//val.g = abs(val.g * int(aspect.x * aspect.y) - geometricSeries(1.0, 1.0, int(aspect.x * aspect.y))) * 1000.0 * 0.0;
				//val.b = abs(val.b * int(aspect.x * aspect.y) - geometricSeries(1.0, 1.0, int(aspect.x * aspect.y))) * 1000.0 * 0.0;
				//val.r = abs(val2.r * int(aspect.x * aspect.y) - 0.0625 * 0.5) * 1000.0 + abs(val2.a * int(aspect.x * aspect.y) - 0.0625 * 0.0625) * 1000.0;
				//val.g = abs(val2.g * int(aspect.x * aspect.y) - 0.0625 * 0.25) * 1000.0;
				//val.b = abs(val2.b * int(aspect.x * aspect.y) - 0.0625 * 0.125) * 1000.0;
			} else {
				val = vec4(0.0);
			}
		} else {
			if (alg == MIN_MAX_SDSM) {
				val = vec4(0.0);
			} else if (alg == MOMENT_BASED_SDSM) {
				vec4 m1, m2;
				m1.x = 0.514328479767;
				m1.y = 0.324611485004;
				m1.z = 0.228099301457;
				m1.w = 0.17240396142;
				m2.x = 0.137514427304;
				m2.y = 0.114200524986;
				m2.z = 0.0977492257953;
				m2.w = 0.0855881497264;
				vec3 minMaxOnes = texture(imgTestMinMaxOnes, texC).rgb;

				val /= (aspect.x * aspect.y - minMaxOnes.z);
				val2 /= (aspect.x * aspect.y - minMaxOnes.z);

				//val.r = abs (minMaxOnes.z - 141970) * 1000.0;
				//val.r = abs (minMaxOnes.x - 0.11425781) * 1000.0;
				//val.g = abs (minMaxOnes.y - 0.9921875) * 1000.0;
				val.r = length (val - m1) * 1000.0;
				val.g = length (val2 - m2) * 1000.0;
				val.b = 0.0;

			} else {
				val = vec4(0.0);
			}
		}
	}
	FragColor = vec4(val.rgb, 1.0);
	//FragColor = vec4(vec3(LinearizeDepth(val.r)), 1.0);
} 