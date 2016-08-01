#pragma version 330 core
#pragma interface_start
	vec3 FragPosition;
	vec2 Texcoord;
	vec3 Normal;
	vec4 ShadowCoord;
#pragma interface_end

#pragma vertex
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

	vs_out.ShadowCoord = depthMVP * model * vec4(position, 1.0);
}

#pragma fragment
#pragma include Material
uniform Material material;
#pragma include Light
uniform Light light;

uniform vec3 viewPosition;

uniform sampler2DShadow shadow;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 WorldPosOut;
layout (location = 2) out vec3 NormalOut;

#pragma include LinearizeDepth

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float CalcShadowFactor() {

	vec3 lightDir = normalize(-light.direction);

	float cosTheta = clamp( dot( fs_in.Normal, lightDir ), 0, 1 );
	float bias = 0.005*tan(acos(cosTheta));
	/* bias = 0.005; */
	bias = clamp(bias, 0,0.01);
	float visibility = 1.0;
	/* if ( texture( shadow, fs_in.ShadowCoord.xyz )  <  fs_in.ShadowCoord.z - bias){ */
	/* 	visibility = 0.0; */
	/* } */

	for (int i = 0; i < 4; i++) {
		visibility -= 0.2*(1.0-texture( shadow, vec3(fs_in.ShadowCoord.xy + poissonDisk[i]/1000.0, (fs_in.ShadowCoord.z - bias)/fs_in.ShadowCoord.w ) ));
	}

	return visibility;
}

void main() {

	// Export some values that other shaders can use
	WorldPosOut.rgb = fs_in.FragPosition;
	WorldPosOut.a = LinearizeDepth(gl_FragCoord.z);
	NormalOut = fs_in.Normal;

	if (texture(material.diffuse, fs_in.Texcoord).a == 0.0) {
		discard;
	}

	vec3 color = texture(material.diffuse, fs_in.Texcoord).rgb;

	// Ambient
	vec3 ambient = light.ambient * color;
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(fs_in.Normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * color;

	// Specular (currently not in use)
	const float kPi = 3.14159265;
	float shininess = material.shininess;
	shininess = 32;
	float energyConservation = (8.0 + shininess) / (8.0 * kPi);
	vec3 viewDir = normalize(viewPosition - fs_in.FragPosition);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = energyConservation * pow(max(dot(fs_in.Normal, halfwayDir), 0.0), shininess);
	vec3 specular = light.specular * texture(material.specular, fs_in.Texcoord).rgb * spec;

	FragColor.rgb = ambient + (diffuse * CalcShadowFactor());

	// Gamma correction
	float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
	FragColor.a = LinearizeDepth(gl_FragCoord.z);

	/* FragColor.rgb = vec3(texture( shadow, fs_in.ShadowCoord.xy ).r); */
}
