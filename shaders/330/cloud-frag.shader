#version 330

in vec3 fragPosition;
in vec3 fragNormal;

// uniform sampler2D environMap;
uniform float lightIntensity;  // Link to global cloud light intensity
// uniform vec3 cloudColor; 

out vec4 outputColor;

void main()
{	
    //vec3 cloudColor = vec3(1.0f, 1.0f, 1.0f); // without alpha first 
    //vec3 scaledCloudColor = cloudColor * lightIntensity;
    //outputColor = vec4(scaledCloudColor, lightIntensity); // light intensity as alpha
    // Sample the environment map using spherical coordinates
    //vec2 texCoord = textureLocation(fragPosition);
    outputColor = vec4(1.0f, 1.0f, 1.0f, 0.5f);
}