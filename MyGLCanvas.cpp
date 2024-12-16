#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace std;

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

	fogColor = glm::vec3(0.8f, 0.8f, 0.8f);
	fogDensity = 0.2f;

	noiseScale = 0.1f;
	noiseSpeed = 0.5f;

	numDrops = 1000;
	useFog = false;

	//useDiffuse = true;
	firstTime = true;

	myTextureManager = new TextureManager();
	myShaderManager = new ShaderManager();
	myObjectPLY = new ply("./data/cube.ply");
	myEnvironmentPLY = new ply("./data/cube.ply");
	mySunPLY = new ply("./data/sphere.ply");
    myRainPLY = new ply("./data/sphere.ply");

	initDrops();
}

MyGLCanvas::~MyGLCanvas() {
	delete myTextureManager;
	delete myShaderManager;
	delete myObjectPLY;
	delete myEnvironmentPLY;
}

void MyGLCanvas::initShaders() {

    // the original environment mapping line 
	// myTextureManager->loadTexture("environMap", "./data/lol.ppm");
	myTextureManager->loadTexture("objectTexture", "./data/wave.ppm");

	myShaderManager->addShaderProgram("objectShaders", "shaders/330/object-vert.shader", "shaders/330/object-frag.shader");
	myObjectPLY->buildArrays();
	myObjectPLY->bindVBO(myShaderManager->getShaderProgram("objectShaders")->programID);

	myShaderManager->addShaderProgram("environmentShaders", "shaders/330/environment-vert.shader", "shaders/330/environment-frag.shader");
	myEnvironmentPLY->buildArrays();
	myEnvironmentPLY->bindVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);

	myShaderManager->addShaderProgram("sunShaders", "shaders/330/sun-vert.shader", "shaders/330/sun-frag.shader");
	mySunPLY->buildArrays();
	mySunPLY->bindVBO(myShaderManager->getShaderProgram("sunShaders")->programID);

    myShaderManager->addShaderProgram("rainShaders", "shaders/330/rain-vert.shader", "shaders/330/rain-frag.shader");
	myRainPLY->buildArrays();
	myRainPLY->bindVBO(myShaderManager->getShaderProgram("rainShaders")->programID);
}

void MyGLCanvas::initDrops() {
	float oceanSize = 50.0f;

	std::vector<std::pair<int, int>> coordinates;
	for (int x = 0; x < 50; x++) {
		for (int z = 0; z < 50; z++) {
			coordinates.emplace_back(x/2, z);
		}
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(coordinates.begin(), coordinates.end(), gen);



	for (int i = 0; i < numDrops; i++) {
		ply* currDropPLY = new ply("./data/sphere.ply");
		// float speed = 0.2f;
		glm::mat4 modelMatrix = glm::mat4(1.0);
		std::pair<int, int> coordinate = coordinates[i];

		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_int_distribution<> dis(0, 50);

		float random_number = dis(gen) / 50;

		modelMatrix = glm::translate(modelMatrix, glm::vec3((float)coordinate.first + random_number, 0.0f, (float)coordinate.second - random_number));
		std::cout << (float)coordinate.first + random_number << ", " << ", 0.0, " << (float)coordinate.second - random_number << std::endl;
		rainParticle currParticle; 
		currParticle.rainDrop = currDropPLY;
		currParticle.speed = 0.5f;
		currParticle.modelMatrix = modelMatrix;
		rainDrops.emplace_back(currParticle);
	}
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

	modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 0.1f, 10.0f)); 

	glm::vec4 lookVec(0.0f, 0.0f, -1.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	//Pass first texture info to our shader 

	glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, myTextureManager->getCubeMapTextureID("environMap")); // skybox cubemap texture 
	glBindTexture(GL_TEXTURE_2D, myTextureManager->getTextureID("environMap"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, myTextureManager->getTextureID("objectTexture"));

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
	GLint fogColorLoc = glGetUniformLocation(objectShaderProgram, "fogColor");
	GLint fogDensityLoc = glGetUniformLocation(objectShaderProgram, "fogDensity");
	GLint noiseScaleLoc = glGetUniformLocation(objectShaderProgram, "noiseScale");
	GLint noiseSpeedLoc = glGetUniformLocation(objectShaderProgram, "noiseSpeed");
	GLint useFogLoc = glGetUniformLocation(objectShaderProgram, "useFog");


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
	glUniform3fv(fogColorLoc, 1, glm::value_ptr(fogColor));
	glUniform1f(fogDensityLoc, fogDensity);
	glUniform1f(noiseScaleLoc, noiseScale);
	glUniform1f(noiseSpeedLoc, noiseSpeed);
	glUniform1i(useFogLoc, useFog);

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
	glm::mat4 environmentModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(7.0f, 7.0f, 7.0f));
	// Pass matrix uniforms for environment shader
	glUniformMatrix4fv(envModelLoc, 1, GL_FALSE, glm::value_ptr(environmentModelMatrix));
	glUniformMatrix4fv(envViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(envProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	// Pass texture unit for environment shader
    // GLint environMapLocEnv = glGetUniformLocation(environmentShaderProgram, "cubeMap");
	// GLint environMapLocEnv = glGetUniformLocation(environmentShaderProgram, "environMap");
	//glUniform1i(environMapLocEnv, 0);  // GL_TEXTURE0
    //glDepthMask(GL_FALSE); // Disable depth writes
	//myEnvironmentPLY->renderVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);
    //glDepthMask(GL_TRUE);  // Re-enable depth writes

    // shade raindrops 
    // loop through all num drops and pass them to the rain shader
    glUseProgram(myShaderManager->getShaderProgram("rainShaders")->programID);
    // Get shader program
    GLuint rainShaderProgram = myShaderManager->getShaderProgram("rainShaders")->programID;
    // Variable binding for environment shader
    GLint rainModelLoc = glGetUniformLocation(rainShaderProgram, "rainModel");
    viewLoc = glGetUniformLocation(rainShaderProgram, "rainView");
    projLoc = glGetUniformLocation(rainShaderProgram, "rainProjection");

    for (int i = 0; i < numDrops; i++) {
        // Create rain model matrix (scaled up) 
        glm::mat4 rainModelMatrix = rainDrops[i].modelMatrix;
        rainModelMatrix *= 0.05;

        // rainModelMatrix = glm::translate(rainModelMatrix, glm::vec3(lightPos));
        rainModelMatrix = glm::scale(rainModelMatrix, glm::vec3(0.01f, 0.01f, 0.01f));
        // Pass matrix uniforms for environment shader
        glUniformMatrix4fv(rainModelLoc, 1, GL_FALSE, glm::value_ptr(rainModelMatrix));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
        mySunPLY->renderVBO(myShaderManager->getShaderProgram("rainShaders")->programID);
    }


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
	mySunPLY->renderVBO(myShaderManager->getShaderProgram("sunShaders")->programID);
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
		float zoomFactor = Fl::event_dy() * 0.1f;
		eyePosition.z += zoomFactor;
		eyePosition.z = std::max(2.0f, std::min(10.0f, eyePosition.z));

		redraw();
		return 1;
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

