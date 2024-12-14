#version 330

in vec3 fragPosition;
in vec3 fragNormal;

uniform sampler2D environMap;
uniform sampler2D objectTexture;
uniform vec3 lightPos;
uniform float lightIntensity;
uniform vec3 viewPos;
uniform float textureBlend;
uniform float repeatU;
uniform float repeatV;
uniform float time;
uniform vec2 waveSpeed;
uniform float waveAmplitude;
uniform float waveFrequency;

out vec4 outputColor;

vec2 planarTextureCoords(vec3 point, float time) {
    float u = point.x * repeatU + time * waveSpeed.x; 
    float v = point.z * repeatV + time * waveSpeed.y;

    u += waveAmplitude * sin(2.0 * 3.14159 * waveFrequency * v + time);
    v += waveAmplitude * cos(2.0 * 3.14159 * waveFrequency * u + time);

    u = mod(u, 1.0);
    if (u < 0.0) u += 1.0;
    v = mod(v, 1.0);
    if (v < 0.0) v += 1.0;

    return vec2(u, v);
}

void main() {
    vec2 texCoord = planarTextureCoords(fragPosition, time);

    vec4 environColor = texture(environMap, texCoord);

    vec4 objectColor = texture(objectTexture, texCoord);

    vec4 finalTexture = mix(environColor, objectColor, textureBlend);

    vec4 diffuseColor = vec4(1.0);
    
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);

    diffuseColor = vec4(diff * vec3(1.0), 1.0);

    outputColor = finalTexture * diffuseColor + lightIntensity;
}
