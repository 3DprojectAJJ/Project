#ifndef MESH_H
#define MESH_H
#include "Entity.h"
#include <vector>

#pragma warning(disable:4996)

class Mesh : public Entity
{
public:
	struct vertexInfo
	{
		glm::vec3 pos;
		glm::vec2 uv;
		glm::vec3 normal;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		unsigned int specularExponent;
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
	std::vector<unsigned int> posIndices;
	std::vector<material> materials;

	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec2> vertexUVs;
	std::vector<glm::vec3> vertexNormals;

	std::vector<vertexInfo> vertices;
	std::vector<GLuint> endOfMat;
	std::vector<GLuint> startOfMat;

	GLuint vao;
	GLuint vbo;
	GLuint elementbuffer;

	bool readMTLFile(const char * path);
	GLuint loadImage(const char * imagepath);
public:
	Mesh(const char * filepath);
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
