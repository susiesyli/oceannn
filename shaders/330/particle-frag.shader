#version 330

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 lightPos;
uniform sampler2D particleTexture;

out vec4 outputColor;

vec2 uvCylinderProjection(vec3 position) {
    // Correctly use atan2 for a full range of theta (-pi to pi)
    float theta = atan(position.z, position.x);
    float u = (theta + 3.14159265358979323846) / (2.0 * 3.14159265358979323846);

    // Normalize v to the [0, 1] range based on the sphere's expected height range
    float v = position.y * 0.5 + 0.5;

    return vec2(u, v);
}

void main()
{	
    // Use the corrected UV projection
    vec2 uv = uvCylinderProjection(normalize(fragPosition));
    
    // Sample the texture using the UV coordinates
    outputColor = texture(particleTexture, uv);
}
