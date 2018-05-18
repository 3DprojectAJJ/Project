#include "Particle.h"

void ParticleEmitter::render(GLuint program, Camera *cam)
{
	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "forward"), cam->getForward().x, cam->getForward().y, cam->getForward().z);
	glUniform1f(glGetUniformLocation(program, "particleSize"), 0.1f);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &world[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, billboardVertexBuffer);

	GLint quadPos = glGetAttribLocation(program, "corner");

	if (quadPos == -1)
	{
		OutputDebugStringA("Error, cannot find 'corner' attribute in Particle Vertex shader\n");
		return;
	}

	glEnableVertexAttribArray(quadPos);

	glVertexAttribPointer(
		quadPos,            // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 1rst attribute buffer : vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	GLint vertexPos = glGetAttribLocation(program, "vertexPosition");

	if (vertexPos == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexPosition' attribute in Particle Vertex shader\n");
		return;
	}

	glEnableVertexAttribArray(vertexPos);

	glVertexAttribPointer(
		vertexPos,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);



	
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);

	GLint color = glGetAttribLocation(program, "inColor");

	if (color == -1)
	{
		OutputDebugStringA("Error, cannot find 'inColor' attribute in Particle Fragment shader\n");
		return;
	}

	glEnableVertexAttribArray(color);

	glVertexAttribPointer(
		color,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glVertexAttribDivisor(vertexPos, 1);
	glVertexAttribDivisor(color, 1);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particles.size());

	glDisableVertexAttribArray(quadPos);
	glDisableVertexAttribArray(vertexPos);
	glDisableVertexAttribArray(color);
}

void ParticleEmitter::emittParticle()
{
	Particle temp = origin;
	float x = (float)(rand()) / (float)RAND_MAX;
	float z = (float)(rand()) / (float)RAND_MAX;
	temp.velocity = glm::vec3((x - 0.5f) / 2, origin.velocity.y, (z - 0.5f) / 2);
	x = (float)(rand()) / (float)RAND_MAX;
	z = (float)(rand()) / (float)RAND_MAX;
	temp.position = glm::vec3((x - 0.5f) * 0.66, origin.position.y, (z - 0.5f) * 0.66);
	if (particles.size() < PARTICLE_NUM) {
		particles.insert(particles.begin() + lastParticle, temp);
		lastParticle++;
	}
	else {
		particles[lastParticle] = temp;
		lastParticle++;
	}
	if(lastParticle == PARTICLE_NUM){
		lastParticle = 0;
	}
}

void ParticleEmitter::interpolateColor(int index)
{
	particles[index].color.x = (particles[index].lifeTime - origin.lifeTime) * origin.color.x + (origin.lifeTime - particles[index].lifeTime) * endColor.x;
	particles[index].color.y = (particles[index].lifeTime - origin.lifeTime) * origin.color.y + (origin.lifeTime - particles[index].lifeTime) * endColor.y;
	particles[index].color.z = (particles[index].lifeTime - origin.lifeTime) * origin.color.z + (origin.lifeTime - particles[index].lifeTime) * endColor.z;
	particles[index].color = glm::clamp(particles[index].color, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}

void ParticleEmitter::init(glm::vec3 position, glm::vec3 initalVelocity, float lifeSpan)
{	
	origin.position = position;
	origin.velocity = initalVelocity;
	origin.lifeTime = lifeSpan;

	world = glm::mat4(1.0f);
	lastParticle = 0;

	std::vector<float> vertexData = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};

	glGenBuffers(1, &billboardVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboardVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
}

ParticleEmitter::ParticleEmitter(glm::vec3 position, glm::vec3 initalVelocity, float lifeSpan, glm::vec3 color)
{
	origin.color = color;
	endColor = glm::vec3(-1.0f);
	init(position, initalVelocity, lifeSpan);
}

ParticleEmitter::ParticleEmitter(glm::vec3 position, glm::vec3 initalVelocity, float lifeSpan, glm::vec3 beginColor, glm::vec3 endColor)
{
	origin.color = beginColor;
	this->endColor = endColor;
	init(position, initalVelocity, lifeSpan);
}

void ParticleEmitter::update(GLuint program, Camera *cam, float dt)
{
	particleTimer += dt;
	if (particleTimer >= 1.0f/(float)PARTICLE_PER_SECOND) {
		while (particleTimer > 0) {
			particleTimer -= 1.0f/(float)PARTICLE_PER_SECOND;
			emittParticle();
		}
		particleTimer = 0;
	}

	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].lifeTime > 0) {
			particles[i].position += particles[i].velocity * dt;
			particles[i].lifeTime -= dt;
			if (endColor != glm::vec3(-1, -1, -1)) {
				interpolateColor(i);
			}
		}
		else {
			particles[i].position = cam->getPos() - cam->getForward();
		}

		posdata[i * 3] = particles[i].position.x;
		posdata[i * 3 + 1] = particles[i].position.y;
		posdata[i * 3 + 2] = particles[i].position.z;

		colorData[i * 4] = particles[i].color.x;
		colorData[i * 4 + 1] = particles[i].color.y;
		colorData[i * 4 + 2] = particles[i].color.z;
		colorData[i * 4 + 3] = 1.0f;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(GLfloat) * 3, posdata);
	
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(GLfloat) * 4, colorData);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	render(program, cam);
}
