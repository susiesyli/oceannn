#version 330 core

in vec3 rainFragPosition;  
in vec3 rainFragNormal;  

uniform sampler2D environMap; 
uniform vec3 rainPositions[1000]; 

out vec4 outputColor;

void main() {
    vec3 color = vec3(0.0f); 

    for (int i = 0; i < 1000; i++) {
        vec3 dropPosition = rainPositions[i];
        vec2 texCoord = dropPosition.xy;
        
        color += texture(environMap, texCoord).rgb; 
        
    }
    
    outputColor = vec4(color / 1000.0f, 1.0f);
}
