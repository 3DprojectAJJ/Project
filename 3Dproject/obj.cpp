#include "obj.h"

void Obj::expandVec3(glm::vec3 ** list, int size, int * buff)
{
	if (size == *buff)
	{
		glm::vec3 ** tmp = new glm::vec3*[*buff * 2];

		for (int i = 0; i < *buff; i++)
		{
			tmp[i] = list[i];
		}

		for (int i = *buff; i < *buff * 2; i++)
		{
			tmp[i] = nullptr;
		}

		delete[] list;
		list = tmp;
	}
}

void Obj::expandVec2(glm::vec2 ** list, int size, int * buff)
{
	if (size == *buff)
	{
		glm::vec2 ** tmp = new glm::vec2*[*buff * 2];

		for (int i = 0; i < *buff; i++)
		{
			tmp[i] = list[i];
		}

		for (int i = *buff; i < *buff * 2; i++)
		{
			tmp[i] = nullptr;
		}

		delete[] list;
		list = tmp;
	}
}

Obj::Obj(int verticesBuffSize, int uvsBufferSize, int normalsBuffSize)
{
	buffV = verticesBuffSize;
	buffVT = uvsBufferSize;
	buffVN = normalsBuffSize;

	vertices = new glm::vec3*[buffV];
	uvs = new glm::vec2*[buffVT];
	normals = new glm::vec3*[buffVN];

	for (int i = 0; i < buffV; i++)
	{
		vertices[i] = nullptr;
	}

	for (int i = 0; i < buffVT; i++)
	{
		uvs[i] = nullptr;
	}

	for (int i = 0; i < buffVN; i++)
	{
		normals[i] = nullptr;
	}
}

Obj::~Obj()
{
	for (int i = 0; i < buffV; i++)
	{
		delete vertices[i];
	}

	for (int i = 0; i < buffVT; i++)
	{
		delete uvs[i];
	}

	for (int i = 0; i < buffVN; i++)
	{
		delete normals[i];
	}

	delete[] vertices;
	delete[] uvs;
	delete[] normals;
}

void Obj::addVertex(glm::vec3 vertex)
{
	expandVec3(vertices, sizeV, &buffV);

	vertices[sizeV++] = new glm::vec3(vertex);
}

void Obj::addUV(glm::vec2 uv)
{
	expandVec2(uvs, sizeVT, &buffVT);

	uvs[sizeVT++] = new glm::vec2(uv);
}

void Obj::addNormal(glm::vec3 normal)
{
	expandVec3(normals, sizeVN, &buffVN);

	normals[sizeVN++] = new glm::vec3(normal);
}

bool Obj::readOBJFile(const char * path)
{
	FILE * file;
	file = fopen(path, "r");
	if (file == NULL)
	{
		return false;
	}
	int res;
	char lineHeader[128];

	while (res == EOF)
	{
		res = fscanf(file, "%s", lineHeader);
	}
}
