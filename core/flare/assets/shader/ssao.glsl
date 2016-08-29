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

void main() {

	gl_Position = vec4(position, 1.0);
	vs_out.Texcoord = texcoord;
}

#fragment
uniform sampler2DMS gPositionMap;
uniform sampler2D noiseTexture;
uniform sampler2DMS gNormalMap;

uniform vec3 samples[128];
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out float FragColor;

const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0);

int kernelSize = 128;
float radius = 2.0;

void main() {

	vec3 fragPos = texelFetch(gPositionMap, ivec2(fs_in.Texcoord.x * 1280, fs_in.Texcoord.y * 720), 0).xyz;

	vec3 normal = texelFetch(gNormalMap, ivec2(fs_in.Texcoord.x * 1280, fs_in.Texcoord.y * 720), 0).xyz;
	mat3 normalMatrix = transpose(inverse(mat3(view)));
	normal = normalMatrix * normal;
	vec3 randomVec = texture(noiseTexture, fs_in.Texcoord * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));

	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; i++) {

		vec3 sample = TBN * samples[i];
		sample = fragPos + sample * radius;

		vec4 offset = vec4(sample, 1.0);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = -texelFetch(gPositionMap, ivec2(offset.x * 1280, offset.y * 720), 0).a;

		// Enable this when we have skyboxes
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck;
		//occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0);     

	}
	occlusion = 1.0 - (occlusion / kernelSize);

	FragColor = occlusion;
}
