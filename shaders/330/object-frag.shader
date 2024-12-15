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
//uniform sampler2D skyTexture;

out vec4 outputColor;

vec3 calculateEnvironmentColor(vec3 lightDirection) {
    vec3 normLight = normalize(lightDirection);

    float elevation = normLight.y;

    vec3 sunriseColor = vec3(1.0, 0.5, 0.2);
    vec3 dayColor = vec3(0.5, 0.8, 1.0);
    vec3 sunsetColor = vec3(1.0, 0.4, 0.2);
    vec3 nightColor = vec3(0.05, 0.05, 0.2);

    vec3 environmentColor;
    if (elevation > 0.5) {
        // Bright day
        environmentColor = mix(dayColor, sunriseColor, (1.0 - elevation) * 2.0);
    } else if (elevation > 0.0) {
        // Sunrise/sunset
        environmentColor = mix(sunriseColor, nightColor, 1.0 - elevation * 2.0);
    } else {
        // Nighttime
        environmentColor = mix(nightColor, sunsetColor, elevation + 1.0);
    }

    return environmentColor;
}

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

    vec3 sampledNormal = normalize(texture(objectTexture, texCoord).rgb * 2.0 - 1.0);

    vec4 environColor = texture(environMap, texCoord);
    vec4 objectColor = texture(objectTexture, texCoord);

    vec4 finalTexture = mix(environColor, objectColor, textureBlend);

    // Lighting calculation
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);

    // Scale diffuse lighting by lightIntensity
    vec3 diffuseLight = lightIntensity * diff * vec3(1.0, 1.0, 1.0);

    // Beam effect
    vec3 horizontalLightPos = vec3(lightPos.x, 0.0f, -lightPos.z);
    vec3 horizontalViewPos = vec3(viewPos.x, 0.0, viewPos.z);
    vec3 horizontalFragPos = vec3(fragPosition.x, 0.0, -fragPosition.z);

    vec3 beamDir = normalize(horizontalViewPos - horizontalLightPos);
    vec3 fragToBeamStart = horizontalFragPos - horizontalLightPos;
    float beamProj = dot(fragToBeamStart, beamDir);
    vec3 closestPointOnBeam = horizontalLightPos + beamProj * beamDir;
    float distanceToBeam = length(horizontalFragPos - closestPointOnBeam);

    // Beam attenuation based on distance
    float beamRadius = 0.1;
    float beamFalloff = 10.0;
    float beamIntensity = exp(-distanceToBeam * beamFalloff);

    // Smooth the edges of the beam
    beamIntensity *= smoothstep(0.0, beamRadius, beamRadius - distanceToBeam);

    // Increase beam intensity based on the y-position of the light
    float heightFactor = lightPos.y * 0.5;
    beamIntensity *= (1.0 + heightFactor);

    vec3 beamLight = beamIntensity * vec3(1.0, 1.0, 1.0);

    vec3 oceanColor = vec3(0.0, 0.5, 0.8);

    // Combine lighting effects
    vec3 lightingEffect = diffuseLight + beamLight;
    vec4 diffuseColor = vec4(lightingEffect, 1.0);

    vec3 finalColor = (finalTexture.rgb * lightingEffect);
    finalColor.r *= 0.25;
    finalColor.g *= 1.2;
    finalColor.b *= 1.5;

    outputColor = vec4(finalColor, 1.0);
}

