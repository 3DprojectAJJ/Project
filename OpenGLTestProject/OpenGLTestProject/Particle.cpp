#include "Particle.h"

ParticleEmitter::ParticleEmitter(glm::vec3 position, glm::vec3 initalVelocity, float gravity, float lifeSpan, glm::vec3 color)
{
	emitter.position = position;
	emitter.velocity = initalVelocity;
	emitter.gravity = gravity;
	emitter.lifeSpan = lifeSpan;
	emitter.color = color;

	for (int i = 0; i < PARTICLE_NUM; i++) {
		Particle temp = emitter;
		temp.lifeTime = i / PARTICLE_NUM;
		particles.push_back(temp);
	}

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexBuffer);

	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// Give our vertices to OpenGL.

	glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(glm::vec3), &particles[0].position, GL_STATIC_DRAW);
}

void ParticleEmitter::update(GLuint program, glm::vec3 camPos, float dt)
{
	for (int i = 0; i < particles.size(); i++) {
		particles.at(i).update(dt, emitter);
	}

	glUniform3f(glGetUniformLocation(program, "cameraPos"), camPos.x, camPos.y, camPos.z);

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

	// Draw the triangles !
	glDrawArrays(
		GL_POINTS,      // mode
		particles.size(),    // count
		GL_UNSIGNED_INT   // type
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
