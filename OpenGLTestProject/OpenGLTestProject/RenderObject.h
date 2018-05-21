#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "ObjLoader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include <GLM\mat4x4.hpp>

class RenderObject
{
public:

private:
	std::vector<float> m_data;
	std::vector<unsigned int> m_indexing;
	glm::mat4x4 m_model;
	GLuint m_texID;
	VertexArray vao;
	VertexBuffer vbo;
	IndexBuffer ib;
	void addToData(ObjLoader::DataFormat format);
public:
	RenderObject(std::vector<ObjLoader::DataFormat> data, std::vector<unsigned int> indexing, GLuint texID);
	RenderObject(const void * data, unsigned int size, const unsigned int * indices, unsigned int count);
	~RenderObject();
	void draw(unsigned int program);
	void setTex(GLuint id);
};

#endif