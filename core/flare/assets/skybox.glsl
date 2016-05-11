#pragma version 330
#pragma interface_start
	vec3 TexCoords;
#pragma interface_end

#pragma vertex
layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;

void main() {

	gl_Position = projection * view * vec4(position, 1.0);
	vs_out.TexCoords = position;
}

#pragma fragment

out vec4 color;

uniform samplerCube skybox;

void main() {

	color.rgb = texture(skybox, fs_in.TexCoords).rgb;
	color.a = 1.0;

	/* color = vec4(fs_in.TexCoords.x, fs_in.TexCoords.y, 1.0, 1.0); */
}
