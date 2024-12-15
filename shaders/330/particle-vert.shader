#version 330

in vec3 myNormal;
in vec3 myPosition;

uniform mat4 particleModel;
uniform mat4 particleView;
uniform mat4 particleProjection;

out vec3 particleFragPosition;
out vec3 particleFragNormal;

void main()
{
    // Transform vertex position to world space
    particleFragPosition = vec3(particleModel * vec4(myPosition, 1));
    
    // Transform normal to world space (using inverse transpose to handle non-uniform scaling)
    particleFragNormal = mat3(transpose(inverse(particleModel))) * myNormal;
    
    // Compute final vertex position
    gl_Position = particleProjection * particleView * particleModel * vec4(myPosition, 1);
}
