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

void Obj::expandIndices()
{
	if (sizeIndices == buffIndices)
	{
		index ** tmp = new index*[buffIndices * 2];

		for (int i = 0; i < buffIndices; i++)
		{
			tmp[i] = indices[i];
		}

		for (int i = buffIndices; i < buffIndices * 2; i++)
		{
			tmp[i] = nullptr;
		}

		delete[] indices;
		indices = tmp;
	}
}

Obj::Obj(int verticesBuffSize, int uvsBufferSize, int normalsBuffSize, int buffIndicesSize)
{
	buffV = verticesBuffSize;
	buffVT = uvsBufferSize;
	buffVN = normalsBuffSize;
	buffIndices = buffIndicesSize;

	vertices = new glm::vec3*[buffV];
	uvs = new glm::vec2*[buffVT];
	normals = new glm::vec3*[buffVN];
	indices = new index*[buffIndices];

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

	for (int i = 0; i < buffIndices; i++)
	{
		indices[i] = nullptr;
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

void Obj::addIndex(glm::vec3 indices)
{
	expandIndices();

	this->indices[sizeIndices++]->vertex = indices.x;
	this->indices[sizeIndices++]->uv = indices.y;
	this->indices[sizeIndices++]->normal = indices.z;
}

bool Obj::readOBJFile(const char * path)
{
	FILE * file;
	file = fopen(path, "r");
	if (file == NULL)
	{
		return false;
	}
	int res = 0;
	char lineHeader[128];

	while (res == EOF)
	{
		res = fscanf(file, "%s", lineHeader);

		if (std::strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			addVertex(vertex);
		}
		else if (std::strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			addUV(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			addNormal(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				return false;
			}
			for(int i = 0; i < 3; i++)
				addIndex(glm::vec3(vertexIndex[i], uvIndex[i], normalIndex[i]));
		}
	}
}
