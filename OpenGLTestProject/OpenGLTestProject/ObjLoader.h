#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <GL/glew.h>
#include <GLM/vec3.hpp>
#include <GLM/vec2.hpp>
#include <GLM/vec4.hpp>
#include <Windows.h>
#include <stdio.h>
#include <vector>

class ObjLoader
{
public:
	struct DataFormat
	{
		glm::vec3 pos;
		glm::vec2 uv;
		glm::vec3 normal;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec4 specular;
	};
private:
	struct Material
	{
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec4 specular;
	};

	std::vector<DataFormat> m_data;
	Material mat;
	GLuint texID;
	bool readMTLFile(const char * path);
	GLuint loadImage(const char * imagepath);
public:
	ObjLoader();
	~ObjLoader();

	bool readOBJFile(const char * path);
};
#endif