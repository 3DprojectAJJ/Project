#include "obj.h"

Obj::Obj()
{
}

Obj::~Obj()
{
}

bool Obj::readOBJFile(const char * path)
{
	FILE * file;
	file = fopen(path, "r");
	if (file == NULL)
	{
		return false;
	}

	std::vector<glm::vec3> tmpVertices;
	std::vector<glm::vec2> tmpUVs;
	std::vector<glm::vec3> tmpNormals;


	int res = 0;
	char lineHeader[128];

	do 
	{
		res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			break;
		}

		if (std::strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			tmpVertices.push_back(vertex);
		}
		else if (std::strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);

			tmpUVs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			tmpNormals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				return false;
			}
			for (int i = 0; i < 3; i++)
			{
				index tmp;
				tmp.vertex = vertexIndex[i];
				tmp.uv = uvIndex[i];
				tmp.normal = normalIndex[i];

				indices.push_back(tmp);
			}
		}
	} while (true);

	for (int i = 0; i < indices.size(); i++)
	{
		vertices.push_back(tmpVertices[indices[i].vertex - 1]);
		uvs.push_back(tmpUVs[indices[i].uv - 1]);
		normals.push_back(tmpNormals[indices[i].normal - 1]);
	}
	return true;
}

std::vector<glm::vec3> Obj::getVertices() const
{
	return vertices;
}
