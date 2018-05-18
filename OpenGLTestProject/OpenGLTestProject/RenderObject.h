#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "ObjLoader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

class RenderObject
{
public:

private:
	std::vector<float> m_data;
	std::vector<unsigned int> m_indexing;
	VertexArray vao;
	VertexBuffer vbo;
	IndexBuffer ib;
	GLuint texID;
	void addToData(ObjLoader::DataFormat format);
public:
	RenderObject(std::vector<ObjLoader::DataFormat> data, std::vector<unsigned int> indexing);
	RenderObject(const void * data, unsigned int size, const unsigned int * indices, unsigned int count);
	~RenderObject();
	void draw();
	void setTex(GLuint id);
};

#endif