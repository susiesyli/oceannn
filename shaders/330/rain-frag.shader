#version 330 core

in vec3 rainFragPosition;  
in vec3 rainFragNormal;  

// uniform sampler2D environMap; 

out vec4 outputColor;

void main() {



    vec3 color = vec3(0.8f, 0.9f, 0.9f); 
    color *= 0.8;

    // for (int i = 0; i < 1000; i++) {
    //     vec3 dropPosition = rainPositions[i];
    //     vec2 texCoord = dropPosition.xy;
        
    //     color += texture(environMap, texCoord).rgb; 
        
    // }


    outputColor = vec4(color, 0.5f);
}
