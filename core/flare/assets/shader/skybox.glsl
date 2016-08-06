#version 330
#interface_start
	vec3 EyeDirection;
#interface_end

#vertex
layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;

void main() {

	mat4 inverseProjection = inverse(projection);
	mat3 inverseModelview = transpose(mat3(view));
	vec3 unprojected = (inverseProjection * vec4(position, 1.0)).xyz;
	vs_out.EyeDirection = inverseModelview * unprojected;

	gl_Position = vec4(position, 1.0);
}

#fragment

#import include/CalcTexCoord.glsl

out vec4 color;

uniform samplerCube skybox;
uniform sampler2D render;

void main() {

	vec2 texCoords = CalcTexCoord();

	if (texture(render, texCoords).a == 0.0) {

		color.rgb = texture(skybox, fs_in.EyeDirection).rgb;
	} else {

		color.rgb = texture(render, texCoords).rgb;
	}
}
