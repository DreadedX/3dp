#pragma version 330 core

#pragma interface_start
	vec2 blurTextureCoords[25];
#pragma interface_end

#pragma vertex
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texcoord;

void main() {

	gl_Position = vec4(position, 1.0);

	vec2 centerTexCoords = position.xy * 0.5 + 0.5;
	float pixelSize = 1.0 / 720;

	for (int i = -12; i <= 12; i++) {
		vs_out.blurTextureCoords[i+12] = centerTexCoords + vec2(0, pixelSize * i);
	}
}

#pragma fragment
uniform sampler2D renderedTexture;

out vec3 FragColor;

uniform float test;

void main() {

	FragColor = vec3(0.0);

	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[0]).rgb * 0.004571;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[1]).rgb * 0.00723;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[2]).rgb * 0.010989;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[3]).rgb * 0.016048;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[4]).rgb * 0.022521;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[5]).rgb * 0.03037;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[6]).rgb * 0.039354;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[7]).rgb * 0.049003;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[8]).rgb * 0.058632;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[9]).rgb * 0.067411;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[10]).rgb * 0.074476;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[11]).rgb * 0.079066;

	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[12]).rgb * 0.080657;

	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[13]).rgb * 0.079066;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[14]).rgb * 0.074476;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[15]).rgb * 0.067411;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[16]).rgb * 0.058632;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[17]).rgb * 0.049003;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[18]).rgb * 0.039354;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[19]).rgb * 0.03037;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[20]).rgb * 0.022521;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[21]).rgb * 0.016048;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[22]).rgb * 0.010989;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[23]).rgb * 0.00723;
	FragColor += texture(renderedTexture, fs_in.blurTextureCoords[24]).rgb * 0.004571;

	// TODO: Attach this to a uniform to allow transition to blur
	FragColor = mix(texture(renderedTexture, fs_in.blurTextureCoords[12]).rgb, FragColor, test);

	FragColor *= (1.0 - (test/3));
}  
