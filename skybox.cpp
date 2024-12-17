
#include <iostream>
#include <cmath>
#include "TextureManager.h"
#include "skyboxTechnique.h"
#include <glm/gtc/constants.hpp>
#include <vector> 
#include <string>
#include "skybox.h"   



SkyBox::SkyBox(glm::vec3* eyePos, glm::vec3* lookAt, glm::mat4 projection)
    : m_eyePos(eyePos), m_lookAt(lookAt), m_projection(projection)
{
    // Initialize any other members if needed
    m_pSkyboxTechnique = nullptr;
    m_pCubemapTex = nullptr;
    m_pMesh = nullptr;
}

bool SkyBox::Init(const string& PosXFilename,
                  const string& NegXFilename,
                  const string& PosYFilename,
                  const string& NegYFilename,
                  const string& PosZFilename,
                  const string& NegZFilename)
{
    // Create and initialize the SkyboxTechnique class for shaders
    m_pSkyboxTechnique = new SkyboxTechnique();
    if (!m_pSkyboxTechnique->Init()) {
        return false;  // Failed to initialize shaders
    }

    // call cubemap texture class to load the cubemap texture
    m_pCubemapTex = new CubemapTexture(PosXFilename, NegXFilename,
                                       PosYFilename, NegYFilename,
                                       PosZFilename, NegZFilename);
    if (!m_pCubemapTex->Load()) {
        return false;  // Failed to load cubemap texture
    }

    // Initialize the mesh (typically a cube or sphere for the skybox)
    m_pMesh = new Mesh();
    // Load a cube or sphere mesh into the m_pMesh

    return true;
}

void SkyBox::Render() {
    // Create the view matrix using the eye position and the look-at point
    glm::mat4 viewMatrix = glm::lookAt(*m_eyePos, *m_lookAt, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 WVP = m_projection * viewMatrix;  // Combine view and projection matrices
    m_pSkyboxTechnique->SetWVP(WVP);  // Set the WVP matrix for the skybox shader

    // Render the skybox mesh
    m_pMesh->Render();
}