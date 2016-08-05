#version 330 core
#interface_start
	vec2 Texcoord;
#interface_end
#vertex
layout (location = 0) in vec3 position;
layout (location = 3) in vec2 texcoord;

uniform mat4 model;
uniform mat4 depthMVP;

void main() {

	gl_Position = depthMVP * model * vec4(position, 1.0);
	vs_out.Texcoord = texcoord;
}

#fragment
#import include/Material
uniform Material material;
out float fragmentdepth;

void main() {

	if (texture(material.diffuse, fs_in.Texcoord).a == 0.0) {
		discard;
	}

	fragmentdepth = gl_FragCoord.z;
}
