#pragma once
#include "glm/glm.hpp"

class CollisionBox {
private:
	glm::vec3 m_collisionSize;
public:
	CollisionBox(glm::vec3 collisionBox) : m_collisionSize(collisionBox) {
		
	}

	glm::vec3 getCollisionBox();
	void moveCollision(glm::vec3 moveAmount);
};