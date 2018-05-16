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

class ParticleEmitter {
private:
	struct Particle {
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 color;
		float gravity;
		float lifeSpan;
		float lifeTime = 0;
		void update(float dt, Particle reset) {
			if (lifeTime >= 0) {
				position += velocity * dt;
				velocity.y += gravity * dt;
			}
			lifeTime += dt;
			if (lifeTime >= lifeSpan) {
				lifeTime = 0;
				position = reset.position;
				velocity = reset.velocity;
			}
		}
	};

	float size;

	Particle emitter;

	GLuint vertexArrayID, vertexBuffer;

	glm::mat4 world = glm::mat4(1.0f);
	std::vector<Particle> particles;
public:
	ParticleEmitter(glm::vec3 position, glm::vec3 initalVelocity, float gravity, float lifeSpan, glm::vec3 color);

	void update(GLuint program, glm::vec3 camPos, float dt);

};

#endif