#include "GL/glew.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "SkyBox.h"
#include "Camera.h"
#include "Model.h"
#include "Lights.h"
#include "GameObject.h"
#include "PNGProcessor.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>
using namespace std;

const static int WINDOW_WIDTH = 1280;
const static int WINDOW_HEIGHT = 720;

GLFWwindow* window;
GLfloat lastFrame;

//glfw values
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

//Shader
Shader *shadowShader;
Shader *shader;
Shader *furShader;
Shader *animShader;

glm::mat4 biasMatrix{
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
};

// Generates fur map
PNGProcessor furProcessor;

std::vector<GameObject*> gameObjects;
//Models
GameObject *floorModel;
GameObject *benchModel;
GameObject *pillar, *button;
GameObject *werewolf;
GameObject *lowWall1, *lowWall2;
GameObject *lowWallEnd1, *lowWallEnd2;
GameObject *gate, *gateDoor;
GameObject *lWall1, *lWall2;
GameObject *animModel;
GameObject *player;

// For Collision testing
glm::vec3 prevPlayerPos;

// For Gameplay
bool buttonPressed = false;

Model* test;

//buffer and shader values

//fog shader variables
int fogSelector = 2;
//0 plane based; 1 range based
int depthFog = 1;
//normal map functions
bool normalMapped = true;


// Framebuffer parameters
GLuint frameBuffer;
GLuint depthTexture;

// Fur Settings
float furLength = 1.0;
int layers = 30;
int furDensity = 50000;
float furFlowOffset = 0; // For fur animation/ movement.
bool increment = false;
GLuint furMap;
GLuint furTexture;

DirLight dirLight = {
	0.5f, 2.0f, 2.0f,
	0.4f, 0.4f, 0.4f,
	0.4f, 0.4f, 0.4f,
	0.4f, 0.4f, 0.4f,
};

SpotLight light{

	glm::cos(glm::radians(12.5f)),
	glm::cos(glm::radians(17.5f)),

	1.0f, 0.09f, 0.032f,

{ 0.5f, 0.5f, 0.5f },
{ 0.8f, 0.8f, 0.8f },
{ 0.5f, 0.5f, 0.5f }

};

PointLight pointLight{

	1.0f, 0.09f, 0.032f,

{ 0.0f, 0.0f, 0.0f },
{ 0.0f, 0.0f, 0.0f },
{ 0.0f, 0.0f, 0.0f },
{ 1.0f, 3.0f, 0.0f }
};

Camera camera(glm::vec3(0.0f, 3.0f, 0.0f));
glm::mat4 ProjectionMatrix = glm::perspective(camera.GetZoom(), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

void MouseCallback(GLFWwindow *window, double xPos, double yPos);

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);

