#include "Entity.h"

void Entity::setWorld()
{
	world = glm::translate(position);
	if (rotation.x != 0) {
		world *= glm::rotate(rotation.x, glm::vec3(1, 0, 0));
	}
	if (rotation.y != 0) {
		world *= glm::rotate(rotation.y, glm::vec3(0, 1, 0));
	}
	if (rotation.z != 0) {
		world *= glm::rotate(rotation.z, glm::vec3(0, 0, 1));
	}
	world *= glm::scale(scale);
}

Entity::Entity()
{
	scale = glm::vec3(1.0f);
	rotation = glm::vec3(1.0f);
	position = glm::vec3(1.0f);
}

glm::vec3 Entity::getPosition() const
{
	return position;
}

glm::vec3 Entity::getRotation() const
{
	return glm::degrees(rotation);
}

glm::vec3 Entity::getScale() const
{
	return scale;
}

void Entity::setPosition(glm::vec3 position)
{
	this->position = position;
	setWorld();
}

void Entity::setRotation(glm::vec3 rotation)
{
	this->rotation = glm::radians(rotation);
	setWorld();
}

void Entity::setScale(glm::vec3 scale)
{
	this->scale = scale;
	setWorld();
}
