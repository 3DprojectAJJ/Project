#include "Particle.h"

void ParticleEmitter::render(GLuint program, glm::vec3 camPos)
{
	glUniform1f(glGetUniformLocation(program, "particleSize"), 0.1f);

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &world[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		10,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, billboardVertexBuffer);
	glVertexAttribPointer(
		11,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(
		12,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particles.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void ParticleEmitter::emittParticle()
{
	if (lastParticle < PARTICLE_NUM) {
		Particle temp = origin;
		particles.insert(particles.begin() + lastParticle, temp);
		lastParticle++;
	}
}

ParticleEmitter::ParticleEmitter(glm::vec3 position, glm::vec3 initalVelocity, float lifeSpan, glm::vec3 color)
{
	origin.position = position;
	origin.velocity = initalVelocity;
	origin.lifeTime = lifeSpan;
	origin.color = color;

	world = glm::mat4(1.0f);
	lastParticle = 0;

	static const float vertexData[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};

	glGenBuffers(1, &billboardVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboardVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

void ParticleEmitter::update(GLuint program, glm::vec3 camPos, float dt)
{
	particleTimer += dt;
	if (particleTimer >= 1 / PARTICLE_PER_SECOND) {
		emittParticle();
		particleTimer = 0;
	}

	for (int i = 0; i < particles.size(); i++) {
		particles[i].position += particles[i].velocity * dt;
		particles[i].lifeTime -= dt;

		posData[i * 3] = particles[i].position.x;
		posData[(i * 3) + 1] = particles[i].position.y;
		posData[(i * 3) + 2] = particles[i].position.z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(GLfloat) * 3, posData);

	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(GLubyte) * 4, &particles[0].color);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	render(program, camPos);
}
