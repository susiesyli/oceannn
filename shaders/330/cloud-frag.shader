#version 330

in vec3 cloudfragPosition;
in vec3 cloudFragNormal;

uniform sampler2D cloudTexture;  // Cloud texture
uniform vec3 u_TransparentColor; // Color to make transparent (e.g., dark gray)
uniform float u_Threshold; // Threshold for similarity


// uniform sampler2D environMap;
uniform float lightIntensity;  // Link to global cloud light intensity
// uniform vec3 cloudColor; 

out vec4 outputColor;

vec2 textureLocation(vec3 point) {
    vec3 norm = normalize(point);
    // convert to spherical coordinates
    float phi = -atan(norm.z, -norm.x);
    float theta = acos(norm.y);
    // map to texture coordinates
    vec2 coord = vec2(0.5 + phi / (2.0 * 3.14159), theta / 3.14159);
    return coord;
}

void main()
{	
    vec2 texCoord = textureLocation(cloudFragPosition);
    //vec3 cloudColor = vec3(1.0f, 1.0f, 1.0f); // without alpha first 
    //vec3 scaledCloudColor = cloudColor * lightIntensity;
    //outputColor = vec4(scaledCloudColor, lightIntensity); // light intensity as alpha
    // Sample the environment map using spherical coordinates
    //vec2 texCoord = textureLocation(fragPosition);
    outputColor = vec4(1.0f, 0.0f, 0.0f, 0.05f);
}