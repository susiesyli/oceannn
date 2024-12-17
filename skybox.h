#pragma once

#ifndef SKYBOX_H
#define SKYBOX_H

#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif
#include <FL/glut.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <time.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <random> 

#include "TextureManager.h"
#include "ShaderManager.h"
#include "skyboxTechnique.h"
#include "cubemapTexture.h"
#include "ply.h"
#include "gfxDefs.h"

class SkyBox
{
public:
    SkyBox(glm::vec3* eyePos, glm::vec3* lookAt, glm::mat4 projection);

    bool Init(const string& Directory,
        const string& PosXFilename,
        const string& NegXFilename,
        const string& PosYFilename,
        const string& NegYFilename,
        const string& PosZFilename,
        const string& NegZFilename);

    void Render();

private: 
    glm::vec3* m_eyePos;
    glm::vec3* m_lookAt;
    glm::mat4 m_projection;
    SkyboxTechnique* m_pSkyboxTechnique;
    CubemapTexture* m_pCubemapTex;
    Mesh* m_pMesh;
};

#endif
