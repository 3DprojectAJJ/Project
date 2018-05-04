#ifndef FRONTBACKRENDER_H
#define FRONTBACKRENDER_H

#include "Entity.h"
#include <vector>
#include <algorithm>

class FrontBackRender {
private:
	std::vector<glm::vec2> order;

	float distance(glm::vec3 p0, glm::vec3 p1);
	void sort(std::vector<Entity*> *entities, glm::vec3 cameraPos);
public:
	FrontBackRender();

	void render(std::vector<Entity*> *entities, glm::vec3 cameraPos, GLuint program);
};

#endif // !FRONTBACKRENDER_H
