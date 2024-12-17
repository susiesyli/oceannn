#pragma once

#ifndef SKYBOXTECHNIQUE_H
#define SKYBOXTECHNIQUE_H

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
#include "ply.h"
#include "gfxDefs.h"

class SkyboxTechnique
{
    public:
        SkyboxTechnique();

        virtual bool Init();

        void SetWVP(const glm::mat4& WVP);
        void SetTextureUnit(unsigned int TextureUnit);

    private:

        GLuint m_WVPLocation;
        GLuint m_textureLocation;
};

#endif
