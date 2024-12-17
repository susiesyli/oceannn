#version 330

in vec3 fragPosition;
in vec3 fragNormal;

// uniform sampler2D environMap;
uniform float lightIntensity;  // Link to global sun light intensity
// uniform vec3 sunColor; 

out vec4 outputColor;

void main()
{	
    vec3 sunColor = vec3(1.0f, 0.9f, 0.7f); // without alpha first 
    vec3 scaledSunColor = sunColor * lightIntensity;
    outputColor = vec4(scaledSunColor, lightIntensity);
    // Sample the environment map using spherical coordinates
    //vec2 texCoord = textureLocation(fragPosition);
    // outputColor = vec4(1.0f, 1.0f, 1.0f, 0.5f);
}