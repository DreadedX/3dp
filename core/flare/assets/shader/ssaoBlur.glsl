#version 330 core

#vertex
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texcoord;

void main() {

	gl_Position = vec4(position, 1.0);
}

#fragment
uniform sampler2D ssaoTexture;
uniform sampler2DMS render;

out vec4 FragColor;

#import include/CalcTexCoord.glsl

void main() {

	vec2 TexCoords = CalcTexCoord();

    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
	{
		for (int y = -2; y < 2; ++y) 
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(ssaoTexture, TexCoords + offset).r;
		}
	}

	vec3 color;
	for (int i = 0; i < 8; i++) {

		color += texelFetch(render, ivec2(CalcTexCoord().x * 1280, CalcTexCoord().y * 720), i).rgb;
	}

    FragColor.rgb = (color/8) * vec3(result / (4.0 * 4.0)) * vec3(result / (4.0 * 4.0));
	FragColor.a = texelFetch(render, ivec2(CalcTexCoord().x * 1280, CalcTexCoord().y * 720), 0).a;
}  
