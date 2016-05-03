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
uniform sampler2D ssaoTexture;

out float FragColor;

vec2 calcTexCoord() {

	return gl_FragCoord.xy / vec2(1280, 720);
}

void main() {

	vec2 TexCoords = calcTexCoord();

    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoTexture, TexCoords + offset).r;
        }
    }
    FragColor = result / (4.0 * 4.0);
}  
