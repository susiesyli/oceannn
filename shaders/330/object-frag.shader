void main() {
    vec2 texCoord = planarTextureCoords(fragPosition, time);

    vec4 environColor = texture(environMap, texCoord);
    vec4 objectColor = texture(objectTexture, texCoord);

    vec4 finalTexture = mix(environColor, objectColor, textureBlend);

    // Lighting calculation
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);

    // Scale diffuse lighting by lightIntensity
    vec3 diffuseLight = lightIntensity * diff * vec3(1.0, 1.0, 1.0);

   

    // Combine lighting effects
    vec4 diffuseColor = vec4(diffuseLight + beamLight, 1.0);

    outputColor = diffuseColor;
}
