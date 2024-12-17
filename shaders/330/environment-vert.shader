#version 330

in vec3 myNormal;
in vec3 myPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragDirection;
// out vec3 fragPosition;

void main()
{
    // simply pass position as direction vector
    fragDirection = myPosition;

    // apply projection (no view translation to keep the cube static)
    gl_Position = projection * vec4(myPosition, 1);

    // // Transform vertex position to world space
    // fragPosition = vec3(model * vec4(myPosition, 1));
    
    // // Compute final vertex position
    // gl_Position = projection * view * model * vec4(myPosition, 1);
}