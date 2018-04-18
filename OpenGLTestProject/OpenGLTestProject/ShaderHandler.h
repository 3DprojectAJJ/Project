#ifndef SHADERHANDLER_H
#define SHADERHANDLER_H
#include <GL\glew.h>
#include <vector>
#include <fstream>
//#include <debugapi.h>
class ShaderHandler
{
private:
	std::vector<GLuint> programIDs;

	struct shader
	{
		GLenum type;
		GLuint id;
	};

	std::vector<shader> shaders;
public:
	ShaderHandler();
	~ShaderHandler();
	bool addShader(const char * path, GLenum type);
	bool createProgram();
	GLuint getProgramID(int index);
};

#endif