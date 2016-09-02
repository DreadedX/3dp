#version 330 core
#interface_start
	vec3 FragPosition;
	vec2 Texcoord;
	vec3 Normal;
#interface_end

#vertex
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 depthMVP;

void main() {

	vec4 viewPos = view * model * vec4(position, 1.0);
	gl_Position = projection * viewPos;
	vs_out.FragPosition = viewPos.xyz;
	vs_out.Texcoord = texcoord;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vs_out.Normal = normalize(normalMatrix * normal);
}

#fragment
#import include/Material.glsl
uniform Material material;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec3 NormalOut;

#import include/LinearizeDepth.glsl

void main() {

	// Export some values that other shaders can use
	WorldPosOut.rgb = fs_in.FragPosition;
	WorldPosOut.a = LinearizeDepth(gl_FragCoord.z);
	NormalOut = fs_in.Normal;
}
