#version 400 core
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
uniform sampler2DMS render;

void main() {

	vec2 texCoords = CalcTexCoord();

	if (texelFetch(render, ivec2(texCoords.x * 1280, texCoords.y * 720), gl_SampleID).a == 0.0) {

		color.rgb = texture(skybox, fs_in.EyeDirection).rgb;
	} else {

		color.rgb = texelFetch(render, ivec2(texCoords.x * 1280, texCoords.y * 720), gl_SampleID).rgb;
	}

	color.a = texelFetch(render, ivec2(texCoords.x * 1280, texCoords.y * 720), gl_SampleID).a;
}
