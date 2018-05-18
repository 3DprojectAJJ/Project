#include "ObjLoader.h"

bool ObjLoader::readMTLFile(const char * path)
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

		// Assumption, each object only uses ONE material. 
		if (std::strcmp(lineHeader, "Ns") == 0)
		{
			fscanf(file, "%f\n", &mat.specular.w);
		}
		else if (std::strcmp(lineHeader, "Ka") == 0)
		{
			fscanf(file, "%f %f %f\n", &mat.ambient);
		}
		else if (std::strcmp(lineHeader, "Kd") == 0)
		{
			fscanf(file, "%f %f %f\n", &mat.diffuse);
		}
		else if (std::strcmp(lineHeader, "Ks") == 0)
		{
			glm::vec3 tmp;
			fscanf(file, "%f %f %f\n", &tmp);
			mat.specular.x = tmp.x;
			mat.specular.y = tmp.y;
			mat.specular.z = tmp.z;
		}
		// Assumption, each object only has one texture
		else if (std::strcmp(lineHeader, "map_Kd") == 0)
		{
			char path[128];
			fscanf(file, "%s\n", &path);
			texID = loadImage(path);
		}
	} while (true);
	return false;
}

GLuint ObjLoader::loadImage(const char * imagepath)
{
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
	return textureID;
}

ObjLoader::ObjLoader()
{
}

ObjLoader::~ObjLoader()
{
}

bool ObjLoader::readOBJFile(const char * path)
{
	FILE * file;
	file = fopen(path, "r");
	if (file == NULL)
	{
		return false;
	}

	std::vector<glm::vec3> tmpPositions;
	std::vector<glm::vec2> tmpUvs;
	std::vector<glm::vec3> tmpNormals;

	int res = 0;
	char lineHeader[128];
	bool readingVertices = true;
	Material currmat;
	do
	{
		res = fscanf(file, "%s", lineHeader);

			if (res == EOF)
			{
				break;
			}

			if (readingVertices)
			{
				if (std::strcmp(lineHeader, "v") == 0)
				{
					glm::vec3 vertex;
					fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
					tmpPositions.push_back(vertex);
				}
				else if (std::strcmp(lineHeader, "vt") == 0)
				{
					glm::vec2 uv;
					fscanf(file, "%f %f\n", &uv.x, &uv.y);
					tmpUvs.push_back(uv);
				}
				else if (strcmp(lineHeader, "vn") == 0)
				{
					glm::vec3 normal;
					fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
					tmpNormals.push_back(normal);
				}
			}

		if (strcmp(lineHeader, "f") == 0)
		{
			std::vector<DataFormat> tmpDatas;
			if (readingVertices)
			{
				readingVertices = false;
			}

			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			
			if (matches != 9)
			{
				return false;
			}

			for (int i = 0; i < 3; i++)
			{
				DataFormat d;
				d.pos = tmpPositions[vertexIndex[i] - 1];
				d.uv = tmpUvs[uvIndex[i] - 1];
				d.normal = tmpNormals[normalIndex[i] - 1];
				d.ambient = mat.ambient;
				d.diffuse = mat.diffuse;
				d.specular = mat.specular;

				m_data.push_back(d);
			}

		}
		else if (strcmp(lineHeader, "usemtl") == 0)
		{
			// no need since we already only have one material
		}
		else if (std::strcmp(lineHeader, "mtllib") == 0)
		{
			char path[128];
			fscanf(file, "%s\n", &path);
			readMTLFile(path);
		}
	} while(true);

	return false;
}