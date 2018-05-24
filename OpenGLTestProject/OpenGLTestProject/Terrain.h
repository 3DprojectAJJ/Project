#ifndef TERRAIN_H
#define TERRAIN_H
#include "Entity.h"
#include <string>
#include <vector>
#pragma warning(disable:4996)

class Terrain : public Entity
{
private:
	int width, height, heightScaling;
	std::vector<int> indices;
	std::vector<glm::vec3> heightmap;
	std::vector<glm::vec2> heightmaptex;

	std::vector<glm::vec3> ambient;
	std::vector<glm::vec3> diffuse;
	std::vector<glm::vec4> specular;
	glm::mat4 world;

	std::vector<glm::vec3> tangents;
	GLuint tangentID;
	std::vector<glm::vec3> bitangents;
	GLuint bitangentID;

	GLuint normalID;
	GLuint vertexArrayID, vertexBuffer, texbuffer, elementbuffer, tex, amBuffer, diffBuffer, specBuffer;
	GLuint loadImage(const char * imagepath);
public:
	Terrain(const char* filepath);

	void loadNormalMap(GLuint program, const char * path);
	void readHeightMap(const char* filepath);
	void draw(GLuint program);

};

#endif // !TERRAIN_H
