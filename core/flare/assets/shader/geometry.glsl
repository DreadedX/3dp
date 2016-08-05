#pragma version 330 core
#pragma interface_start
	vec3 FragPosition;
	vec2 Texcoord;
	vec3 Normal;
#pragma interface_end

#pragma vertex
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

	vec4 viewPos = view * model * vec4(position, 1.0);
	gl_Position = projection * viewPos;
	vs_out.FragPosition = viewPos.xyz;
	vs_out.Texcoord = texcoord;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vs_out.Normal = normalMatrix * normal;
}

#pragma fragment
layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec3 ColorOut;
layout (location = 2) out vec3 NormalOut;
layout (location = 3) out vec3 TexCoordOut;

layout (location = 4) out vec3 DiffuseColorOut;
layout (location = 5) out vec4 SpecularColorOut;

#pragma include include/Material
uniform Material material;
#pragma include include/Light
uniform Light light;

#pragma include include/LinearizeDepth

void main() {

	if (texture(material.diffuse, fs_in.Texcoord).a == 0.0) {
		discard;
	}

	WorldPosOut.xyz = fs_in.FragPosition;
	WorldPosOut.a = LinearizeDepth(gl_FragCoord.z);
	ColorOut = texture(material.diffuse, fs_in.Texcoord).rgb;
	NormalOut = normalize(fs_in.Normal);
	TexCoordOut = vec3(fs_in.Texcoord, 0.0);

	DiffuseColorOut = light.diffuse;
	SpecularColorOut.rgb = light.specular * texture(material.specular, fs_in.Texcoord).rgb;
	SpecularColorOut.a = material.shininess;
}
