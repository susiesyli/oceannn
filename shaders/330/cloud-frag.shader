#version 330

in vec3 fragPosition;
in vec3 fragNormal;

uniform sampler2D cloudTexture;  // Cloud texture
uniform vec3 u_TransparentColor; // Color to make transparent (e.g., dark gray)
uniform float u_Threshold; // Threshold for similarity


// uniform sampler2D environMap;
uniform float lightIntensity;  // Link to global cloud light intensity
// uniform vec3 cloudColor; 

out vec4 outputColor;

void main()
{	
    vec4 texColor = texture(cloudTexture, v_TexCoords);

    // Calculate the difference between the pixel color and the transparent color
    float diff = distance(texColor.rgb, u_TransparentColor);

    // Check if the difference is below the threshold
    if (diff < u_Threshold) {
        discard; // Skip rendering this fragment
    }

    // Otherwise, render the texture color
    gl_FragColor = texColor;
    //vec3 cloudColor = vec3(1.0f, 1.0f, 1.0f); // without alpha first 
    //vec3 scaledCloudColor = cloudColor * lightIntensity;
    //outputColor = vec4(scaledCloudColor, lightIntensity); // light intensity as alpha
    // Sample the environment map using spherical coordinates
    //vec2 texCoord = textureLocation(fragPosition);
    outputColor = vec4(1.0f, 0.0f, 0.0f, 0.8f);
}