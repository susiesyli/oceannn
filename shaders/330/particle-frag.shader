#version 330

in vec3 fragPosition;
in vec3 fragNormal;

uniform sampler2D environMap;

out vec4 outputColor;

void main()
{	
    // Sample the environment map using spherical coordinates
    //vec2 texCoord = textureLocation(fragPosition);
    outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}