#ifndef OBJ_H
#define OBJ_H
#include <GLM\common.hpp>
#include <fstream>
#include <iostream>
class Obj
{
private:
	void expandVec3(glm::vec3 ** list, int size, int * buff);
	void expandVec2(glm::vec2 ** list, int size, int * buff);
public:
	Obj(int verticesBuffSize = 2, int uvsBufferSize = 2, int normalsBuffSize = 2);
	~Obj();
	void addVertex(glm::vec3 vertex);
	void addUV(glm::vec2 uv);
	void addNormal(glm::vec3 normal);
	bool readOBJFile(const char * path);
private:
	int sizeV = 0;
	int buffV;

	int sizeVT = 0;
	int buffVT;

	int sizeVN = 0;
	int buffVN;

	glm::vec3 ** vertices;
	glm::vec2 ** uvs;
	glm::vec3 ** normals;

};


#endif

