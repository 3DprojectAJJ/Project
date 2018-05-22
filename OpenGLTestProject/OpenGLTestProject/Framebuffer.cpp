#include "Framebuffer.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Framebuffer::Framebuffer(const int width, const int height)
{
	this->width = width;
	this->height = height;
}

Framebuffer::~Framebuffer()
{
}

void Framebuffer::init()
{
	//generate and bind fbo
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	//generate and bind textures
	glGenTextures(NUM_OF_TEXTURES, textures);

	for (unsigned int i = 0; i < NUM_OF_TEXTURES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (i == NUM_OF_TEXTURES - 1)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + NUM_OF_TEXTURES - 1, GL_TEXTURE_2D, textures[i], 0);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
		}
	}
	// generate and bind the renderbuffers for tex and depth textures 
	glGenRenderbuffers(1, &texID);
	glBindRenderbuffer(GL_RENDERBUFFER, texID);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, NUM_OF_TEXTURES, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_TEXTURE_2D, GL_RENDERBUFFER, texID);

	glGenRenderbuffers(1, &depthID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthID);

	GLenum drawbuffers[NUM_OF_TEXTURES];

	for (unsigned int i = 0; i < NUM_OF_TEXTURES; i++)
	{
		drawbuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	glDrawBuffers(NUM_OF_TEXTURES, drawbuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	//The quad that will be used for second renderpass
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	static const GLfloat gQuadVertexBufferData[] = {
		- 1.0f, -1.0f, 0.0f, 0.0, 1.0,
		  1.0f, -1.0f, 0.0f, 1.0, 1.0,
		- 1.0f,  1.0f, 0.0f, 0.0, 0.0,
		- 1.0f,  1.0f, 0.0f, 0.0, 0.0,
		  1.0f, -1.0f, 0.0f, 1.0, 1.0,
		  1.0f,  1.0f, 0.0f, 1.0, 0.0
	};
	glGenBuffers(1, &quadID);
	glBindBuffer(GL_ARRAY_BUFFER, quadID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gQuadVertexBufferData), gQuadVertexBufferData, GL_STATIC_DRAW);
}

void Framebuffer::shadowInit()
{
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthCubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glViewport(0, 0, width, height);
}

void Framebuffer::bindShadowFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glViewport(0, 0, shadowWidth, shadowHeight);
}

void Framebuffer::unbindFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}

void Framebuffer::getUniform(GLuint program)
{
	colorLoc = glGetUniformLocation(program, "colorTexture");
	normalLoc = glGetUniformLocation(program, "normalTexture");
	posLoc = glGetUniformLocation(program, "positionTexture");
	depthLoc = glGetUniformLocation(program, "depthTexture");
}

unsigned int Framebuffer::nrOfTextures()
{
	return NUM_OF_TEXTURES;
}

GLuint * Framebuffer::getTexID()
{
	return textures;
}

GLuint Framebuffer::getQuadID() const
{
	return quadID;
}

void Framebuffer::addLight(glm::vec3 pos, glm::vec4 color)
{
	Light temp;
	temp.pos = pos;
	temp.color = color;

	lights.push_back(temp);

}

glm::vec3 Framebuffer::getLightPos(int index)
{
	return lights[index].pos;
}

void Framebuffer::draw(GLuint program)
{
	glUseProgram(program);

	glUniform1i(colorLoc, 0);
	glUniform1i(normalLoc, 1);
	glUniform1i(posLoc, 2);
	glUniform1i(depthLoc, 3);

	for (int i = 0; i < nrOfTextures(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, getTexID()[i]);
	}

	glm::vec3* pos = new glm::vec3[lights.size()];
	glm::vec4* color = new glm::vec4[lights.size()];

	for (int i = 0; i < lights.size(); i++) {
		pos[i] = lights[i].pos;
		color[i] = lights[i].color;
	}

	glUniform3fv(glGetUniformLocation(program, "lightPosition"), lights.size(), glm::value_ptr(pos[0]));
	glUniform4fv(glGetUniformLocation(program, "lightColor"), lights.size(), glm::value_ptr(color[0]));
	glUniform1i(glGetUniformLocation(program, "nrOfLights"), lights.size());

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quadID);

	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		5*4,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, quadID);

	glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			5 * 4,                  // stride
			(void*)(3*4)            // array buffer offset
		);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	for (int i = 0; i < NUM_OF_TEXTURES; i++)
	{
		glDisable(GL_TEXTURE0 + i);
	}
	glDisable(GL_TEXTURE_2D);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	delete[] pos;
	delete[] color;
}
