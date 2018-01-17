#ifndef PLAYER_H
#define PLAYER_H

#include <GLFW\glfw3.h>
#include <glm/glm.hpp>

class player {
private:
	glm::vec3 position;

public:
	void controlls();

	glm::vec3 getPosition();
};

#endif // !PLAYER_H
