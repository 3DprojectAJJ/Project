#include "ShaderHandler.h"



ShaderHandler::ShaderHandler()
{
	// nothing
}


ShaderHandler::~ShaderHandler()
{
	// nothing
}

bool ShaderHandler::addShader(const char * path, GLenum type)
{
	// buffer for error/debug output
	char buff[1024];
	memset(buff, 0, 1024);

	shader tmp;

	const char* shaderTextPtr;
	tmp.id = glCreateShader(type);
	tmp.type = type;

	GLint compileResult = 0;

	std::ifstream file(path);
	std::string shaderText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	shaderTextPtr = shaderText.c_str();

	glShaderSource(tmp.id, 1, &shaderTextPtr, nullptr);

	glCompileShader(tmp.id);
	glGetShaderiv(tmp.id, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		glGetShaderInfoLog(tmp.id, 1024, nullptr, buff);
		//OutputDebugStringA(buff);
		return false;
	}
	else
	{
		shaders.push_back(tmp);
	}
	return true;
}

bool ShaderHandler::createProgram()
{
	bool result = true;
	GLuint programID = glCreateProgram();
	unsigned int nrOf = shaders.size();
	for (int i = 0; i < nrOf; i++)
	{
		if (shaders[i].id != -1);
		{
			glAttachShader(programID, shaders[i].id);
		}
	}
	glLinkProgram(programID);
	GLint compileResult = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		result = false;
	}
	else
	{
		for (unsigned int i = 0; i < nrOf; i++)
		{
			if (shaders[i].id != -1)
			{
				glDetachShader(programID, shaders[i].id);
				glDeleteShader(shaders[i].id);
			}
		}
		shaders.clear();
		programIDs.push_back(programID);
	}
	return result;
}

GLuint ShaderHandler::getProgramID(int index)
{
	return programIDs[index];
}
