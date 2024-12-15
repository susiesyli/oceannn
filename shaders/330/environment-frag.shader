#version 330

in vec3 fragDirection; // texture coordinate 
uniform samplerCube cubeMap; // take in a cube map (skybox)

// in vec3 fragPosition;
// uniform sampler2D environMap;

out vec4 outputColor; // same as the old env map 

void main()
{	
    // Normalize the direction vector and sample the cube map
    vec3 direction = normalize(fragDirection);
    outputColor = texture(cubeMap, direction);
    // outputColor = vec4(1.0, 0.0, 0.0, 1.0); // Red for skybox

    // // Sample the environment map using spherical coordinates
    // vec2 texCoord = textureLocation(fragPosition);
    // outputColor = texture(environMap, texCoord);
}