#version 330

in vec3 fragPosition; // The fragment's position in world space
in vec3 fragNormal;   // The fragment's normal vector in world space

uniform sampler2D moonMap; // The moon's texture map

out vec4 outputColor;

vec2 textureLocation(vec3 point) {
    vec3 normalizedPoint = normalize(point);

    float u = 0.5 + atan(normalizedPoint.z, normalizedPoint.x) / (2.0 * 3.14159265359);
    float v = 0.5 - asin(normalizedPoint.y) / 3.14159265359;

    return vec2(u, v);
}

void main()
{
    vec2 texCoords = textureLocation(fragPosition);

    vec4 textureColor = texture(moonMap, texCoords);

    outputColor = textureColor;
}
