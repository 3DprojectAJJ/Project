#include "Mesh.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Reads a material file for an obj and saves values important values as a material.
bool Mesh::readMTLFile(const char * path)
{
	// open file for reading
	FILE * file;
	file = fopen(path, "r");
	int res = 0;
	char lineHeader[128];

	do
	{
		// scans a header which determines which kind of values will be read, each if statement ahead determines what header there is. 
		res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
		{
			break;
		}

		// start a new material, and often will contain multiple materials
		if (std::strcmp(lineHeader, "newmtl") == 0)
		{
			material mt;
			materials.push_back(mt);
			fscanf(file, "%s\n", &materials[materials.size() - 1].name);
		}
		// Specular exponent or as most call it, the shininess constant 
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
		// Ambient value of the material, the base color (with no light) for the model.
		else if (std::strcmp(lineHeader, "Ka") == 0)
		{
			fscanf(file, "%f %f %f\n", &materials[materials.size() - 1].Ka.x, &materials[materials.size() - 1].Ka.y, &materials[materials.size() - 1].Ka.z);
		}
		// Diffuse value of the material, the amount of light that is accepted to be absorbed for the material color.
		else if (std::strcmp(lineHeader, "Kd") == 0)
		{
			fscanf(file, "%f %f %f\n", &materials[materials.size() - 1].Kd.x, &materials[materials.size() - 1].Kd.y, &materials[materials.size() - 1].Kd.z);
		}
		// Specular value of the material, the amount of light that should be reflected from the model.
		else if (std::strcmp(lineHeader, "Ks") == 0)
		{
			fscanf(file, "%f %f %f\n", &materials[materials.size() - 1].Ks.x, &materials[materials.size() - 1].Ks.y, &materials[materials.size() - 1].Ks.z);
		}
		// The file path for the diffuse texture map, in this program this value is used as the ambient texture of objects
		else if (std::strcmp(lineHeader, "map_Kd") == 0)
		{
			fscanf(file, "%s\n", &materials[materials.size() - 1].map_Kd);
		}
	} while (true);

	std::fclose(file);

	// for each material we need to load the specified texture for the program.
	for (int i = 0; i < materials.size(); i++)
	{
		materials[i].kdTex = loadImage(materials[i].map_Kd);
	}

	return true;
}

// Loads an image (BMP file) given a filepath, most of this function has been created by following the tutorial "tutorial 5: a textured cube" on the website "www.opengl-tutorial.org"
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

	// Create the texture in the program and prep it for the gpu
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete[] data;
	return textureID;
}

Mesh::Mesh(const char* filepath)
{
	readOBJFile(filepath);
}

Mesh::~Mesh()
{
	//Nothing
}

// Reads a file specified by the given path, and saves relevant values for the mesh.
bool Mesh::readOBJFile(const char * path)
{
	// open file for reading
	FILE * file;
	file = fopen(path, "r");
	if (file == NULL)
	{
		return false;
	}

	// temporary values used in the loop.
	int res = 0;
	char lineHeader[128];
	unsigned int currMaterial;

	do
	{
		// reads a header which determines what type of values we are loading, saves the header in "lineheader".
		res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
		{
			break;
		}

		// v means we have a vertex position, saves the three values to the vector vertexPositions.
		if (std::strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

			vertexPositions.push_back(vertex);
		}
		// vt is the UV values for a vertex, saves the two values to the vector vertexUVs.
		else if (std::strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);

			vertexUVs.push_back(uv);
		}
		// vn is the normal of the vertex, saves the three values to vertexNormals.
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			vertexNormals.push_back(normal);
		}
		// f in an obj file means it's time to specify the triangles, loads the indices and saves to the vector indices
		// Also saves the position indices to the vector posIndices, to make things easier in the future.
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
				
				// since the indices start at 1 in most obj files instead of 0 we subtract 1 when saving to posIndices.
				posIndices.push_back(tmp.pos - 1);
			}
		}
		// mtllib means that we need to read an mtl file specified by the given filepath. Reads and saves to the vector materials.
		else if (strcmp(lineHeader, "mtllib") == 0)
		{
			char path[128];
			fscanf(file, "%s\n", &path);
			readMTLFile(path);
		}
		// Sets a new material as the current material to use, saves as the start and the end of a region of the object, because it is nice with filters.
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
	// make sure that the last end of a region is put in as well.
	endOfMat.push_back(indices.size());

	std::fclose(file);
	return true;
}

