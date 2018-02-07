#ifndef OBJ_H
#define OBJ_H
#include <GLM\common.hpp>
#include <fstream>
#include <iostream>
#include <string>
class Obj
{
private:
	void expandVec3(glm::vec3 ** list, int size, int * buff);
	void expandVec2(glm::vec2 ** list, int size, int * buff);
	void expandIndices();
public:
	Obj(int verticesBuffSize = 2, int uvsBufferSize = 2, int normalsBuffSize = 2, int buffIndicesSize = 2);
	~Obj();
	void addVertex(glm::vec3 vertex);
	void addUV(glm::vec2 uv);
	void addNormal(glm::vec3 normal);
	void addIndex(glm::vec3 indices);
	bool readOBJFile(const char * path);
private:
	int sizeV = 0;
	int buffV;

	int sizeVT = 0;
	int buffVT;

	int sizeVN = 0;
	int buffVN;

	int sizeIndices = 0;
	int buffIndices;

	glm::vec3 ** vertices;
	glm::vec2 ** uvs;
	glm::vec3 ** normals;

	struct index
	{
		unsigned int vertex;
		unsigned int uv;
		unsigned int normal;
	};

	index ** indices;

};


#endif

