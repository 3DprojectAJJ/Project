#include "RenderObject.h"

void RenderObject::addToData(ObjLoader::DataFormat format)
{
	m_data.push_back(format.pos.x);
	m_data.push_back(format.pos.y);
	m_data.push_back(format.pos.z);

	m_data.push_back(format.uv.x);
	m_data.push_back(format.uv.y);

	/*m_data.push_back(format.normal.x);
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
	m_data.push_back(format.specular.w);*/
}

RenderObject::RenderObject(std::vector<ObjLoader::DataFormat> data, std::vector<unsigned int> indexing, GLuint texID, GLuint programID) :ib(&indexing[0], indexing.size()), m_model(1.0f)
{
	m_texID = texID;
	unsigned int nrOfVertices = data.size();

	for (int i = 0; i < nrOfVertices; i++)
	{
		addToData(data[i]);
	}
	vao.bind();
	vbo.init(&m_data[0], nrOfVertices * (3+2) * sizeof(float));

	VertexBufferLayout layout;
	layout.push<float>(3);
	layout.push<float>(2);
	//layout.push<float>(3);
	//layout.push<float>(3);
	//layout.push<float>(3);
	//layout.push<float>(1);

	vao.addBuffer(vbo, layout);

	/*glBindBuffer(GL_ARRAY_BUFFER,vbo.getID());
	GLint vertexPos = glGetAttribLocation(programID, "vertexPosition");
	glEnableVertexAttribArray(vertexPos);
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint vertexUV = glGetAttribLocation(programID, "vertexUV");
	glEnableVertexAttribArray(vertexUV);
	glVertexAttribPointer(vertexUV, 2, GL_FLOAT, GL_FALSE, 3*4, nullptr);

	GLint vertexAmbient = glGetAttribLocation(programID, "vertexAmbient");
	glEnableVertexAttribArray(vertexAmbient);
	glVertexAttribPointer(vertexAmbient, 3, GL_FLOAT, GL_FALSE, 3*2*4, nullptr);

	GLint vertexDiffuse = glGetAttribLocation(programID, "vertexDiffuse");
	glEnableVertexAttribArray(vertexDiffuse);
	glVertexAttribPointer(vertexDiffuse, 3, GL_FLOAT, GL_FALSE, 3*2*3*4, nullptr);


	glDisableVertexAttribArray(vertexPos);
	glDisableVertexAttribArray(vertexUV);
	glDisableVertexAttribArray(vertexAmbient);
	glDisableVertexAttribArray(vertexDiffuse);*/

	vao.unBind();
	ib.unBind();
}

RenderObject::RenderObject(const void * data, unsigned int size, const unsigned int * indices, unsigned int count): ib(indices,count), m_model(1.0f)
{
	vbo.init(data, size);
	VertexBufferLayout layout;
	layout.push<float>(2);
	vao.addBuffer(vbo, layout);
}

RenderObject::RenderObject(const float * data, unsigned int size, const unsigned int * indices, unsigned int count, GLuint texID,  std::vector<unsigned int> layouts)
{
	m_texID = texID;
	ib.init(indices, count);
	for (int i = 0; i < size; i++)
	{
		m_data.push_back(data[i]);
	}
	vao.bind();
	vbo.init(&m_data[0], size*sizeof(float));
	VertexBufferLayout layout;
	for (int i = 0; i < layouts.size; i++)
	{
		layout.push<float>(layouts[i]);
	}
	vao.addBuffer(vbo,layout);
	vao.unBind();
	ib.unBind();
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
	/*GLint vertexPos = glGetAttribLocation(program, "vertexPosition");
	glBindBuffer(GL_ARRAY_BUFFER, vbo.getID());
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint vertexUV = glGetAttribLocation(program, "vertexUV");
	glBindBuffer(GL_ARRAY_BUFFER, vbo.getID());
	glVertexAttribPointer(vertexUV, 2, GL_FLOAT, GL_FALSE, 3 * 4, nullptr);

	GLint vertexAmbient = glGetAttribLocation(program, "vertexAmbient");
	glBindBuffer(GL_ARRAY_BUFFER, vbo.getID());
	glVertexAttribPointer(vertexAmbient, 3, GL_FLOAT, GL_FALSE, 3 * 2 * 4, nullptr);

	GLint vertexDiffuse = glGetAttribLocation(program, "vertexDiffuse");
	glBindBuffer(GL_ARRAY_BUFFER, vbo.getID());
	glVertexAttribPointer(vertexDiffuse, 3, GL_FLOAT, GL_FALSE, 3 * 2 * 3 * 4, nullptr);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texID);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &m_model[0][0]);*/

	GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));

	/*glDisableVertexAttribArray(vertexPos);
	glDisableVertexAttribArray(vertexUV);
	glDisableVertexAttribArray(vertexAmbient);
	glDisableVertexAttribArray(vertexDiffuse);*/

	vao.unBind();
	ib.unBind();

}

void RenderObject::setTex(GLuint id)
{
	m_texID = id;
}

GLuint RenderObject::loadImage(const char * imagepath) {

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int imageSize;

	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if (!file)
	{
		printf("Image could not be opened\n");
		return -1;
	}

	if (fread(header, 1, 54, file) != 54)
	{
		printf("Not a correct BMP file\n");
		return -1;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return -1;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)
		imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)
		dataPos = 54; // The BMP header is done that way

					  // Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete[] data;
	m_texID = textureID;
	return textureID;
}