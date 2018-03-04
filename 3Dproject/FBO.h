#ifndef FBO_H
#define FBO_H
#include <GL\glew.h>
#define NUM_OF_TEXTURES 7
class FBO
{
private:

	GLuint fboID;
	GLuint m_textures[NUM_OF_TEXTURES];
	GLuint depthID;
	GLuint texID;
	GLuint colorID;
	GLuint quadID;
	int width;
	int height;
public:
	FBO(int width = 1024, int height = 768);
	~FBO();
	void Init();
	void BindFBO();
	void UnbindFBO(int width, int height);
	int NrOfTex();

	GLuint* GetTexID();
	GLuint GetQuadID() const;
};

#endif