void setMaterial()
{
	animShader->Use();
	glUniform1f(glGetUniformLocation(animShader->getID(), "material.shininess"), 32.0f);
	glUniform1f(glGetUniformLocation(animShader->getID(), "material.transparency"), 1.0f);
	glUniform3f(glGetUniformLocation(animShader->getID(), "view_pos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
}

void setAnimLights()
{
	animShader->Use();
	//camera light
	glUniform3f(glGetUniformLocation(animShader->getID(), "light.ambient"), light.ambient[0], light.ambient[1], light.ambient[2]);
	glUniform3f(glGetUniformLocation(animShader->getID(), "light.diffuse"), light.diffuse[0], light.diffuse[1], light.diffuse[2]);
	glUniform3f(glGetUniformLocation(animShader->getID(), "light.specular"), light.specular[0], light.specular[1], light.specular[2]);
	glUniform1f(glGetUniformLocation(animShader->getID(), "light.constant"), light.constant);
	glUniform1f(glGetUniformLocation(animShader->getID(), "light.linear"), light.linear);
	glUniform1f(glGetUniformLocation(animShader->getID(), "light.quadratic"), light.quadratic);
	glUniform3f(glGetUniformLocation(animShader->getID(), "light.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
	glUniform3f(glGetUniformLocation(animShader->getID(), "light.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
	glUniform1f(glGetUniformLocation(animShader->getID(), "light.cutOff"), light.cutOff);
	glUniform1f(glGetUniformLocation(animShader->getID(), "light.outerCutOff"), light.outerCutOff);
	//spotlight
	glUniform3f(glGetUniformLocation(animShader->getID(), "pointLight1.ambient"), pointLight.ambient[0], pointLight.ambient[1], pointLight.ambient[2]);
	glUniform3f(glGetUniformLocation(animShader->getID(), "pointLight1.diffuse"), pointLight.diffuse[0], pointLight.diffuse[1], pointLight.diffuse[2]);
	glUniform3f(glGetUniformLocation(animShader->getID(), "pointLight1.specular"), pointLight.specular[0], pointLight.specular[1], pointLight.specular[2]);
	glUniform1f(glGetUniformLocation(animShader->getID(), "pointLight1.constant"), pointLight.constant);
	glUniform1f(glGetUniformLocation(animShader->getID(), "pointLight1.linear"), pointLight.linear);
	glUniform1f(glGetUniformLocation(animShader->getID(), "pointLight1.quadratic"), pointLight.quadratic);
	glUniform3f(glGetUniformLocation(animShader->getID(), "pointLight1.position"), pointLight.position[0], pointLight.position[1], pointLight.position[2]);
	glUniform3f(glGetUniformLocation(animShader->getID(), "dirLight.direction"), dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]);
	glUniform3f(glGetUniformLocation(animShader->getID(), "dirLight.ambient"), dirLight.ambient[0], dirLight.ambient[1], dirLight.ambient[2]);
	glUniform3f(glGetUniformLocation(animShader->getID(), "dirLight.diffuse"), dirLight.diffuse[0], dirLight.diffuse[1], dirLight.diffuse[2]);
	glUniform3f(glGetUniformLocation(animShader->getID(), "dirLight.specular"), dirLight.specular[0], dirLight.specular[1], dirLight.specular[2]);

	glUniform1i(glGetUniformLocation(animShader->getID(), "fogSelector"), fogSelector);
	glUniform1i(glGetUniformLocation(animShader->getID(), "depthFog"), depthFog);
}

void setLights(Shader* shader)
{
	// Use our shader
	shader->Use();

	//set the directional light 
	glUniform3f(glGetUniformLocation(shader->getID(), "dirLight.direction"), dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]);
	glUniform3f(glGetUniformLocation(shader->getID(), "dirLight.ambient"), dirLight.ambient[0], dirLight.ambient[1], dirLight.ambient[2]);
	glUniform3f(glGetUniformLocation(shader->getID(), "dirLight.diffuse"), dirLight.diffuse[0], dirLight.diffuse[1], dirLight.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader->getID(), "dirLight.specular"), dirLight.specular[0], dirLight.specular[1], dirLight.specular[2]);

	//set camera position
	GLint viewPosLoc = glGetUniformLocation(shader->getID(), "viewPos");
	glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

	//spotlight
	glUniform3f(glGetUniformLocation(shader->getID(), "light.ambient"), light.ambient[0], light.ambient[1], light.ambient[2]);
	glUniform3f(glGetUniformLocation(shader->getID(), "light.diffuse"), light.diffuse[0], light.diffuse[1], light.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader->getID(), "light.specular"), light.specular[0], light.specular[1], light.specular[2]);
	glUniform1f(glGetUniformLocation(shader->getID(), "light.constant"), light.constant);
	glUniform1f(glGetUniformLocation(shader->getID(), "light.linear"), light.linear);
	glUniform1f(glGetUniformLocation(shader->getID(), "light.quadratic"), light.quadratic);
	glUniform3f(glGetUniformLocation(shader->getID(), "light.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
	glUniform3f(glGetUniformLocation(shader->getID(), "light.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
	glUniform1f(glGetUniformLocation(shader->getID(), "light.cutOff"), light.cutOff);
	glUniform1f(glGetUniformLocation(shader->getID(), "light.outerCutOff"), light.outerCutOff);

	furShader->Use();

	glUniform3f(glGetUniformLocation(furShader->getID(), "dirLight.direction"), dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]);
	glUniform3f(glGetUniformLocation(furShader->getID(), "dirLight.ambient"), dirLight.ambient[0], dirLight.ambient[1], dirLight.ambient[2]);
	glUniform3f(glGetUniformLocation(furShader->getID(), "dirLight.diffuse"), dirLight.diffuse[0], dirLight.diffuse[1], dirLight.diffuse[2]);
	glUniform3f(glGetUniformLocation(furShader->getID(), "dirLight.specular"), dirLight.specular[0], dirLight.specular[1], dirLight.specular[2]);

	glUniform1i(glGetUniformLocation(furShader->getID(), "fogSelector"), fogSelector);
	glUniform1i(glGetUniformLocation(furShader->getID(), "depthFog"), depthFog);

	shader->Use();

	//spotlight
	glUniform3f(glGetUniformLocation(shader->getID(), "pointLight.ambient"), pointLight.ambient[0], pointLight.ambient[1], pointLight.ambient[2]);
	glUniform3f(glGetUniformLocation(shader->getID(), "pointLight.diffuse"), pointLight.diffuse[0], pointLight.diffuse[1], pointLight.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader->getID(), "pointLight.specular"), pointLight.specular[0], pointLight.specular[1], pointLight.specular[2]);
	glUniform1f(glGetUniformLocation(shader->getID(), "pointLight.constant"), pointLight.constant);
	glUniform1f(glGetUniformLocation(shader->getID(), "pointLight.linear"), pointLight.linear);
	glUniform1f(glGetUniformLocation(shader->getID(), "pointLight.quadratic"), pointLight.quadratic);
	glUniform3f(glGetUniformLocation(shader->getID(), "pointLight.position"), pointLight.position[0], pointLight.position[1], pointLight.position[2]);

	glUniform1i(glGetUniformLocation(shader->getID(), "normalMapped"), normalMapped);
}

int initglfw(int windowWidth, int windowHeight, const char* message)
{

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(windowWidth, windowHeight, message, NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size:
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);


	// Set the required callback functions

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
}

int initFrameBuffer()
{
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	frameBuffer = 0;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// Depth texture
	//depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	// No color output in the bound framebuffer, only depth.
	glDrawBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
}

void setContext(bool attach, int windowWidth, int windowHeight)
{
	if (attach == true)
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, windowWidth, windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

						 // Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLuint LoadTexture(const char *path)
{
	//Generate texture ID and load texture data
	string filename = string(path);

	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;

	unsigned char *image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureID;
}

void initObjects()
{
	// Initalise objects
	floorModel = new GameObject("assets/Ground/ground.obj", glm::vec3(0.0f, 0.0f, 0.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	floorModel->Scale(glm::vec3(1.2f, 1.0f, 1.2f));
	gameObjects.push_back(floorModel);

	benchModel = new GameObject("assets/Bench/bench_v01.obj", glm::vec3(-5.0f, 0.0f, 0.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	benchModel->Scale(glm::vec3(0.02f, 0.02f, 0.02f));
	gameObjects.push_back(benchModel);

	lowWall1 = new GameObject("assets/SmallWall/smallwall.obj", glm::vec3(-9.0f, 0.0f, 5.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	lowWall1->Rotate(90, glm::vec3(0, 1, 0));
	lowWall1->addCollision(glm::vec3(4.0f, 4.0f, 0.5f));
	gameObjects.push_back(lowWall1);

	lowWall2 = new GameObject("assets/SmallWall/smallwall.obj", glm::vec3(9.0f, 0.0f, 5.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	lowWall2->Rotate(90, glm::vec3(0, 1, 0));
	lowWall2->addCollision(glm::vec3(4.0f, 4.0f, 0.5f));
	gameObjects.push_back(lowWall2);

	lowWallEnd1 = new GameObject("assets/SmallWallEnd/smallwallend.obj", glm::vec3(5.0f, 0.0f, 5.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	lowWallEnd1->addCollision(glm::vec3(0.5f, 4.0f, 0.5f));
	gameObjects.push_back(lowWallEnd1);

	lowWallEnd2 = new GameObject("assets/SmallWallEnd/smallwallend.obj", glm::vec3(-5.0f, 0.0f, 5.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	lowWallEnd2->addCollision(glm::vec3(0.5f, 4.0f, 0.5f));
	gameObjects.push_back(lowWallEnd2);

	// Large Wall & Gate
	gate = new GameObject("assets/FrontGate/frontgate.obj", glm::vec3(0.0f, 0.0f, -8.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	gate->Rotate(90, glm::vec3(0, 1, 0));
	gameObjects.push_back(gate);

	lWall1 = new GameObject("assets/LargeWall/largewall.obj", glm::vec3(-11.0f, 0.0f, -8.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	lWall1->Rotate(90, glm::vec3(0, 1, 0));
	lWall1->addCollision(glm::vec3(7.0f, 4.0f, 1.0f));
	gameObjects.push_back(lWall1);

	lWall2 = new GameObject("assets/LargeWall/largewall.obj", glm::vec3(10.0f, 0.0f, -8.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	lWall2->Rotate(90, glm::vec3(0, 1, 0));
	lWall2->addCollision(glm::vec3(6.0f, 4.0f, 1.0f));
	gameObjects.push_back(lWall2);

	gateDoor = new GameObject("assets/Gate/gate.obj", glm::vec3(0.0f, 0.0f, -8.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	gateDoor->Rotate(90, glm::vec3(0, 1, 0));
	gateDoor->addCollision(glm::vec3(4.0f, 1.0f, 0.2f));
	gameObjects.push_back(gateDoor);

	// Pillar
	pillar = new GameObject("assets/Pillar/pillar.obj", glm::vec3(6.0, 0.0, 0.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	pillar->Rotate(180, glm::vec3(0, 1, 0));
	pillar->addCollision(glm::vec3(1.0f, 0.5f, 1.0f));
	gameObjects.push_back(pillar);

	button = new GameObject("assets/Button/button.obj", glm::vec3(6.0, 0.0, 0.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	button->Rotate(180, glm::vec3(0, 1, 0));
	button->addCollision(glm::vec3(1.5f, 1.5f, 1.5f));

	werewolf = new GameObject("assets/Werewolf/werewolf.obj", glm::vec3(10.0f, 0.0f, 0.0f), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	werewolf->Scale(glm::vec3(0.05f, 0.05f, 0.05f));
	werewolf->Rotate(-90, glm::vec3(0, 1, 0));
	gameObjects.push_back(werewolf);

	animModel = new GameObject("assets/Sassy/sassy.dae", glm::vec3(0.0f, 0.0f, -10.0f), animShader, shadowShader);
	animModel->Rotate(-90, glm::vec3(1.0, 0.0, 0.0));
	animModel->Scale(glm::vec3(0.3f, 0.3f, 0.3f));
	gameObjects.push_back(animModel);

	player = new GameObject("assets/Player/player.obj", camera.GetPosition(), WINDOW_WIDTH, WINDOW_HEIGHT, shader, shadowShader);
	player->addCollision(glm::vec3(1.0f, 4.0f, 1.0f));
}

void init() {
	// Initalise skybox
	SkyBox::Instance()->init();

	//shader for handling animated models with bones
	animShader = new Shader("AnimFog.vert", "AnimFog.frag");

	// generate the shader for the shadow shader
	shadowShader = new Shader("DepthRTT.vert", "DepthRTT.frag");

	initFrameBuffer();

	// generate the shader for the actual rendering
	shader = new Shader("fogMap.vert", "fogMap.frag");

	// set up the fur shader
	furShader = new Shader("furShader.vert", "furShader.frag");

	initObjects();

	// Initalise textures
	furMap = furProcessor.createFurTextures(383832, 128, 20, furDensity, "furPattern.png");
	furTexture = LoadTexture("assets/leopard.jpg");

}

// Bruteforce Collisions
// Do all collision checks surrounding the players, this prevents any unnesscary checks between static objects
void bfCollisions() {
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->hasCollision()) {
			if (player->checkCollision(gameObjects[i])) {
				camera.SetPosition(prevPlayerPos);
			}
		}
	}
}

void update(GLfloat deltaTime) {
	glm::mat4 pv = ProjectionMatrix * glm::mat4(glm::mat3(camera.GetViewMatrix()));
	SkyBox::Instance()->update(pv);

	// Store previous position
	prevPlayerPos = camera.GetPosition();

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	if (keys[GLFW_KEY_E])
	{
		if (player->checkCollision(button)) {
			button->Move(glm::vec3(-0.005, -0.005, 0.0));
			cout << "button pressed" << endl;
			buttonPressed = true;
		}
	}

	if (keys[GLFW_KEY_Z]) {
		light.cutOff = glm::cos(glm::radians(0.0f));
		light.outerCutOff = glm::cos(glm::radians(0.0f));
	}
	if (keys[GLFW_KEY_X]) {
		light.cutOff = glm::cos(glm::radians(12.5f));
		light.outerCutOff = glm::cos(glm::radians(17.5f));
	}

	if (keys[GLFW_KEY_R]) normalMapped = true;
	if (keys[GLFW_KEY_F]) normalMapped = false;

	if (keys[GLFW_KEY_Y]) fogSelector = 0;
	if (keys[GLFW_KEY_U]) fogSelector = 1;
	if (keys[GLFW_KEY_I]) fogSelector = 2; //best one

	if (keys[GLFW_KEY_K]) depthFog = 0;
	if (keys[GLFW_KEY_L]) depthFog = 1;  // best one

	// Move to Player model with the player, offset model
	player->Move(camera.GetPosition() - player->getPosition() - glm::vec3(0.0, 2.5f, 0.0));

	if (buttonPressed) {
		if (gateDoor->getPosition().y <= 5) {
			cout << gateDoor->getPosition().y << endl;
			gateDoor->Move(glm::vec3(0.0f, 0.05f, 0.0f));
		}
	}
}

void draw() {
	glEnable(GL_DEPTH_TEST);
	setContext(true, 1024, 1024);

	for (int i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->DrawShadow(glm::vec3(dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]));
	}
	player->DrawShadow(glm::vec3(dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]));
	button->DrawShadow(glm::vec3(dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]));
	setContext(false, WINDOW_WIDTH, WINDOW_HEIGHT);

	//set the shadow to the highest possible texture (to avoid overwriting by the model class)
	setMaterial();
	setAnimLights();
	setLights(shader);
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(glGetUniformLocation(shader->getID(), "shadowMap"), 31);
	

	
	for (int i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->Draw(camera.GetViewMatrix(), ProjectionMatrix);
	}	
	glCullFace(GL_FRONT_AND_BACK);
	player->Draw(camera.GetViewMatrix(), ProjectionMatrix);
	glCullFace(GL_BACK);
	button->Draw(camera.GetViewMatrix(), ProjectionMatrix);

	glUniform3f(glGetUniformLocation(shader->getID(), "LightInvDirection_worldspace"), dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]);
	glUniform3f(glGetUniformLocation(furShader->getID(), "LightInvDirection_worldspace"), dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]);
	//set fog values based on input
	glUniform1i(glGetUniformLocation(shader->getID(), "fogSelector"), fogSelector);
	glUniform1i(glGetUniformLocation(shader->getID(), "depthFog"), depthFog);

	float num = 1;

	// Assign textures
	furShader->Use();

	// Pass through the total amount of layers
	glUniform1f(glGetUniformLocation(furShader->getID(), "layers"), (float)layers);
	// Pass through fur length 
	glUniform1f(glGetUniformLocation(furShader->getID(), "furLength"), furLength);

	for (int i = 0; i < layers; i++) {
		// Pass through currentLayer
		glUniform1f(glGetUniformLocation(furShader->getID(), "currentLayer"), (float)i);

		// Determine the alpha and pass it through via UVScale
		num = num - (1 / (float)layers);
		if (num > 1) num = 1;
		if (num < 0) num = 0;
		glUniform1f(glGetUniformLocation(furShader->getID(), "UVScale"), num);

		// Passthrough fur movement.
		if (furFlowOffset > 0.01) {
			increment = false;
		}
		else if (furFlowOffset < -0.01) {
			increment = true;
		}
		if (increment) furFlowOffset += 0.000001;
		else furFlowOffset -= 0.000001;
		glUniform1f(glGetUniformLocation(furShader->getID(), "furFlowOffset"), furFlowOffset * ((float)i / (float)layers));
		werewolf->DrawFurry(*furShader, furTexture, furMap);
	}

	SkyBox::Instance()->draw();

	glDepthMask(GL_TRUE);
	glfwSwapBuffers(window);
}

int main() {
	initglfw(WINDOW_WIDTH, WINDOW_HEIGHT, "AGP Group Project");
	// Required on Windows *only* init GLEW to access OpenGL beyond 1.1
	glewExperimental = GL_TRUE;
	cout << glGetString(GL_VERSION) << endl;
	cout << "Game Start" << endl;
	bool running = true;
	init();
	while(running) {
		// Get and pass delta time
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			running = false;
		}
		GLfloat currentFrame = glfwGetTime();
		GLfloat deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();
		update(deltaTime);
		bfCollisions(); // Check Collisions
		draw();
	}

	glfwPollEvents();
	return 0;
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}