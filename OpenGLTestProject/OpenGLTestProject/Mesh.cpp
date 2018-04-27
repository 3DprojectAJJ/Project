#include "Mesh.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

bool Mesh::readMTLFile(const char * path)
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

	for (int i = 0; i < materials.size(); i++)
	{
		materials[i].kdTex = loadImage(materials[i].map_Kd);
	}
	return true;
}
GLuint Mesh::loadImage(const char * imagepath) {

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

Mesh::Mesh(glm::mat4 worldMat)
{
	world = worldMat;
}

Mesh::~Mesh()
{
	//Nothing
}

bool Mesh::readOBJFile(const char * path)
{
	FILE * file;
	file = fopen(path, "r");
	if (file == NULL)
	{
		return false;
	}

	std::vector<glm::vec3> tmpPositions;
	std::vector<glm::vec2> tmpUVs;
	std::vector<glm::vec3> tmpNormals;


	int res = 0;
	char lineHeader[128];
	unsigned int currMaterial;

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
			tmpPositions.push_back(vertex);
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
				tmp.pos = vertexIndex[i];
				tmp.uv = uvIndex[i];
				tmp.normal = normalIndex[i];
				tmp.mat = currMaterial;
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
					if (!startOfMat.empty())
					{
						endOfMat.push_back(indices.size());
					}
					startOfMat.push_back(indices.size());
				}
			}
		}
	} while (true);
	endOfMat.push_back(indices.size());
	for (int i = 0; i < indices.size(); i++)
	{
		vertexInfo tmp;
		tmp.pos = tmpPositions[indices[i].pos - 1];
		tmp.uv = tmpUVs[indices[i].uv - 1];
		tmp.normal = tmpNormals[indices[i].normal - 1];
		tmp.ambient = materials[indices[i].mat].Ka;
		tmp.diffuse = materials[indices[i].mat].Kd;
		tmp.specular = materials[indices[i].mat].Ks;
		tmp.specularExponent = materials[indices[i].mat].Ns;
		tmp.texID = materials[indices[i].mat].kdTex;

		vertices.push_back(tmp);
	}
	std::fclose(file);
	return true;
}

void Mesh::makeBuffer(GLuint program)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexInfo)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	GLint vertexPos = glGetAttribLocation(program, "vertexPosition");

	if (vertexPos == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexPosition' attribute in Vertex shader\n");
		return;
	}

	glVertexAttribPointer(
		vertexPos,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertexInfo),
		(void*)0
	);
	glEnableVertexAttribArray(1);
	GLint vertexUV = glGetAttribLocation(program, "vertexUV");

	if (vertexUV == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexUV' attribute in Vertex shader\n");
		return;
	}

	glVertexAttribPointer(
		vertexUV,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertexInfo),
		BUFFER_OFFSET(3*sizeof(GL_FLOAT))
	);

	/*glEnableVertexAttribArray(3);
	GLint vertexAmbient = glGetAttribLocation(program, "vertexAmbient");

	if (vertexAmbient == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexAmbient' attribute in Vertex shader");
		return;
	}

	glVertexAttribPointer(
		vertexAmbient,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertexInfo),
		BUFFER_OFFSET(8 * sizeof(GL_FLOAT))
	);

	glEnableVertexAttribArray(4);
	GLint vertexDiffuse = glGetAttribLocation(program, "vertexDiffuse");

	if (vertexDiffuse == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexDiffuse' attribute in Vertex shader");
		return;
	}

	glVertexAttribPointer(
		vertexDiffuse,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertexInfo),
		BUFFER_OFFSET(11 * sizeof(GL_FLOAT))
	);

	glEnableVertexAttribArray(5);
	GLint vertexSpecular = glGetAttribLocation(program, "vertexSpecular");

	if (vertexSpecular == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexDiffuse' attribute in Vertex shader");
		return;
	}

	glVertexAttribPointer(
		vertexSpecular,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertexInfo),
		BUFFER_OFFSET(14 * sizeof(GL_FLOAT))
	);

	glEnableVertexAttribArray(6);
	GLint vertexSpecularExponent = glGetAttribLocation(program, "vertexSpecularExponent");

	if (vertexSpecularExponent == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexSpecularExponent' attribute in Vertex shader");
		return;
	}

	glVertexAttribPointer(
		vertexSpecularExponent,
		1,
		GL_INT,
		GL_FALSE,
		sizeof(vertexInfo),
		BUFFER_OFFSET(17 * sizeof(GL_FLOAT))
	);*/

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	/*glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(6);*/
}
void Mesh::draw(GLuint program)
{
	glUseProgram(program);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glActiveTexture(GL_TEXTURE0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &world[0][0]);
	for (int i = 0; i < startOfMat.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, vertices[startOfMat[i]].texID);
		//GLuint tmp = glGetUniformLocation(program, "tex");
		glUniform1i(glGetUniformLocation(program, "tex"), 0);
		glDrawArrays(GL_TRIANGLES, startOfMat[i], endOfMat[i]);
	}
}

std::vector<Mesh::vertexInfo> Mesh::getVertices() const
{
	return vertices;
}

std::vector<Mesh::material> Mesh::getMaterials() const
{
	return materials;
}
std::vector<Mesh::index> Mesh::getIndices() const
{
	return indices;
}

glm::mat4 Mesh::getMatrix() const
{
	return world;
}

void Mesh::setMatrix(glm::mat4 world)
{
	this->world = world;
}

std::vector<GLuint> Mesh::getStartOfMaterial() const
{
	return startOfMat;
}

std::vector<GLuint> Mesh::getEndOfMaterial() const
{
	return endOfMat;
}