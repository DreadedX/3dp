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
uniform sampler2D renderedTexture;

out vec3 FragColor;

#pragma include CalcTexCoord

void main() {

	vec2 TexCoord = CalcTexCoord();

    /* FragColor = vec3(1.0, 1.0, 1.0); */
    FragColor = texture(renderedTexture, TexCoord).rgb;

	int y = int(TexCoord.y * 720);
	if (y % 4 == 0 || y % 4 == 1) {
		FragColor *= 0.5;
	}
}  
