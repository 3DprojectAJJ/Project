#ifndef OBJ_H
#define OBJ_H
#include <GLM\common.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

class Obj
{
public:
	Obj();
	~Obj();
	bool readOBJFile(const char * path);
	std::vector<glm::vec3> getVertices() const;
	std::vector<glm::vec2> getUVs() const;
	std::vector<glm::vec3> getNormals() const;
	char * getTexturePath();
private:
	bool readMTLFile(const char * path);
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

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
	};

	struct index
	{
		unsigned int vertex;
		unsigned int uv;
		unsigned int normal;
		material mat;
	};

	std::vector<index> indices;
	std::vector<material> materials;

};


#endif

