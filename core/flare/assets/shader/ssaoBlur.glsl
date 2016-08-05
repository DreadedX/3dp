#pragma version 330 core

#pragma vertex
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texcoord;

void main() {

	gl_Position = vec4(position, 1.0);
}

#pragma fragment
uniform sampler2D ssaoTexture;
uniform sampler2D render;

out vec4 FragColor;

#pragma include include/CalcTexCoord

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

    FragColor.rgb = texture(render, CalcTexCoord()).rgb * vec3(result / (4.0 * 4.0)) * vec3(result / (4.0 * 4.0));
	FragColor.a = texture(render, CalcTexCoord()).a;
}  
