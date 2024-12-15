#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//using namespace std;

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char* l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);

	eyePosition = glm::vec3(0.0f, 0.0f, 3.0f);
	lookatPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	rotVec = glm::vec3(0.0f, 0.0f, 0.0f);
	rotWorldVec = glm::vec3(0.0f, 0.0f, 0.0f);
	lightPos = eyePosition;

	viewAngle = 60;
	clipNear = 0.01f;
	clipFar = 20000.0f;
	lightAngle = 0.0f;
	lightIntensity = 0.0f;
	textureBlend = 1.0f;
	repeatU = 5;
	repeatV = 5;

	waveSpeed = glm::vec2(0.1f, 0.05f);
	waveAmplitude = 0.02f;
	waveFrequency = 1.5f;

	//useDiffuse = true;
	firstTime = true;

	myTextureManager = new TextureManager();
	myShaderManager = new ShaderManager();
	myObjectPLY = new ply("./data/cube.ply");
	myEnvironmentPLY = new ply("./data/cube.ply");
	mySunPLY = new ply("./data/sphere.ply");
	myParticlePLY = new ply("./data/sphere.ply");
}

MyGLCanvas::~MyGLCanvas() {
	delete myTextureManager;
	delete myShaderManager;
	delete myObjectPLY;
	delete myEnvironmentPLY;
}

void MyGLCanvas::initShaders() {
    // load 6 faces of sky box 
    std::vector<std::string> skyboxFaces = {
        "./data/skybox/right.ppm",
        "./data/skybox/left.ppm",
        "./data/skybox/bottom.ppm",
        "./data/skybox/top.ppm",
        "./data/skybox/front.ppm",
        "./data/skybox/back.ppm"
    };

    myTextureManager->loadCubeMap("environMap", skyboxFaces); // load environment map 
	myTextureManager->loadTexture("objectTexture", "./data/oceanNormal.ppm"); // load object (ocean) map 
	myTextureManager->loadTexture("fogTexture", "./data/top.ppm");

	myShaderManager->addShaderProgram("objectShaders", "shaders/330/object-vert.shader", "shaders/330/object-frag.shader");
	myObjectPLY->buildArrays();
	myObjectPLY->bindVBO(myShaderManager->getShaderProgram("objectShaders")->programID);

	myShaderManager->addShaderProgram("environmentShaders", "shaders/330/environment-vert.shader", "shaders/330/environment-frag.shader");
	myEnvironmentPLY->buildArrays();
	myEnvironmentPLY->bindVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);

	myShaderManager->addShaderProgram("sunShaders", "shaders/330/sun-vert.shader", "shaders/330/sun-frag.shader");
	mySunPLY->buildArrays();
	mySunPLY->bindVBO(myShaderManager->getShaderProgram("sunShaders")->programID);


	myShaderManager->addShaderProgram("particleShaders", "shaders/330/particle-vert.shader", "shaders/330/particle-frag.shader");
	myParticlePLY->buildArrays();
	myParticlePLY->bindVBO(myShaderManager->getShaderProgram("particleShaders")->programID);

}

void MyGLCanvas::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!valid()) {  //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		/****************************************/
		/*          Enable z-buferring          */
		/****************************************/

		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(1, 1);
		if (firstTime == true) {
			firstTime = false;
			initShaders();
		}
	}
	// Clear the buffer of colors in each bit plane.
	// bit plane - A set of bits that are on or off (Think of a black and white image)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
}

