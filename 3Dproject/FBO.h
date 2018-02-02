#ifndef FBO_H
#define FBO_H
#include <GL\glew.h>
class FBO
{
private:
	GLuint fboID;
	GLuint texID;
	GLuint depthID;
	GLuint quadID;
	int width;
	int height;
public:
	FBO(int width = 1024, int height = 768);
	~FBO();
	void Init();
	void BindFBO();
	void UnbindFBO(int width , int height);

	GLuint GetTexID() const;
	GLuint GetQuadID() const;
};

#endif