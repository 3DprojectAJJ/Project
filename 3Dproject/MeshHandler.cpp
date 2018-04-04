#include "MeshHandler.h"

MeshHandler::MeshHandler(std::vector<Mesh> meshes)
{
	this->meshes = meshes;
	for (int i = 0; i < meshes.size(); i++)
	{
		for (int j = 0; j < meshes[i].getVertices().size(); j++)
		{
			vertices.push_back(meshes[i].getVertices()[j]);
		}
		worldMatrices.push_back(meshes[i].getMatrix());
		startOfMaterials.push_back(meshes[i].getStartOfMaterial());
		endOfMaterials.push_back(meshes[i].getEndOfMaterial());
	}
}

MeshHandler::~MeshHandler()
{
	//nothing
}

void MeshHandler::makeBuffer(GLuint program)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::vertexInfo)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

	GLint vertexPos = glGetAttribLocation(program, "vertexPositionModelSpace");

	if (vertexPos == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertex_position' attribute in Vertex shader\n");
		return;
	}

	glVertexAttribPointer(
		vertexPos,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Mesh::vertexInfo),
		(void*)0
	);

	GLint vertexUV = glGetAttribLocation(program, "vertexUV");

	if (vertexUV == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertex_UV' attribute in Vertex shader\n");
		return;
	}

	glVertexAttribPointer(
		vertexUV,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Mesh::vertexInfo),
		(void*)sizeof(glm::vec2)
	);

	GLint vertexNormal = glGetAttribLocation(program, "vertexNormal");

	glVertexAttribPointer(
		vertexNormal,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Mesh::vertexInfo),
		(void*)sizeof(glm::vec3)
	);

	GLint vertexAmbient = glGetAttribLocation(program, "vertexAmbient");

	if (vertexAmbient == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertex_Ambient' attribute in Vertex shader\n");
		return;
	}
	else
	{

		glVertexAttribPointer(
			vertexAmbient,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Mesh::vertexInfo),
			(void*)sizeof(glm::vec3)
		);
	}

	GLint vertexDiffuse = glGetAttribLocation(program, "vertexDiffuse");

	if (vertexDiffuse == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertex_Diffuse' attribute in Vertex shader\n");
		return;
	}
	else
	{

		glVertexAttribPointer(
			vertexDiffuse,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Mesh::vertexInfo),
			(void*)sizeof(glm::vec3)
		);
	}

	GLint vertexSpecular = glGetAttribLocation(program, "vertexSpecular");

	if (vertexSpecular == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertex_Specular' attribute in Vertex shader\n");
		return;
	}
	else
	{
		glVertexAttribPointer(
			vertexSpecular,
			4,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Mesh::vertexInfo),
			(void*)sizeof(glm::vec4)
		);
	}
}

void MeshHandler::draw(GLuint program)
{
	glUseProgram(program);
	for(int i = 0; i < 3; i++)
		glEnableVertexAttribArray(i);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glActiveTexture(GL_TEXTURE0);
	GLuint boost = 0;
	for (int i = 0; i < startOfMaterials.size(); i++)
	{
		GLint tst = glGetUniformLocation(program, "Model");
		glUniformMatrix4fv(glGetUniformLocation(program, "Model"), 1, GL_FALSE, &worldMatrices[i][0][0]);
		for (int j = 0; j < startOfMaterials[i].size(); j++)
		{
			glBindTexture(GL_TEXTURE_2D, vertices[startOfMaterials[i][j] + boost].texID);
			glUniform1i(glGetUniformLocation(program, "tex"), 0);
			glDrawArrays(GL_TRIANGLES, startOfMaterials[i][j] + boost, endOfMaterials[i][j] + boost);
		}
		boost += endOfMaterials[i][startOfMaterials[i].size() - 1];
	}

	for (int i = 0; i < 3; i++)
		glDisableVertexAttribArray(i);
}

void MeshHandler::addMesh(Mesh & mesh)
{
	meshes.push_back(mesh);
	startOfMaterials.push_back(mesh.getStartOfMaterial());
	endOfMaterials.push_back(mesh.getEndOfMaterial());
	worldMatrices.push_back(mesh.getMatrix());
	std::vector<Mesh::vertexInfo> tmp = mesh.getVertices();
	for (int i = 0; i < tmp.size(); i++)
	{
		vertices.push_back(tmp[i]);
	}
}

glm::mat4 MeshHandler::getMatrix(int index)
{
	return worldMatrices[index];
}

void MeshHandler::setMatrix(int index, glm::mat4 matrix)
{
	worldMatrices[index] = matrix;
}