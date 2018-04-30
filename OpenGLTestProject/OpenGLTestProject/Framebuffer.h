#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <GL\glew.h>

#define NUM_OF_TEXTURES 3

class Framebuffer
{
private:
	GLuint id;
	GLuint textures[NUM_OF_TEXTURES];
	GLuint depthID;
	GLuint texID;
	GLuint colorID;
	GLuint quadID;
	int width;
	int height;
public:
	Framebuffer(const int width = 1024, const int height = 768);
	~Framebuffer();
	void init();
	void bindFBO();
	void unbindFBO(int width, int height);
	unsigned int nrOfTextures();

	GLuint * getTexID();
	GLuint getQuadID() const;

	void draw(GLuint program);
};

#endif