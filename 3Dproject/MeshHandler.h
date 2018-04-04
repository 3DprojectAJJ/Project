#ifndef MESHHANDLER_H
#define MESHHANDLER_H
#include "Mesh.h"
#include<GLM\matrix.hpp>
class MeshHandler
{
private:
	GLuint vao;
	GLuint vbo;
	std::vector<Mesh::vertexInfo> vertices;
	std::vector<Mesh> meshes;
	std::vector<glm::mat4> worldMatrices;
	std::vector<std::vector<GLuint>> startOfMaterials;
	std::vector<std::vector<GLuint>> endOfMaterials;
public:
	MeshHandler(std::vector<Mesh> meshes);
	~MeshHandler();
	void makeBuffer(GLuint program);
	void draw(GLuint program);
	void addMesh(Mesh & mesh);
	glm::mat4 getMatrix(int index);
	void setMatrix(int index, glm::mat4 matrix);
};
#endif