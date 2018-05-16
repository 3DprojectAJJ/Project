#ifndef WATER_H
#define WATER_H

#include "Mesh.h"

class Water {
private:
	glm::vec3 position;
	glm::vec2 size;
	int waveIntesity;

public:
	Water(glm::vec3 position, glm::vec2 size, int waveIntensity);

	void update(double dt);
};

#endif 0