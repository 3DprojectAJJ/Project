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
	int currMaterial;

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
		else if (strcmp(lineHeader, "mtllib") == 0)
		{
			char path[128];
			fscanf(file, "%s\n", &path);
			readMTLFile(path);
		}
		else if (strcmp(lineHeader, "usemtl") == 0)
		{
			char tmp[128];
			fscanf(file, "%s", tmp);
			for (int i = 0; i < materials.size(); i++)
			{
				if (strcmp(tmp, materials[i].name) == 0)
				{
					currMaterial = i;
					areaStarts.push_back(indices.size());
				}
			}
		}
	} while (true);

	for (int i = 0; i < indices.size(); i++)
	{
		vertices.push_back(tmpVertices[indices[i].vertex - 1]);
		uvs.push_back(tmpUVs[indices[i].uv - 1]);
		normals.push_back(tmpNormals[indices[i].normal - 1]);
	}
	std::fclose(file);
	return true;
}

std::vector<glm::vec3> Obj::getVertices() const
{
	return vertices;
}

std::vector<glm::vec2> Obj::getUVs() const
{
	return uvs;
}

std::vector<glm::vec3> Obj::getNormals() const
{
	return normals;
}

char * Obj::getTexturePath(int area)
{
	return materials[area].map_Kd;
}

int Obj::getNrOfMaterials() const
{
	return materials.size();
}

int Obj::materialAreaStart(int area) const
{
	return areaStarts[area];
}

int Obj::materialAreaEnd(int area) const
{
	int res = vertices.size();
	if (area < materials.size() - 1)
	{
		res = materialAreaStart(area + 1);
	}
	return res;
}

bool Obj::readMTLFile(const char * path)
{
	FILE * file;
	file = fopen(path, "r");
	int res = 0;
	char lineHeader[128];

	do
	{
		res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			break;
		}

		if (std::strcmp(lineHeader, "newmtl") == 0)
		{
			material mt;
			materials.push_back(mt);
			fscanf(file, "%s\n", &materials[materials.size() - 1].name);
		}
		else if (std::strcmp(lineHeader, "Ns") == 0)
		{
			fscanf(file, "%d\n", &materials[materials.size() - 1].Ns);
		}
		else if (std::strcmp(lineHeader, "Ni") == 0)
		{
			fscanf(file, "%f\n", &materials[materials.size() - 1].Ni);
		}
		else if (std::strcmp(lineHeader, "d") == 0)
		{
			fscanf(file, "%f\n", &materials[materials.size() - 1].d);
		}
		else if (std::strcmp(lineHeader, "Tf") == 0)
		{
			fscanf(file, "%d %d %d\n", &materials[materials.size() - 1].Tf.x, &materials[materials.size() - 1].Tf.y, &materials[materials.size() - 1].Tf.z);
		}
		else if (std::strcmp(lineHeader, "illum") == 0)
		{
			fscanf(file, "%d\n", &materials[materials.size() - 1].illum);
		}
		else if (std::strcmp(lineHeader, "Ka") == 0)
		{
			fscanf(file, "%f %f %f\n", &materials[materials.size() - 1].Ka.x, &materials[materials.size() - 1].Ka.y, &materials[materials.size() - 1].Ka.z);
		}
		else if (std::strcmp(lineHeader, "Kd") == 0)
		{
			fscanf(file, "%f %f %f\n", &materials[materials.size() - 1].Kd.x, &materials[materials.size() - 1].Kd.y, &materials[materials.size() - 1].Kd.z);
		}
		else if (std::strcmp(lineHeader, "Ks") == 0)
		{
			fscanf(file, "%f %f %f\n", &materials[materials.size() - 1].Ks.x, &materials[materials.size() - 1].Ks.y, &materials[materials.size() - 1].Ks.z);
		}
		else if (std::strcmp(lineHeader, "map_Kd") == 0)
		{
			fscanf(file, "%s\n", &materials[materials.size() - 1].map_Kd);
		}
	} while (true);
	std::fclose(file);
	return true;
}
