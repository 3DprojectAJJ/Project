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

RenderObject::RenderObject(std::vector<ObjLoader::DataFormat> data, std::vector<unsigned int> indexing, GLuint texID) :ib(&indexing[0], indexing.size()), m_model(1.0f)
{
	m_texID = texID;
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

RenderObject::RenderObject(const void * data, unsigned int size, const unsigned int * indices, unsigned int count): ib(indices,count), m_model(1.0f)
{
	vbo.init(data, size);
	VertexBufferLayout layout;
	layout.push<float>(2);
	vao.addBuffer(vbo, layout);
}

RenderObject::~RenderObject()
{
}

void RenderObject::draw(unsigned int program)
{
	glUseProgram(program);

	vao.bind();
	ib.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texID);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"),1,GL_FALSE, &m_model[0][0]);

	GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));

	vao.unBind();
	ib.unBind();

}

void RenderObject::setTex(GLuint id)
{
	m_texID = id;
}
