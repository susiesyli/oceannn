#version 330

in vec3 fragPosition;
in vec3 fragNormal;

// uniform sampler2D environMap;
uniform float lightIntensity;  // Link to global star light intensity
// uniform vec3 starColor; 

out vec4 outputColor;

void main()
{	
    vec3 starColor = vec3(1.0f, 0.9f, 0.9f); // without alpha first 
    vec3 scaledstarColor = starColor;
    // float skyBrightness = clamp(lightIntensity * (0.1 + 0.5 * sunDir.y), 0.0, 1.0); // darker as sun falls 

    outputColor = vec4(scaledstarColor, (1-lightIntensity));
    // Sample the environment map using spherical coordinates
    // vec2 texCoord = textureLocation(fragPosition);
    // outputColor = vec4(1.0f, 1.0f, 1.0f, 0.5f);
}