#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Shader.h"
#include "Model.h"
#include "Mesh.h"
#include "AnimModel.h"
#include "AnimMesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CollisionBox.h"
class GameObject {
private:
	Model* m_objectModel;
	AnimModel* m_animModel;
	GLboolean m_isAnimModel;

	Shader *m_shadowShader;
	Shader *m_Shader;
	glm::mat4 m_orientation;
	glm::mat4 m_depthMVP;
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;

	GLuint m_framebuffer;
	GLuint m_depthTexture;
	GLuint m_screenWidth, m_screenHeight;
	glm::vec3 m_dirLightPos;
	glm::mat4 biasMatrix{
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	};

	bool m_hasCollision = false;
	CollisionBox *m_objCollision;


	// Framebuffers controls for Shadowmapping
	void setContext(bool attach, int windowWidth, int windowHeight);

public:
	GameObject(const char* filename, glm::vec3 initalPosition, GLuint screenWidth, GLuint screenHeight, Shader* fogShader, Shader* shadowShader) :
		m_screenWidth(screenWidth), m_screenHeight(screenHeight), m_Shader(fogShader), m_shadowShader(shadowShader) {
		m_objectModel = new Model(filename);
		m_orientation = glm::mat4(1.0);
		m_orientation = glm::translate(m_orientation, initalPosition);
		m_isAnimModel = false;
	}
	GameObject(const char* filename, glm::vec3 initalPosition,  Shader* animShader, Shader* shadowShader) : m_Shader(animShader), m_shadowShader(shadowShader) {
		m_animModel = new AnimModel();
		m_animModel->loadModel(filename);
		m_animModel->initShaders(animShader->getID());
		m_orientation = glm::mat4(1.0);
		m_orientation = glm::translate(m_orientation, initalPosition);
		m_isAnimModel = true;
	}
	~GameObject() {
		delete m_objectModel;
	}

	// Translations
	void Move(glm::vec3 movePosition);
	void Rotate(GLfloat rotateDegress, glm::vec3 axis);
	void Scale(glm::vec3 scaleAmount);

	glm::vec3 getPosition();

	void addCollision(glm::vec3 size);
	glm::vec3 getCollision();
	GLboolean hasCollision();
	GLboolean checkCollision(GameObject* otherObject);

	// Draw Methods
	void DrawShadow(glm::vec3 dirLight);
	void Draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
	void DrawFurry(Shader furShader, GLuint tex1, GLuint tex2);
};