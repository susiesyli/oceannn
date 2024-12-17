#pragma once

#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

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
#include <string>
#include <algorithm>
#include <random> 

#include "TextureManager.h"
#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"

class CubemapTexture
{
public:

    CubemapTexture(const std::string& PosXFilename,
                    const std::string& NegXFilename,
                    const std::string& PosYFilename,
                    const std::string& NegYFilename,
                    const std::string& PosZFilename,
                    const std::string& NegZFilename);

    ~CubemapTexture();

    bool Load();

    void Bind(GLenum TextureUnit);

private:
    std::string m_fileNames[6];
    GLuint m_textureObj;
};

#endif
