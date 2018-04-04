#ifndef MESH_H
#define MESH_H

#include <vector>
#include <GL\glew.h>
#include <GLM\common.hpp>
#include <Windows.h>
#include <GLM\mat4x4.hpp>
#pragma warning(disable:4996)

class Mesh
{
public:
	struct vertexInfo
	{
		glm::vec3 pos;
		glm::vec2 uv;
		glm::vec3 normal;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec4 specular;
		GLuint texID;
	};
	struct material
	{
		char name[128];
		unsigned int Ns;
		float Ni;
		float d;
		glm::ivec3 Tf;
		unsigned int illum;
		glm::vec3 Ka;
		glm::vec3 Kd;
		glm::vec3 Ks;
		char map_Kd[128];
		char map_Ka[128];
		char map_Ks[128];
		GLuint kdTex;
	};
	struct index
	{
		unsigned int pos;
		unsigned int uv;
		unsigned int normal;
		unsigned int mat;
	};
private:
	std::vector<index> indices;
	std::vector<material> materials;
	std::vector<vertexInfo> vertices;
	std::vector<GLuint> endOfMat;
	std::vector<GLuint> startOfMat;
	glm::mat4 world;
	GLuint vao;
	GLuint vbo;

	bool readMTLFile(const char * path);
	GLuint loadImage(const char * imagepath);
public:
	Mesh(glm::mat4 worldMat = glm::mat4(1.0f));
	~Mesh();
	void makeBuffer(GLuint program);
	bool readOBJFile(const char * path);
	void draw(GLuint program);
	std::vector<vertexInfo> getVertices() const;
	std::vector<material> getMaterials() const;
	std::vector<index> getIndices() const;
	glm::mat4 getMatrix() const;
	void setMatrix(glm::mat4 world);
	std::vector<GLuint> getStartOfMaterial() const;
	std::vector<GLuint> getEndOfMaterial() const;
};
#endif