#ifndef TERRAIN_H
#define TERRAIN_H
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <vector>
#include <GLM\glm.hpp>
#pragma warning(disable:4996)

class Terrain {
private:
	int width, height, heightScaling;
	std::vector<int> indices;
	std::vector<glm::vec3> heightmap;
	glm::mat4 world;

	GLuint vertexArrayID, vertexBuffer, texbuffer, elementbuffer, tex;
	GLuint loadImage(const char * imagepath);
public:
	Terrain(const char* filepath);

	void readHeightMap(const char* filepath);
	void draw(GLuint program);

};

#endif // !TERRAIN_H
