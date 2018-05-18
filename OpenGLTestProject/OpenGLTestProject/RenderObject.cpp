#include "RenderObject.h"

void RenderObject::addToData(ObjLoader::DataFormat format)
{
	m_data.push_back(format.pos.x);
	m_data.push_back(format.pos.y);
	m_data.push_back(format.pos.z);

	m_data.push_back(format.uv.x);
	m_data.push_back(format.uv.y);

	m_data.push_back(format.normal.x);
	m_data.push_back(format.normal.y);
	m_data.push_back(format.normal.z);

	m_data.push_back(format.ambient.x);
	m_data.push_back(format.ambient.y);
	m_data.push_back(format.ambient.z);

	m_data.push_back(format.diffuse.x);
	m_data.push_back(format.diffuse.y);
	m_data.push_back(format.diffuse.z);

	m_data.push_back(format.specular.x);
	m_data.push_back(format.specular.y);
	m_data.push_back(format.specular.z);
	m_data.push_back(format.specular.w);
}

RenderObject::RenderObject(std::vector<ObjLoader::DataFormat> data, std::vector<unsigned int> indexing) :ib(&indexing[0], indexing.size())
{
	unsigned int nrOfVertices = data.size();

	for (int i = 0; i < nrOfVertices; i++)
	{
		addToData(data[i]);
	}

	vbo.init(&data[0], nrOfVertices * (3+2+3+3+3+4) * sizeof(float));

	VertexBufferLayout layout;
	layout.push<float>(3);
	layout.push<float>(2);
	layout.push<float>(3);
	layout.push<float>(3);
	layout.push<float>(3);
	layout.push<float>(3);
	layout.push<float>(1);

	vao.addBuffer(vbo, layout);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

RenderObject::RenderObject(const void * data, unsigned int size, const unsigned int * indices, unsigned int count): ib(indices,count)
{
	vbo.init(data, size);
	VertexBufferLayout layout;
	layout.push<float>(2);
	vao.addBuffer(vbo, layout);
}

RenderObject::~RenderObject()
{
}

void RenderObject::draw()
{
	vao.bind();
	ib.bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
}

void RenderObject::setTex(GLuint id)
{
	texID = id;
}
