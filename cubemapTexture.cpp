#include "cubemapTexture.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "cubemapTexture.h"

CubemapTexture::CubemapTexture(const std::string& PosXFilename,
                                const std::string& NegXFilename,
                                const std::string& PosYFilename,
                                const std::string& NegYFilename,
                                const std::string& PosZFilename,
                                const std::string& NegZFilename)
{
    // store the files names for each face of the cubemap
    m_fileNames[0] = PosXFilename;
    m_fileNames[1] = NegXFilename;
    m_fileNames[2] = PosYFilename;
    m_fileNames[3] = NegYFilename;
    m_fileNames[4] = PosZFilename;
    m_fileNames[5] = NegZFilename;
}


CubemapTexture::~CubemapTexture()
{
    glDeleteTextures(1, &m_textureObj); // delete texture 
}


bool CubemapTexture::Load()
{
    glGenTextures(1, &m_textureObj);  // Generate texture object
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);  // Bind cubemap texture

    // Array of texture types corresponding to each face of the cubemap
    GLenum types[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    // TextureManager textureManager;  

    // loop through each face of the cubemap and load the image using ppm class
    for (unsigned int i = 0; i < 6; i++) {
        // Use TextureManager to load the ppm file
        ppm* curPPM = new ppm(m_fileNames[i]);
        curPPM->bindTexture();

        // call glTexImage2D to load the texture into the cubemap
        glTexImage2D(types[i], 0, GL_RGB, curPPM->getWidth(), curPPM->getHeight(), 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, curPPM->getData());

        delete curPPM; // cleanup 
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;  // Return success
}

// must be called before the texture can be used for drawing the skybox. 
// bind to GL_TEXTURE_CUBE_MAP - the enum in Load() 
void CubemapTexture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
