#include "Camera.h"

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(this->position, this->position + this->front, this->up);
}

glm::vec3 Camera::getFrontDirection()
{
	return frontDirection;
}

GLfloat Camera::getPitch()
{
	return pitch;
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
	GLfloat velocity = this->movementSpeed * deltaTime;

	if (direction == FORWARD)
	{
		float ypos = this->position.y;
		this->position += front * velocity;
		this->position = glm::vec3(this->position.x, ypos, this->position.z);
	}

	if (direction == BACKWARD)
	{
		float ypos = this->position.y;
		this->position -= front * velocity;
		this->position = glm::vec3(this->position.x, ypos, this->position.z);
	}

	if (direction == LEFT)
	{
		this->position -= right * velocity;
	}

	if (direction == RIGHT)
	{
		this->position += right * velocity;
	}
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch)
{

	xOffset *= this->mouseSensitivity;
	yOffset *= this->mouseSensitivity;

	this->yaw += xOffset;
	this->pitch += yOffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (this->pitch > 89.0f)
		{
			this->pitch = 89.0f;
		}

		if (this->pitch < -89.0f)
		{
			this->pitch = -89.0f;
		}
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	this->updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(GLfloat yOffset)
{
}

GLfloat Camera::GetZoom()
{
	return this->zoom;
}

glm::vec3 Camera::GetPosition()
{
	return this->position;
}

void Camera::SetPosition(glm::vec3 newPos)
{
	this->position = newPos;
}

glm::vec3 Camera::GetFront()
{
	return this->front;
}

GLfloat Camera::GetYaw()
{
	return this->yaw;
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors()
{
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->right = glm::normalize(glm::cross(this->front, this->worldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}
}
