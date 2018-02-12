#ifndef OBJ_H
#define OBJ_H
#include <GLM\common.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Obj
{
public:
	Obj();
	~Obj();
	bool readOBJFile(const char * path);
	std::vector<glm::vec3> getVertices() const;
	std::vector<glm::vec2> getUVs() const;
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	struct index
	{
		unsigned int vertex;
		unsigned int uv;
		unsigned int normal;
	};

	std::vector<index> indices;

};


#endif

