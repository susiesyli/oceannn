#version 330

layout (location = 0) in vec3 Position;  // Object space vertex position

uniform mat4 skyModel;
uniform mat4 skyProjection;   
uniform mat4 skyView; // View matrix (camera view)

out vec3 TexCoord0;         // Texture coordinates for fragment shader

void main()
{
    TexCoord0 = vec3(skyModel * vec4(Position, 1));
    vec4 pos = skyProjection * skyView * skyModel * vec4(Position, 1);
    gl_Position = pos.xyww;
    // mat4 viewWithoutTranslation = mat4(mat3(skyView)); // remove the translation part of the view matrix

    // Combine with the projection matrix
    // mat4 mvp = skyProjection * viewWithoutTranslation;

    // Transform the position and set the texture coordinates
    // gl_Position = mvp * vec4(Position, 1);
    // TexCoord0 = Position;  // Skybox uses the cube's vertex positions as texture coordinates
}