// converts all the read data into a vector called "data" and creates buffers.
void Mesh::makeBuffer(GLuint program)
{
	// all indices are to be read and sorted into data.
	for (int i = 0; i < indices.size(); i++)
	{
		// Check if the position already exists in the vector "data", if the position already is the same, we expect
		// everything else to be the same and just save the index instead of the data.
		bool notInData = true;
		for (int j = 0; j < data.size(); j+=21)
		{
			if (data[j] == vertexPositions[indices[i].pos - 1].x && data[j + 1] == vertexPositions[indices[i].pos - 1].y &&data[j + 2] == vertexPositions[indices[i].pos - 1].z)
			{
				notInData = false;
			}
		}
		// Adds all relevant data to "data"
		if (notInData)
		{
			// values such as pos and uv starts their indices at 1 instead of 0, therefore subtract one from the position.
			data.push_back(vertexPositions[indices[i].pos - 1].x);
			data.push_back(vertexPositions[indices[i].pos - 1].y);
			data.push_back(vertexPositions[indices[i].pos - 1].z);

			data.push_back(vertexUVs[indices[i].uv - 1].x);
			data.push_back(vertexUVs[indices[i].uv - 1].y);

			data.push_back(materials[indices[i].mat].Ka.x);
			data.push_back(materials[indices[i].mat].Ka.y);
			data.push_back(materials[indices[i].mat].Ka.z);

			data.push_back(materials[indices[i].mat].Kd.x);
			data.push_back(materials[indices[i].mat].Kd.y);
			data.push_back(materials[indices[i].mat].Kd.z);

			data.push_back(materials[indices[i].mat].Ks.x);
			data.push_back(materials[indices[i].mat].Ks.y);
			data.push_back(materials[indices[i].mat].Ks.z);

			data.push_back(materials[indices[i].mat].Ns);

			// if there is no normal mapping on the object, none of those values are to be loaded into data. 
			if (normalID != 0)
			{
				data.push_back(tangents[i].x);
				data.push_back(tangents[i].y);
				data.push_back(tangents[i].z);

				data.push_back(bitangents[i].x);
				data.push_back(bitangents[i].y);
				data.push_back(bitangents[i].z);
			}
		}
	}

	// create buffers
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*data.size(), &data[0], GL_STATIC_DRAW);

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*posIndices.size(), &posIndices[0], GL_STATIC_DRAW);
}