void MyGLCanvas::drawScene() {
	glm::mat4 viewMatrix = glm::lookAt(eyePosition, lookatPoint, glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, TO_RADIANS(rotWorldVec.x), glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, TO_RADIANS(rotWorldVec.y), glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, TO_RADIANS(rotWorldVec.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelMatrix = glm::mat4(1.0);

	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.1f, 0.0f));

	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS(rotVec.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// modelMatrix = glm::scale(modelMatrix, glm::vec3(10000.0f, 0.1f, 10000.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(10000.0f, 0.185f, 10000.0f));

	glm::vec4 lookVec(0.0f, 0.0f, -1.0f, 0.0f);
	// glm::vec4 lookVec(0.0f, 0.0f, -1.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	//Pass first texture info to our shader 
    GLuint cubeMapID = myTextureManager->getCubeMapTextureID("environMap");
    if (cubeMapID == 0) {
        std::cerr << "Cube map texture ID is 0. Skybox will not render!" << std::endl;
    }

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, myTextureManager->getCubeMapTextureID("environMap")); // skybox cubemap texture 
	// glBindTexture(GL_TEXTURE_2D, myTextureManager->getTextureID("environMap"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, myTextureManager->getTextureID("objectTexture"));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, myTextureManager->getTextureID("fogTexture"));

	//first draw the object sphere
	glUseProgram(myShaderManager->getShaderProgram("objectShaders")->programID);

	//TODO: add variable binding
	GLuint objectShaderProgram = myShaderManager->getShaderProgram("objectShaders")->programID;

	auto currentTime = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float> elapsedTime = currentTime - startTime;
	float totalTime = elapsedTime.count();

	// Get uniform locations
	GLint modelLoc = glGetUniformLocation(objectShaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(objectShaderProgram, "view");
	GLint projLoc = glGetUniformLocation(objectShaderProgram, "projection");
	GLint lightPosLoc = glGetUniformLocation(objectShaderProgram, "lightPos");
	GLint lightIntensityLoc = glGetUniformLocation(objectShaderProgram, "lightIntensity");
	GLint viewPosLoc = glGetUniformLocation(objectShaderProgram, "viewPos");
	GLint textureBlendLoc = glGetUniformLocation(objectShaderProgram, "textureBlend");
	//GLint useDiffuseLoc = glGetUniformLocation(objectShaderProgram, "useDiffuse");
	GLint repeatULoc = glGetUniformLocation(objectShaderProgram, "repeatU");
	GLint repeatVLoc = glGetUniformLocation(objectShaderProgram, "repeatV");
	GLint timeLoc = glGetUniformLocation(objectShaderProgram, "time");
	GLint waveSpeedLoc = glGetUniformLocation(objectShaderProgram, "waveSpeed");
	GLint waveAmplitudeLoc = glGetUniformLocation(objectShaderProgram, "waveAmplitude");
	GLint waveFrequencyLoc = glGetUniformLocation(objectShaderProgram, "waveFrequency");

	// Pass matrix uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));

	// Pass other uniforms

	// add pass light angle 
	glm::vec4 lightPos(0.0f, 0.0f, 1.0f, 0.0f);
    // add light rotation angle 
	lightPos = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Initial light position
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), TO_RADIANS(lightAngle), glm::vec3(0.0, 0.0, 1.0));
	lightPos = rotationMatrix * lightPos;

	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
	glm::vec4 rotatedEye = glm::inverse(viewMatrix) * glm::vec4(eyePosition, 1.0f);
	glm::vec3 transformedEye = glm::vec3(rotatedEye);
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(transformedEye));
	glUniform1f(textureBlendLoc, textureBlend);
	glUniform1f(repeatULoc, repeatU);
	glUniform1f(repeatVLoc, repeatV);
	glUniform1f(timeLoc, totalTime);
	glUniform2f(waveSpeedLoc, waveSpeed[0], waveSpeed[1]);
	glUniform1f(waveAmplitudeLoc, waveAmplitude);
	glUniform1f(waveFrequencyLoc, waveFrequency);
	glUniform1f(lightIntensityLoc, lightIntensity);

	// Pass texture units
	GLint environMapLoc = glGetUniformLocation(objectShaderProgram, "environMap");
	GLint objectTextureLoc = glGetUniformLocation(objectShaderProgram, "objectTexture");
	
	glUniform1i(environMapLoc, 0);  // GL_TEXTURE0
	glUniform1i(objectTextureLoc, 1);  // GL_TEXTURE1
    
	myObjectPLY->renderVBO(myShaderManager->getShaderProgram("objectShaders")->programID);

	// 2. draw the enviroment cube map
	glUseProgram(myShaderManager->getShaderProgram("environmentShaders")->programID);
	// Get shader program
	GLuint environmentShaderProgram = myShaderManager->getShaderProgram("environmentShaders")->programID;
	// Variable binding for environment shader
	GLint envModelLoc = glGetUniformLocation(environmentShaderProgram, "model");
	GLint envViewLoc = glGetUniformLocation(environmentShaderProgram, "view");
	GLint envProjLoc = glGetUniformLocation(environmentShaderProgram, "projection");
	// Create environment model matrix (scaled up)
	glm::mat4 environmentModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    // environmentModelMatrix = glm::translate(environmentModelMatrix, glm::vec3(0.0f, -0.1f, 0.0f));

	// Pass matrix uniforms for environment shader
	glUniformMatrix4fv(envModelLoc, 1, GL_FALSE, glm::value_ptr(environmentModelMatrix));
	glUniformMatrix4fv(envViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(envProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	// Pass texture unit for environment shader
    // GLint environMapLocEnv = glGetUniformLocation(environmentShaderProgram, "cubeMap");
	// glUniform1i(environMapLocEnv, 0);  // GL_TEXTURE0
    glDepthMask(GL_FALSE); // Disable depth writes, makes sure the sun shows up 
    glDepthFunc(GL_LEQUAL); 
	//myEnvironmentPLY->renderVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);
    glDepthMask(GL_TRUE);  // Re-enable depth writes
    glDepthFunc(GL_LESS);   


	// draw sun sphere
	glUseProgram(myShaderManager->getShaderProgram("sunShaders")->programID);
	// Get shader program
	GLuint sunShaderProgram = myShaderManager->getShaderProgram("sunShaders")->programID;
	// Variable binding for environment shader
	GLint sunModelLoc = glGetUniformLocation(sunShaderProgram, "sunModel");
	GLint sunViewLoc = glGetUniformLocation(sunShaderProgram, "sunView");
	GLint sunProjLoc = glGetUniformLocation(sunShaderProgram, "sunProjection");
	// Create sun model matrix (scaled up) 
	glm::mat4 sunModelMatrix = glm::mat4(1.0f);
	sunModelMatrix = glm::translate(sunModelMatrix, glm::vec3(lightPos));
	sunModelMatrix = glm::scale(sunModelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
	// Pass matrix uniforms for environment shader
	glUniformMatrix4fv(sunModelLoc, 1, GL_FALSE, glm::value_ptr(sunModelMatrix));
	glUniformMatrix4fv(sunViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(sunProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	//mySunPLY->renderVBO(myShaderManager->getShaderProgram("sunShaders")->programID);

	// particles!
	glUseProgram(myShaderManager->getShaderProgram("particleShaders")->programID);
	GLuint particleShaderProgram = myShaderManager->getShaderProgram("particleShaders")->programID;

	// Variable binding for environment shader
	GLint particleModelLoc = glGetUniformLocation(particleShaderProgram, "particleModel");
	GLint particleViewLoc = glGetUniformLocation(particleShaderProgram, "particleView");
	GLint particleProjLoc = glGetUniformLocation(particleShaderProgram, "particleProjection");
	GLint particleLightPosLoc = glGetUniformLocation(particleShaderProgram, "lightPos");
	GLint particleTextureLoc = glGetUniformLocation(particleShaderProgram, "particleTexture");

	glm::mat4 particleModelMatrix = glm::mat4(1.0f);
	particleModelMatrix = glm::translate(particleModelMatrix, glm::vec3(lightPos));
	particleModelMatrix = glm::scale(particleModelMatrix, glm::vec3(0.75f, 0.75f, 0.75f));

	glUniformMatrix4fv(particleModelLoc, 1, GL_FALSE, glm::value_ptr(particleModelMatrix));
	glUniformMatrix4fv(particleViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(particleProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	glUniform3fv(particleLightPosLoc, 1, glm::value_ptr(lightPos));
	glUniform1i(particleTextureLoc, 2);

	myParticlePLY->renderVBO(myShaderManager->getShaderProgram("particleShaders")->programID);

}


void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	xy_aspect = (float)width / (float)height;

	perspectiveMatrix = glm::perspective(TO_RADIANS(viewAngle), xy_aspect, clipNear, clipFar);
}


int MyGLCanvas::handle(int e) {
	//static int first = 1;
#ifndef __APPLE__
	if (firstTime && e == FL_SHOW && shown()) {
		firstTime = 0;
		make_current();
		GLenum err = glewInit(); // defines pters to functions of OpenGL V 1.2 and above
		if (GLEW_OK != err) {
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		else {
			//SHADER: initialize the shader manager and loads the two shader programs
			initShaders();
		}
	}
#endif	
	//printf("Event was %s (%d)\n", fl_eventnames[e], e);
	switch (e) {
	case FL_DRAG:
	case FL_MOVE:
	case FL_PUSH:
	case FL_RELEASE:
	case FL_KEYUP:
	case FL_MOUSEWHEEL:
		break;
	}
	return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
	Fl_Gl_Window::resize(x, y, w, h);
	puts("resize called");
}

void MyGLCanvas::reloadShaders() {
	myShaderManager->resetShaders();

	myShaderManager->addShaderProgram("objectShaders", "shaders/330/object-vert.shader", "shaders/330/object-frag.shader");
	myObjectPLY->bindVBO(myShaderManager->getShaderProgram("objectShaders")->programID);

	myShaderManager->addShaderProgram("environmentShaders", "shaders/330/environment-vert.shader", "shaders/330/environment-frag.shader");
	myEnvironmentPLY->bindVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);

	invalidate();
}

void MyGLCanvas::loadPLY(std::string filename) {
	delete myObjectPLY;
	myObjectPLY = new ply(filename);
	myObjectPLY->buildArrays();
	myObjectPLY->bindVBO(myShaderManager->getShaderProgram("objectShaders")->programID);
}

void MyGLCanvas::loadEnvironmentTexture(std::string filename) {
	myTextureManager->deleteTexture("environMap");
	myTextureManager->loadTexture("environMap", filename);
}

void MyGLCanvas::loadObjectTexture(std::string filename) {
	myTextureManager->deleteTexture("objectTexture");
	myTextureManager->loadTexture("objectTexture", filename);
}

