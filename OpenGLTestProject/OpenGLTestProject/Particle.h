#ifndef PARTICLE_H
#define PARTICLE_H
#include <GL\glew.h>
#include <GLM\common.hpp>
#include <vector>
#include <Windows.h>
#include "Camera.h"
#include <GLM\mat4x4.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM\gtx\transform.hpp>
#include <GLM\gtc\type_ptr.hpp>

#define PARTICLE_NUM 2500
#define PARTICLE_PER_SECOND 250

class ParticleEmitter {
private:
	struct Particle {
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 color;
		float lifeTime;
	};

	glm::vec3 endColor;

	float size;
	unsigned int lastParticle;

	float particleTimer;

	Particle origin;

	GLuint vertexArrayID, vertexBuffer;
	GLuint billboardVertexBuffer;
	GLuint colorBuffer;

	glm::mat4 world;
	std::vector<Particle> particles;

	float posdata[PARTICLE_NUM * 3];
	float colorData[PARTICLE_NUM * 4];
	
	void render(GLuint program, Camera *cam);
	void emittParticle();

	void interpolateColor(int index);

	void init(glm::vec3 position, glm::vec3 initalVelocity, float lifeSpan);
public:
	ParticleEmitter(glm::vec3 position, glm::vec3 initalVelocity, float lifeSpan, glm::vec3 color);
	ParticleEmitter(glm::vec3 position, glm::vec3 initalVelocity, float lifeSpan, glm::vec3 beginColor, glm::vec3 endColor);

	void update(GLuint program, Camera *cam, float dt);

};

#endif