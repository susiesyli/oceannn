#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
// #include "skybox.h"

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
	lightIntensity = 0.9f;
	textureBlend = 1.0f;
	repeatU = 5;
	repeatV = 5;

	waveSpeed = glm::vec2(0.1f, 0.05f);
	waveAmplitude = 0.02f;
	waveFrequency = 1.5f;

	fogColor = glm::vec3(0.7f, 0.7f, 0.7f);
	fogDensity = 0.2f;

	noiseScale = 0.1f;
	noiseSpeed = 0.5f;

	numDrops = 1000;
	useFog = false;

	//useDiffuse = true;
	firstTime = true;

	myTextureManager = new TextureManager();
	myShaderManager = new ShaderManager();

	// read in geometry models 
	myObjectPLY = new ply("./data/cube.ply");
	myEnvironmentPLY = new ply("./data/sphere.ply");
	mySunPLY = new ply("./data/sphere.ply");
	myRainPLY = new ply("./data/sphere.ply");
	myCloudPLY = new ply("./data/sphere.ply");

	initDrops();
}

MyGLCanvas::~MyGLCanvas() {
	delete myTextureManager;
	delete myShaderManager;
	delete myObjectPLY;
	delete myEnvironmentPLY;
	// delete mySkyboxPLY;
	delete mySunPLY;
	delete myRainPLY;
}

void MyGLCanvas::initShaders() {
	myTextureManager->loadTexture("environMap", "./data/sky3.ppm");
	myTextureManager->loadTexture("objectTexture", "./data/wave.ppm");
    myTextureManager->loadTexture("cloudTexture", "./data/cloud.ppm");

	myShaderManager->addShaderProgram("objectShaders", "shaders/330/object-vert.shader", "shaders/330/object-frag.shader");
	myObjectPLY->buildArrays();
	myObjectPLY->bindVBO(myShaderManager->getShaderProgram("objectShaders")->programID);

	myShaderManager->addShaderProgram("environmentShaders", "shaders/330/environment-vert.shader", "shaders/330/environment-frag.shader");
	myEnvironmentPLY->buildArrays();
	myEnvironmentPLY->bindVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);

	myShaderManager->addShaderProgram("sunShaders", "shaders/330/sun-vert.shader", "shaders/330/sun-frag.shader");
	mySunPLY->buildArrays();
	mySunPLY->bindVBO(myShaderManager->getShaderProgram("sunShaders")->programID);

	// myShaderManager->addShaderProgram("rainShaders", "shaders/330/rain-vert.shader", "shaders/330/rain-frag.shader");
	// myRainPLY->buildArrays();
	// myRainPLY->bindVBO(myShaderManager->getShaderProgram("rainShaders")->programID);

	myShaderManager->addShaderProgram("cloudShaders", "shaders/330/cloud-vert.shader", "shaders/330/cloud-frag.shader");
	myCloudPLY->buildArrays();
	myCloudPLY->bindVBO(myShaderManager->getShaderProgram("cloudShaders")->programID);


	// Load the textures for the skybox faces
	// myTextureManager->loadTexture("skyboxLeft", "./data/left.ppm");
	// myTextureManager->loadTexture("skyboxRight", "./data/right.ppm");
	// myTextureManager->loadTexture("skyboxTop", "./data/top.ppm");
	// myTextureManager->loadTexture("skyboxBottom", "./data/bottom.ppm");
	// myTextureManager->loadTexture("skyboxFront", "./data/front.ppm");
	// myTextureManager->loadTexture("skyboxBack", "./data/back.ppm");
	// myShaderManager->addShaderProgram("skyboxShaders", "shaders/330/skybox-vert.shader", "shaders/330/skybox-frag.shader");
	// mySkyboxPLY->buildArrays();  // Ensure the cube's vertex data is ready
	// mySkyboxPLY->bindVBO(myShaderManager->getShaderProgram("skyboxShaders")->programID);
}

