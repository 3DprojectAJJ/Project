#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H
#include <vector>
#include <GLM\vec2.hpp>
#include <GLM\vec3.hpp>
#include <GLM\vec4.hpp>
#include "VertexArray.h"
#include "IndexBuffer.h"
class RenderObject
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
	std::vector<float> m_data;
	std::vector<unsigned int> m_indexing;
	VertexArray vao;
	VertexBuffer vbo;
	IndexBuffer ib;
	void addToData(DataFormat format);
public:
	RenderObject(std::vector<DataFormat> data, std::vector<unsigned int> indexing);
	~RenderObject();

};

#endif