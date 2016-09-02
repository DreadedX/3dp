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
uniform sampler2DMS renderedTexture;

out vec3 FragColor;

#import include/CalcTexCoord.glsl

void main() {

	int NUM_SAMPLES = 8;

	vec2 fTexCoords = CalcTexCoord();
	ivec2 TexCoords;
	TexCoords.x = int(fTexCoords.x * 1280);
	TexCoords.y = int(fTexCoords.y * 720);

	for(int sample = 0; sample < NUM_SAMPLES; sample++) {

		FragColor += texelFetch(renderedTexture, TexCoords, sample).rgb;
	}

	FragColor /= NUM_SAMPLES;
}  
