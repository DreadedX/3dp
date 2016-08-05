#pragma version 330 core
#pragma interface_start
	vec2 Texcoord;
#pragma interface_end
#pragma vertex
layout (location = 0) in vec3 position;
layout (location = 3) in vec2 texcoord;

uniform mat4 model;
uniform mat4 depthMVP;

void main() {

	gl_Position = depthMVP * model * vec4(position, 1.0);
	vs_out.Texcoord = texcoord;
}

#pragma fragment
#pragma include include/Material
uniform Material material;
out float fragmentdepth;

void main() {

	if (texture(material.diffuse, fs_in.Texcoord).a == 0.0) {
		discard;
	}

	fragmentdepth = gl_FragCoord.z;
}
