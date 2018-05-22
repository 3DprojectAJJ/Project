#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <GL\glew.h>
#include <GLM\glm.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <vector>
#include <string>
#include "VertexArray.h"
#include "IndexBuffer.h"

#define NUM_OF_TEXTURES 7

class Framebuffer
{
private:
	struct Light {
		glm::vec3 pos;
		glm::vec4 color;
	};

	std::vector<Light> lights;
	VertexArray vao;
	VertexBuffer vbo;
	VertexBufferLayout layout;
	IndexBuffer ib;
	GLuint vaoID;
	GLuint id;
	GLuint textures[NUM_OF_TEXTURES];
	GLint colorLoc, normalLoc, posLoc, depthLoc;
	GLuint depthID;
	GLuint texID;
	GLuint colorID;
	GLuint quadID;
	GLuint lightID;
	GLuint depthMapFBO;
	GLuint depthCubemap;
	int width;
	int height;
	const unsigned int shadowWidth = 1024;
	const unsigned int shadowHeight = 1024;
public:
	Framebuffer(const int width = 1024, const int height = 768);
	~Framebuffer();
	void init(GLuint program);
	void shadowInit();
	void bindFBO();
	void bindShadowFBO();
	void unbindFBO(int width, int height);
	void getUniform(GLuint program);
	unsigned int nrOfTextures();

	GLuint * getTexID();
	GLuint getQuadID() const;

	void addLight(glm::vec3 pos, glm::vec4 color);
	glm::vec3 getLightPos(int index);

	void draw(GLuint program);
};

#endif