// Drawcall
void Mesh::draw(GLuint program)
{
	// makes sure we use right program, vbo and ibo.
	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	// Send in model matrix as a uniform
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &world[0][0]);

	// set the stride for the attribpointers.
	GLsizei stride = 15 * sizeof(float);

	// if there is no normal mapping nothing of this should happen
	if (normalID != 0)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalID);

		glUniform1i(glGetUniformLocation(program, "normalMap"), 1);
		glUniform1i(glGetUniformLocation(program, "useNormalMap"), normalID);
		//if normal mapping is used then the stride should add six values for the two three dimensional vectors.
		stride = 21 * sizeof(float);
	}

	// All attribs has the following format:
	// Enable the position in, check if the position could be found and send in values with a stride 
	// that is pre determined and an offset that gets larger with every attribute.
	glEnableVertexAttribArray(0);
	GLint vertexPos = glGetAttribLocation(program, "vertexPosition");
	if (vertexPos == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexPosition' attribute in Vertex shader\n");
	}
	glVertexAttribPointer(
		vertexPos,
		3,
		GL_FLOAT,
		GL_FALSE,
		stride,
		(void*)0
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLint vertexUV = glGetAttribLocation(program, "vertexUV");
	if (vertexUV == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexUV' attribute in Vertex shader\n");
	}
	glVertexAttribPointer(
		vertexUV,
		2,
		GL_FLOAT,
		GL_FALSE,
		stride,
		(void*)(3*4)
	);

	glEnableVertexAttribArray(2);
	GLint vertexAmbient = glGetAttribLocation(program, "vertexAmbient");
	if (vertexAmbient == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexAmbient' attribute in Vertex shader\n");
	}
	glVertexAttribPointer(
		vertexAmbient,
		3,
		GL_FLOAT,
		GL_FALSE,
		stride,
		(void*)((3 + 2)* 4)
		);

	glEnableVertexAttribArray(3);
	GLint vertexDiffuse = glGetAttribLocation(program, "vertexDiffuse");
	if (vertexDiffuse == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexDiffuse' attribute in Vertex shader\n");
	}
	glVertexAttribPointer(
		vertexDiffuse,
		3,
		GL_FLOAT,
		GL_FALSE,
		stride,
		(void*)((3 + 2 + 3) * 4)
	);


	glEnableVertexAttribArray(4);
	GLint vertexSpecular = glGetAttribLocation(program, "vertexSpecular");
	if (vertexSpecular == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexSpecular' attribute in Vertex shader\n");
	}
	glVertexAttribPointer(
		vertexSpecular,
		3,
		GL_FLOAT,
		GL_FALSE,
		stride,
		(void*)((3 + 2 + 3 + 3) * 4)
	);

	glEnableVertexAttribArray(5);
	GLint vertexSpecularExponent = glGetAttribLocation(program, "vertexSpecularExponent");
	if (vertexSpecularExponent == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertexSpecularExponent' attribute in Vertex shader\n");
	}
	glVertexAttribPointer(
		vertexSpecularExponent,
		1,
		GL_FLOAT,
		GL_FALSE,
		stride,
		(void*)((3 + 2 + 3 + 3 + 3) * 4)
	);

	// if there is no normal, these values should not be loaded.
	if (normalID != 0)
	{
		glEnableVertexAttribArray(6);
		GLint tangent = glGetAttribLocation(program, "tangents");
		if (tangent == -1)
		{
			OutputDebugStringA("Error, cannot find 'tangents' attribute in Vertex shader\n");
		}
		glVertexAttribPointer(
			tangent,
			3,
			GL_FLOAT,
			GL_FALSE,
			stride,
			(void*)((3 + 2 + 3 + 3 + 3 + 1) * 4)
		);

		glEnableVertexAttribArray(7);
		GLint bitangent = glGetAttribLocation(program, "bitangents");
		if (bitangent == -1)
		{
			OutputDebugStringA("Error, cannot find 'bitangents' attribute in Vertex shader\n");
		}
		glVertexAttribPointer(
			bitangent,
			3,
			GL_FLOAT,
			GL_FALSE,
			stride,
			(void*)((3 + 2 + 3 + 3 + 3 + 1 + 3) * 4)
		);
	}

	//Activate the texture at position 0, the base texture that is
	glActiveTexture(GL_TEXTURE0);

	// bind texture and draw, different materials means different textures...
	for (int i = 0; i < startOfMat.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, materials[indices[i].mat].kdTex);
		glUniform1i(glGetUniformLocation(program, "tex"), 0);
		glDrawElements(GL_TRIANGLES, posIndices.size(), GL_UNSIGNED_INT, (void*)0);
	}

	// disable all used attribs.
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(6);
	glDisableVertexAttribArray(7);
}

void Mesh::loadNormalMap(GLuint program, const char * path)
{
	normalID = loadImage(path);

	for (int i = 0; i < indices.size(); i+=3) {
		glm::vec3 v0 = vertexPositions[indices[i].pos-1];
		glm::vec3 v1 = vertexPositions[indices[i+1].pos-1];
		glm::vec3 v2 = vertexPositions[indices[i+2].pos-1];

		glm::vec2 u0 = vertexUVs[indices[i].uv-1];
		glm::vec2 u1 = vertexUVs[indices[i+1].uv-1];
		glm::vec2 u2 = vertexUVs[indices[i+2].uv-1];

		glm::vec3 dV1 = v1 - v0;
		glm::vec3 dV2 = v2 - v0;

		glm::vec2 dU1 = u1 - u0;
		glm::vec2 dU2 = u2 - u0;

		float r = 1.0f / (dU1.x * dU2.y - dU1.y * dU2.x);
		glm::vec3 tangent = (dV1 * dU2.y - dV2 * dU1.y)*r;
		glm::vec3 bitangent = (dV2 * dU1.x - dV1 * dU2.x)*r;

		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
	}

	/*glGenBuffers(1, &tangentID);
	glBindBuffer(GL_ARRAY_BUFFER, tangentID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*tangents.size(), &tangents[0], GL_STATIC_DRAW);

	glGenBuffers(1, &bitangentID);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*bitangents.size(), &bitangents[0], GL_STATIC_DRAW);*/
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