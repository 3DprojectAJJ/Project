#include "RenderObject.h"

void RenderObject::addToData(DataFormat format)
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

RenderObject::RenderObject(std::vector<DataFormat> data, std::vector<unsigned int> indexing) :ib(&indexing[0], indexing.size())
{
	unsigned int nrOfVertices = indexing.size();

	for (int i = 0; i < nrOfVertices; i++)
	{
		addToData(data[indexing[i]]);
	}

	vbo.init(&data[0], nrOfVertices * sizeof(DataFormat));

	VertexBufferLayout layout;
	layout.push<float>(3);
	layout.push<float>(2);
	layout.push<float>(3);
	layout.push<float>(3);
	layout.push<float>(3);
	layout.push<float>(4);
}

RenderObject::~RenderObject()
{
}
