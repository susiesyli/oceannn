#version 330

in vec3 TexCoord0;
uniform samplerCube cubemapTexture; // sampler cub instead of sampler 2d texture 
out vec4 outputColor;

void main()
{
    outputColor = texture(cubemapTexture, TexCoord0);
}