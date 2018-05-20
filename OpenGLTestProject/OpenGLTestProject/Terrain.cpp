#include "Terrain.h"

GLuint Terrain::loadImage(const char * imagepath)
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

	delete data;
	return textureID;
}

Terrain::Terrain(const char*filepath)
{
	heightScaling = 4;
	readHeightMap(filepath);
	tex = loadImage("red.bmp");
	world = glm::mat4(1.0f);
}

void Terrain::readHeightMap(const char* filepath)
{
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;

	unsigned char * data;

	FILE * file = fopen(filepath, "rb");
	if (!file)
	{
		printf("Image could not be opened\n");
	}

	if (fread(header, 1, 54, file) != 54)
	{
		printf("Not a correct BMP file\n");
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
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

	float vertexWidth = 0.25f;

	for (unsigned int i = 0; i < height; i++) {
		for (unsigned int j = 0; j < width; j++) {
			heightmap.push_back(glm::vec3((float)(j * vertexWidth) - (float)(width * (vertexWidth * 0.5f)), (float)data[(j + i * height) * 3] * heightScaling / 255, (float)(i * vertexWidth) - (float)(height * (vertexWidth * 0.5f))));
			heightmaptex.push_back(glm::vec2((float)j / width, (float)i / height));
		}
	}

	indices.resize((width - 1) * (height - 1) * 6);

	unsigned int index = 0; // Index in the index buffer
	for (unsigned int i = 0; i < height - 1; ++i)
	{
		for (unsigned int j = 0; j < width - 1; ++j)
		{
			int vertexIndex = (i * width) + j;
			// Top triangle (T0)
			indices[index++] = vertexIndex;                           // V0
			indices[index++] = vertexIndex + width + 1;        // V3
			indices[index++] = vertexIndex + 1;                       // V1
																	  // Bottom triangle (T1)
			indices[index++] = vertexIndex;                           // V0
			indices[index++] = vertexIndex + width;            // V2
			indices[index++] = vertexIndex + width + 1;        // V3
		}
	}

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexBuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, heightmap.size() * sizeof(glm::vec3), &heightmap[0], GL_STATIC_DRAW);

	glGenBuffers(1, &texbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
	glBufferData(GL_ARRAY_BUFFER, heightmap.size() * sizeof(glm::vec2), &heightmaptex[0], GL_STATIC_DRAW);

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	delete data;
}

void Terrain::draw(GLuint program)
{
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &world[0][0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalID);

	glUniform1i(glGetUniformLocation(program, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(program, "useNormalMap"), normalID);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glEnableVertexAttribArray(6);
	glBindBuffer(GL_ARRAY_BUFFER, tangentID);
	GLint tangent = glGetAttribLocation(program, "tangents");

	if (tangent == -1)
	{
		OutputDebugStringA("Error, cannot find 'tangents attribute in Vertex shader\n");
		//return;
	}

	glVertexAttribPointer(
		tangent,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(7);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentID);
	GLint bitangent = glGetAttribLocation(program, "bitangents");

	if (tangent == -1)
	{
		OutputDebugStringA("Error, cannot find 'bitangents' attribute in Vertex shader\n");
		//return;
	}

	glVertexAttribPointer(
		bitangent,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
