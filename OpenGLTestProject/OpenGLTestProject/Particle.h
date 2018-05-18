#ifndef PARTICLE_H
#define PARTICLE_H
#include <GL\glew.h>
#include <GLM\common.hpp>
#include <vector>
#include <GLM\mat4x4.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM\gtx\transform.hpp>
#include <GLM\gtc\type_ptr.hpp>

#define PARTICLE_NUM 500
#define PARTICLE_PER_SECOND 2

class ParticleEmitter {
private:
	struct Particle {
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 color;
		float lifeTime;
	};

	float size;
	unsigned int nrOfParticle;
	unsigned int lastParticle;

	float particleTimer;

	Particle origin;

	GLuint vertexArrayID, vertexBuffer;
	GLuint billboardVertexBuffer;
	GLuint colorBuffer;

	glm::mat4 world;
	std::vector<Particle> particles;
	float posData[1500];
	
	void render(GLuint program, glm::vec3 camPos);
	void emittParticle();
public:
	ParticleEmitter(glm::vec3 position, glm::vec3 initalVelocity, float lifeSpan, glm::vec3 color);

	void update(GLuint program, glm::vec3 camPos, float dt);

};

#endif