#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <GL\glew.h>
#include <GLM\glm.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <vector>
#include <string>

#define NUM_OF_TEXTURES 7

class Framebuffer
{
private:
	struct Light {
		glm::vec3 pos;
		glm::vec4 color;
	};

	std::vector<Light> lights;

	GLuint id;
	GLuint textures[NUM_OF_TEXTURES];
	GLint colorLoc, normalLoc, posLoc, depthLoc;
	GLint ambientLoc, diffuseLoc, specularLoc;
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
	void init();
	void shadowInit();
	void bindFBO();
	void bindShadowFBO();
	void unbindFBO(int width, int height);
	void getUniform(GLuint program);
	GLuint getCubeMap();
	void setCubemap(GLuint CubeMap);
	unsigned int nrOfTextures();

	GLuint * getTexID();
	GLuint getQuadID() const;

	void addLight(glm::vec3 pos, glm::vec4 color);
	glm::vec3 getLightPos(int index);

	void draw(GLuint program, glm::vec3 camPos);
};

#endif
