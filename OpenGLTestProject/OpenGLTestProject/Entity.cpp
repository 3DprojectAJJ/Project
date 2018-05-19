#include "Entity.h"

Entity::Entity()
{
}

glm::vec3 Entity::getPosition() const
{
	return position;
}

glm::vec3 Entity::getRotation() const
{
	return rotation;
}

void Entity::setPosition(glm::vec3 position)
{
	this->position = position;
	world = glm::translate(position);
	setRotation(rotation);
}

void Entity::setRotation(glm::vec3 rotation)
{
	glm::mat4 temp = glm::mat4(1.0f);
	this->rotation = glm::radians(rotation);
	if (rotation.x != 0) {
		temp = glm::rotate(rotation.x, glm::vec3(1, 0, 0));
	}
	if (rotation.y != 0) {
		temp *= glm::rotate(rotation.y, glm::vec3(0, 1, 0));
	}
	if (rotation.z != 0) {
		temp *= glm::rotate(rotation.z, glm::vec3(0, 0, 1));
	}
	world *= temp;
}