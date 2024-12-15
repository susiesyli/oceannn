/*  =================== File Information =================
	File Name: object.h
	Description:
	Author: Michael Shah
	Last Modified: 4/2/14

	Purpose: 
	Usage:	

	Further Reading resources: 
	===================================================== */

#include <iostream>
#include <cmath>
#include "TextureManager.h"
#include <glm/gtc/constants.hpp>
#include <vector> 
#include <string>

#define PI glm::pi<float>()

using namespace::std;

TextureManager::TextureManager(){
}

TextureManager ::~TextureManager(){
	for (auto const& it : textures) {
		delete it.second;  //delete the memory of the ShaderProgram object
	}
	textures.clear();  
}

/*	===============================================
Desc:
Precondition:
Postcondition:
=============================================== */
void TextureManager::loadTexture(string textureName, string fileName) {
	ppm* curPPM = NULL;
	auto it = textures.find(textureName);
	if (it == textures.end()) {  //ppm not found
		curPPM = new ppm(fileName);
	}
	else {
		curPPM = it->second;
	}
	curPPM->bindTexture();
	textures[textureName] = curPPM;  //add the newly added texture to the map
}

// added for rendering sky box 
void TextureManager::loadCubeMap(string textureName, std::vector<std::string> faceFiles) {
    // Check if the cube map is already loaded
    auto it = cubeMapTextures.find(textureName);
    if (it != cubeMapTextures.end()) {
        std::cout << "Cube map texture '" << textureName << "' already loaded!" << std::endl;
        return;
    }

    // Generate and bind a texture ID for the cube map
    GLuint cubeMapID;
    glGenTextures(1, &cubeMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);

    // Load each face of the cube map
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faceFiles.size(); ++i) {
        ppm facePPM(faceFiles[i]); // Assuming PPM class can read image data
        unsigned char* data = facePPM.getImageData();

        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                facePPM.getWidth(), facePPM.getHeight(), 0,
                GL_RGB, GL_UNSIGNED_BYTE, data
            );
        } else {
            std::cerr << "Failed to load cube map face: " << faceFiles[i] << std::endl;
        }
    }

    // Set cube map texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Store the cube map texture ID
    cubeMapTextures[textureName] = cubeMapID;
    std::cout << "Cube map texture '" << textureName << "' loaded successfully." << std::endl;
}



void TextureManager::deleteTexture(string textureName) {
	ppm* curPPM = NULL;
	auto it = textures.find(textureName);
	if (it != textures.end()) {  //ppm not found
		delete it->second;
		textures.erase(it); //erasing the entry by key 
	}
}

// for regular 2D texture 
unsigned int TextureManager::getTextureID(std::string textureName) {
	auto it = textures.find(textureName);
	if (it == textures.end()) {  //ppm not found
		cout << "texutre name not found!!!" << endl;
		return -1;
	}
	return it->second->getTextureID();
}

// for cubemap texture 
unsigned int TextureManager::getCubeMapTextureID(std::string textureName) {
    auto it = cubeMapTextures.find(textureName);
    if (it == cubeMapTextures.end()) {
        std::cout << "Cube map texture not found!!!" << std::endl;
        return -1;
    }
    return it->second;
}