#version 330

in vec3 myNormal;
in vec3 myPosition;

uniform mat4 rainModel;
uniform mat4 rainView;
uniform mat4 rainProjection;

out vec3 rainFragPosition;
out vec3 rainFragNormal;

void main()
{
    // Transform vertex position to world space
    rainFragPosition = vec3(rainModel * vec4(myPosition, 1));
    
    // Transform normal to world space (using inverse transpose to handle non-uniform scaling)
    rainFragNormal = mat3(transpose(inverse(rainModel))) * myNormal;
    
    // Compute final vertex position
    gl_Position = rainProjection * rainView * rainModel * vec4(myPosition, 1);
}