#version 330

in vec3 cloudFragPosition;
in vec3 cloudFragNormal;

uniform sampler2D cloudTexture;  // Cloud texture
uniform float u_Threshold;      // Threshold for white similarity (e.g., 0.8 for 80% white)

// Output color of the fragment
out vec4 outputColor;

vec2 textureLocation(vec3 point) {
    vec3 norm = normalize(point);
    // Convert to spherical coordinates
    float phi = -atan(norm.z, -norm.x);
    float theta = acos(norm.y);
    // Map to texture coordinates
    vec2 coord = vec2(0.5 + phi / (2.0 * 3.14159), theta / 3.14159);
    return coord;
}

void main() {
    // Map fragment position to texture coordinates
    vec2 texCoord = textureLocation(cloudFragPosition);
    // Sample the texture color
    vec3 cloudColor = texture(cloudTexture, texCoord).rgb;

    // Compute the "whiteness" of the sampled color
    float whiteness = dot(cloudColor, vec3(0.333)); // Approximates luminance
    float alpha = whiteness > 0.1f ? 1.0 : 0.0;

    // Set the output color with adjusted alpha
    outputColor = vec4(cloudColor, alpha);
}
