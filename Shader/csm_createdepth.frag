#version 330
out vec4 FragColor;  
in vec2 texCoord;
in vec3 normal;
uniform bool checkDiffuse;
uniform bool useNormlaMap;
uniform bool useTextures;
uniform sampler2D tex_bump;
uniform vec3 lightDir;

void main()
{
	if (!checkDiffuse) {
		FragColor = vec4(1.0);
		return;
	}		
	vec3 nn;
	if (useTextures && useNormlaMap)
		nn = normalize(texture(tex_bump, texCoord).rgb * 2.0 - 1.0);
	else
		nn = normalize(normal);

	bool res = (dot(nn, normalize(lightDir)) >= 0.2);
	//FragColor = vec4(1.0, 0.0, 1.0, 0.0);
	//FragColor = vec4(vec3(nn), 1.0);
	//FragColor = vec4(vec3(depth), 1.0);
	FragColor = res ? vec4(1.0) : vec4(0.0);
} 