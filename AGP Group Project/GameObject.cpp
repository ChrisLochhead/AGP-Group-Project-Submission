#include "GameObject.h"

void GameObject::Move(glm::vec3 movePosition)
{
	m_orientation = glm::translate(m_orientation, movePosition);
}

void GameObject::Rotate(GLfloat rotateDegress, glm::vec3 axis)
{
	m_orientation = glm::rotate(m_orientation, glm::radians(rotateDegress), axis);
}

void GameObject::Scale(glm::vec3 scaleAmount)
{
	m_orientation = glm::scale(m_orientation, scaleAmount);
}

glm::vec3 GameObject::getPosition()
{
	return glm::vec3(m_orientation[3][0], m_orientation[3][1], m_orientation[3][2]);
}

void GameObject::addCollision(glm::vec3 size)
{
	m_objCollision = new CollisionBox(size);
	m_hasCollision = true;
}

GLboolean GameObject::checkCollision(GameObject * otherObject)
{

	if (abs(this->getPosition().x - otherObject->getPosition().x) > (m_objCollision->getCollisionBox().x + otherObject->getCollision().x)) return false;
	if (abs(this->getPosition().y - otherObject->getPosition().y) > (m_objCollision->getCollisionBox().y + otherObject->getCollision().y)) return false;
	if (abs(this->getPosition().z - otherObject->getPosition().z) > (m_objCollision->getCollisionBox().z + otherObject->getCollision().z)) return false;
	return true;
}

glm::vec3 GameObject::getCollision()
{
	if (m_objCollision != nullptr) {
		return m_objCollision->getCollisionBox();
	}
}

GLboolean GameObject::hasCollision()
{
	return m_hasCollision;
}

void GameObject::DrawShadow(glm::vec3 dirLight)
{
	// Use the shadow shader
	glUseProgram(m_shadowShader->getID());

	// Compute the MVP matrix from the light's point of view
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
	glm::mat4 depthViewMatrix = glm::lookAt(dirLight, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_depthMVP = depthProjectionMatrix * depthViewMatrix * m_orientation;


	glUniformMatrix4fv(glGetUniformLocation(m_shadowShader->getID(), "depthMVP"), 1, GL_FALSE, glm::value_ptr(m_depthMVP));
	if (!m_isAnimModel) m_objectModel->DrawVMesh(*m_shadowShader);
}

void GameObject::Draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	// Shrinks Model
	m_Shader->Use();
	ProjectionMatrix = projectionMatrix;
	ViewMatrix = viewMatrix;

	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * m_orientation; //ModelMatrix;

	glm::mat4 depthBiasMVP = biasMatrix * m_depthMVP;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(glGetUniformLocation(m_Shader->getID(), "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(glGetUniformLocation(m_Shader->getID(), "M"), 1, GL_FALSE, glm::value_ptr(m_orientation));
	glUniformMatrix4fv(glGetUniformLocation(m_Shader->getID(), "V"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(m_Shader->getID(), "P"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
	//glm::mat4 matr_normals = glm::mat4(glm::transpose(glm::inverse(m_orientation)));
	glUniformMatrix4fv(glGetUniformLocation(m_Shader->getID(), "normals_matrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::transpose(glm::inverse(m_orientation)))));
	glUniformMatrix4fv(glGetUniformLocation(m_Shader->getID(), "DepthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthBiasMVP));


	if (!m_isAnimModel) m_objectModel->DrawDMesh( *m_Shader);
	else m_animModel->draw(m_Shader->getID(), true);
}

void GameObject::DrawFurry(Shader furShader, GLuint tex1, GLuint tex2)
{
	glUniformMatrix4fv(glGetUniformLocation(furShader.getID(), "M"), 1, GL_FALSE, glm::value_ptr(m_orientation));
	glUniformMatrix4fv(glGetUniformLocation(furShader.getID(), "V"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(furShader.getID(), "P"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
	m_objectModel->DrawFurMesh(furShader, tex1, tex2);
}

void GameObject::setContext(bool attach, int windowWidth, int windowHeight)
{
	if (attach == true)
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, windowWidth, windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
