#ifndef ENTITY_H
#define ENTITY_H
#include <GL\glew.h>
#include <GLM\common.hpp>
#include <Windows.h>
#include <GLM\mat4x4.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM\gtx\transform.hpp>

class Entity {
private:

protected:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 world;

	void setWorld();
public:
	Entity();

	virtual void draw(GLuint program) = 0;
	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;
	glm::vec3 getScale() const;
	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);
};

#endif // !ENTITY_H
