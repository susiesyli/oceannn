#include "skyboxTechnique.h"
#include "ShaderManager.h"

SkyboxTechnique::SkyboxTechnique() : m_WVPLocation(0), m_textureLocation(0) {}

bool SkyboxTechnique::Init() {
    // Load and compile the vertex and fragment shaders for the skybox
    if (!InitShader("shaders/330/skybox-vert.shader", "shaders/330/skybox-frag.shader")) {
        std::cerr << "Error initializing skybox shaders!" << std::endl;
        return false;
    }

    // Get the uniform locations
    m_WVPLocation = glGetUniformLocation(m_programID, "gWVP");
    m_textureLocation = glGetUniformLocation(m_programID, "gCubemapTexture");

    return true;
}

void SkyboxTechnique::SetWVP(const glm::mat4& WVP) {
    glUniformMatrix4fv(m_WVPLocation, 1, GL_FALSE, glm::value_ptr(WVP));
}

void SkyboxTechnique::SetTextureUnit(unsigned int TextureUnit) {
    glUniform1i(m_textureLocation, TextureUnit);  // Set the texture unit for the cubemap
}
