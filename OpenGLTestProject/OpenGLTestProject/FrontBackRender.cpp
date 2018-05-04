#include "FrontBackRender.h"

float FrontBackRender::distance(glm::vec3 p0, glm::vec3 p1)
{
	glm::vec3 vec = (p0 - p1) * (p0 - p1);
	return vec.x + vec.y + vec.z;
}

void FrontBackRender::sort(std::vector<Entity*>* entities, glm::vec3 cameraPos)
{
	for (int i = order.size(); i < entities->size(); i++) {
		order.push_back(glm::vec2(1.0f));
	}
	for (int i = 0; i < entities->size(); i++) {
		order.at(i) = glm::vec2(distance(cameraPos, entities->at(i)->getPosition()), i);
	}
	for (int i = 0; i < entities->size() - 1; i++) {
		int index = i;
		for (int j = i + 1; j < entities->size(); j++) {
			if (order[i].x > order[j].x) {
				index = j;
			}
		}
		if (index != i) {
			iter_swap(order.begin() + i, order.begin() + index);
		}
	}
}

FrontBackRender::FrontBackRender()
{
}

void FrontBackRender::render(std::vector<Entity*> *entities, glm::vec3 cameraPos, GLuint program)
{
	sort(entities, cameraPos);

	for (int i = 0; i < entities->size(); i++) {
		entities->at(order[i].y)->draw(program);
	}
}