void MyGLCanvas::initDrops() {
	int grid_size = static_cast<int>(std::sqrt(numDrops)); // Assuming a square grid for simplicity
	float step_size_x = 10.0f / (grid_size - 1);  // From -5 to 5, with grid_size steps
	float step_size_z = 10.0f / (grid_size - 1);

	// Generate the coordinates uniformly across the grid
	for (int i = 0; i < grid_size; i++) {
		for (int j = 0; j < grid_size; j++) {
			float x = -5 + i * step_size_x;  // x coordinate from -5 to 5
			float z = -5 + j * step_size_z;  // z coordinate from -5 to 5
			glm::vec2 dropLocation = glm::vec2(x, z);
			rainDrops.emplace_back(dropLocation);
		}
	}

	for (const glm::vec2& position : rainDrops) {
		glm::vec3 translation(position.x, 0.1f, position.y); // Use 2D position for x and z
		rainTranslations.push_back(translation);
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

	// skybox rendering 
		// glUseProgram(myShaderManager->getShaderProgram("skyboxShaders")->programID);

		// GLint skyModelLoc = glGetUniformLocation(myShaderManager->getShaderProgram("skyboxShaders")->programID, "skyModel");
		// GLuint projectionLoc = glGetUniformLocation(myShaderManager->getShaderProgram("skyboxShaders")->programID, "skyProjection");
		// GLuint viewLoc = glGetUniformLocation(myShaderManager->getShaderProgram("skyboxShaders")->programID, "skyView");

		// // Create sun model matrix (scaled up) 
		// glm::mat4 skyModelMatrix = glm::mat4(1.0f);
		// skyModelMatrix = glm::scale(skyModelMatrix, glm::vec3(50.0f, 50.0f,50.0f));
		// // Pass matrix uniforms for environment shader
		// glUniformMatrix4fv(skyModelLoc, 1, GL_FALSE, glm::value_ptr(skyModelMatrix));

		// // Set the projection and view matrix for the skybox
		// glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
		// glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		// // Bind the skybox textures
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, myTextureManager->getTextureID("skyboxLeft"));
		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, myTextureManager->getTextureID("skyboxRight"));
		// glActiveTexture(GL_TEXTURE2);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, myTextureManager->getTextureID("skyboxTop"));
		// glActiveTexture(GL_TEXTURE3);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, myTextureManager->getTextureID("skyboxBottom"));
		// glActiveTexture(GL_TEXTURE4);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, myTextureManager->getTextureID("skyboxFront"));
		// glActiveTexture(GL_TEXTURE5);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, myTextureManager->getTextureID("skyboxBack"));

		// // std::cout << "here" << std::endl;
		// glUniform1i(glGetUniformLocation(myShaderManager->getShaderProgram("skyboxShaders")->programID, "cubemapTexture"), 0); // Tell the shader to use texture unit 0

		// // Render the skybox (cube)
		// mySkyboxPLY->renderVBO(myShaderManager->getShaderProgram("skyboxShaders")->programID);


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

	GLint skyLightPosLoc = glGetUniformLocation(environmentShaderProgram, "lightPos");
	glUniform3fv(skyLightPosLoc, 1, glm::value_ptr(lightPos));
	GLint skyLightIntensityLoc = glGetUniformLocation(environmentShaderProgram, "lightIntensity");
	glUniform1f(skyLightIntensityLoc, lightIntensity);

	// glUniform3fv(skyLightIntensityLoc, 1, glm::value_ptr(lightIntensity));
	// Create environment model matrix (scaled up)
	glm::mat4 environmentModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 20.0f, 20.0f));
	// Pass matrix uniforms for environment shader
	glUniformMatrix4fv(envModelLoc, 1, GL_FALSE, glm::value_ptr(environmentModelMatrix));
	glUniformMatrix4fv(envViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(envProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	// Pass texture unit for environment shader
	GLint environMapLocEnv = glGetUniformLocation(environmentShaderProgram, "environMap");
	glUniform1i(environMapLocEnv, 0);  // GL_TEXTURE0

	myEnvironmentPLY->renderVBO(myShaderManager->getShaderProgram("environmentShaders")->programID);

	// // raindrop instancing
	// GLuint instanceVBO;
	// glGenBuffers(1, &instanceVBO);
	// glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	// glBufferData(GL_ARRAY_BUFFER, rainTranslations.size() * sizeof(glm::vec3), rainTranslations.data(), GL_STATIC_DRAW);

	// // Bind instance attribute (location = 2, for example)
	// glBindVertexArray(sphereVAO); // Bind the sphere's VAO
	// glEnableVertexAttribArray(2); // Enable attribute at location 2
	// glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	// glVertexAttribDivisor(2, 1); // Update this attribute per instance


	// shade raindrops 
	// // loop through all num drops and pass them to the rain shader
	// glUseProgram(myShaderManager->getShaderProgram("rainShaders")->programID);
	// // Get shader program
	// GLuint rainShaderProgram = myShaderManager->getShaderProgram("rainShaders")->programID;
	// // Variable binding for environment shader
	// GLint rainModelLoc = glGetUniformLocation(rainShaderProgram, "rainModel");
	// viewLoc = glGetUniformLocation(rainShaderProgram, "rainView");
	// projLoc = glGetUniformLocation(rainShaderProgram, "rainProjection");

	// GLuint rainPosBuffer;
	// glGenBuffers(1, &rainPosBuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, rainPosBuffer);
	// glBufferData(GL_ARRAY_BUFFER, rainDrops.size() * sizeof(glm::vec3), rainDrops.data(), GL_STATIC_DRAW);
	// GLint rainPosLocation = glGetUniformLocation(rainShaderProgram, "rainPositions");


	// // Create rain model matrix (scaled up) 
	// //glUniformMatrix4fv(rainModelLoc, 1, GL_FALSE, glm::value_ptr(rainModelMatrix));
	// glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	// glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	// glUniform3fv(rainPosLocation, (GLsizei)rainDrops.size(), glm::value_ptr(rainDrops[0]));
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// myRainPLY->renderVBO(myShaderManager->getShaderProgram("rainShaders")->programID);

	// draw sun sphere
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(myShaderManager->getShaderProgram("sunShaders")->programID);
	// Get shader program
	GLuint sunShaderProgram = myShaderManager->getShaderProgram("sunShaders")->programID;
	// Variable binding for environment shader
	GLint sunModelLoc = glGetUniformLocation(sunShaderProgram, "sunModel");
	GLint sunViewLoc = glGetUniformLocation(sunShaderProgram, "sunView");
	GLint sunProjLoc = glGetUniformLocation(sunShaderProgram, "sunProjection");
	GLint sunLightIntensityLoc = glGetUniformLocation(sunShaderProgram, "lightIntensity");
	glUniform1f(sunLightIntensityLoc, lightIntensity);
	// Create sun model matrix (scaled up) 
	glm::mat4 sunModelMatrix = glm::mat4(1.0f);
	sunModelMatrix = glm::translate(sunModelMatrix, glm::vec3(lightPos));
	sunModelMatrix = glm::scale(sunModelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
	// Pass matrix uniforms for environment shader
	glUniformMatrix4fv(sunModelLoc, 1, GL_FALSE, glm::value_ptr(sunModelMatrix));
	glUniformMatrix4fv(sunViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(sunProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
	mySunPLY->renderVBO(myShaderManager->getShaderProgram("sunShaders")->programID);

	glUseProgram(myShaderManager->getShaderProgram("cloudShaders")->programID);
	GLuint cloudShaderProgram = myShaderManager->getShaderProgram("cloudShaders")->programID;

	GLint cloudModelLoc = glGetUniformLocation(cloudShaderProgram, "cloudModel");
	GLint cloudViewLoc = glGetUniformLocation(cloudShaderProgram, "cloudView");
	GLint cloudProjLoc = glGetUniformLocation(cloudShaderProgram, "cloudProjection");
    
	glm::mat4 cloudModelMatrix = glm::mat4(1.0f);
	cloudModelMatrix = glm::translate(cloudModelMatrix, glm::vec3(lightPos.x+1, lightPos.y-0.25, lightPos.z));
	cloudModelMatrix = glm::scale(cloudModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

	glUniformMatrix4fv(cloudModelLoc, 1, GL_FALSE, glm::value_ptr(cloudModelMatrix));
	glUniformMatrix4fv(cloudViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(cloudProjLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
    glUniform3f(glGetUniformLocation(cloudShaderProgram, "u_TransparentColor"), 0.0, 0.0, 0.0); // Black
    glUniform1f(glGetUniformLocation(cloudShaderProgram, "u_Threshold"), 0.04); // Threshold
    GLint cloudTextureLoc = glGetUniformLocation(cloudShaderProgram, "cloudTexture");
	glUniform1i(cloudTextureLoc, 0);  // GL_TEXTURE0

	myCloudPLY->renderVBO(myShaderManager->getShaderProgram("cloudShaders")->programID);
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
	static int lastX = 0;
	static bool isMouseDown = false;

	switch (e) {
	case FL_DRAG: {
		if (isMouseDown) {
			int dx = Fl::event_x() - lastX; // find magnitute of movement 
			rotWorldVec.y += dx * 0.5f;  // allow horizontal rotation 
			rotWorldVec.y = fmod(rotWorldVec.y, 360.0f); // normalize to 0, 360 range 
			lastX = Fl::event_x(); // Update last mouse position
			redraw();
			return 1;
		}
		break;
	}
	case FL_MOVE:
	case FL_PUSH: {
		if (Fl::event_button() == FL_LEFT_MOUSE) { // pressed mouse or touchpad 
			isMouseDown = true;
			lastX = Fl::event_x();
		}
		return 1;
	}

	case FL_RELEASE: {
		if (Fl::event_button() == FL_LEFT_MOUSE) {
			isMouseDown = false;
		}
		return 1;
	}

	case FL_KEYUP:
	case FL_MOUSEWHEEL: {
		float zoomFactor = Fl::event_dy() * 0.1f;
		eyePosition.z += zoomFactor;
		eyePosition.z = std::max(2.0f, std::min(5.0f, eyePosition.z));

		redraw();
		return 1;
		break;
	}
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

	myShaderManager->addShaderProgram("sunShaders", "shaders/330/sun-vert.shader", "shaders/330/sun-frag.shader");
	mySunPLY->bindVBO(myShaderManager->getShaderProgram("sunShaders")->programID);

	myShaderManager->addShaderProgram("rainShaders", "shaders/330/rain-vert.shader", "shaders/330/rain-frag.shader");
	myRainPLY->bindVBO(myShaderManager->getShaderProgram("rainShaders")->programID);

	myShaderManager->addShaderProgram("cloudShaders", "shaders/330/cloud-vert.shader", "shaders/330/cloud-frag.shader");
	myRainPLY->bindVBO(myShaderManager->getShaderProgram("cloudShaders")->programID);


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