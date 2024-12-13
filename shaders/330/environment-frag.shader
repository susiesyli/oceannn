#version 330

in vec3 fragPosition;

uniform sampler2D environMap;

out vec4 outputColor;

vec2 textureLocationCube(vec3 point) {
    vec3 absPoint = abs(point);
    vec2 uv;

    if (absPoint.x >= absPoint.y && absPoint.x >= absPoint.z) {
        // X face
        uv = point.x > 0.0 ? vec2(-point.z, -point.y) : vec2(point.z, -point.y);
        uv = uv * 0.5 + 0.5;
    } else if (absPoint.y >= absPoint.x && absPoint.y >= absPoint.z) {
        // Y face
        uv = point.y > 0.0 ? vec2(point.x, point.z) : vec2(point.x, -point.z);
        uv = uv * 0.5 + 0.5;
    } else {
        // Z face
        uv = point.z > 0.0 ? vec2(point.x, -point.y) : vec2(-point.x, -point.y);
        uv = uv * 0.5 + 0.5; 
    }

    return uv;
}

void main()
{	
    vec2 texCoord = textureLocationCube(normalize(fragPosition));
    outputColor = texture(environMap, texCoord);
}
