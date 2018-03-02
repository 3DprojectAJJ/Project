#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <GLM\mat4x4.hpp>
#include <GLM\glm.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM\gtx\transform.hpp>

#include <windows.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "Camera.h"
#include "FBO.h"
#include "GBuffer.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"
#include "obj.h"

#pragma warning(disable:4996)

int width = 1024;
int height = 768;
float mouseSpeed = 1.0f;
float moveSpeed = 5.0f;

glm::mat4 Projection;
glm::mat4 Model;
glm::mat4 View;

GLFWwindow* Window;
GLuint VertexArrayID;
GLuint eVertexArrayID;
GLuint Vertexbuffer;
GLuint eVertexbuffer;
GLuint colorbuffer;
GLuint texbuffer;
GLuint etexbuffer;
GLuint normalbuffer;
GLuint gShaderProgram;
GLuint terrainProgram;
GLuint quad_programID;
GLuint matrixIDModel;
GLuint matrixIDView;
GLuint matrixIDProjection;
GLuint vectorCameraPos;
GLuint elementbuffer;

GLuint cubeTexID;
GLuint tstTexture;
GLuint terrainTex;
GLuint texID[4];
GLuint texi;

glm::vec3* heightmap;
int heightmapWidth, heightmapHeight;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

std::vector<unsigned int> indices;

bool orbit = true;

Camera Cam;
FBO Fbo;
GBuffer Gbo;
Obj obj;

bool optionWindow = false;

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 tex;
	Vertex() {}

	Vertex(glm::vec3 otherPos, glm::vec2 otherTex)
	{
		pos = otherPos;
		tex = otherTex;
	}
};



GLuint loadImage(const char * imagepath) {

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
		imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
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

void createHeightMap()
{
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int imageSize;

	unsigned char * data;

	FILE * file = fopen("heightmap.bmp", "rb");
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

	heightmap = new glm::vec3[width*height];
	glm::vec2* heightmaptex = new glm::vec2[width*height];

	heightmapWidth = width;
	heightmapHeight = height;

	float vertexWidth = 0.25f;
	float heightScaling = 5;

	for (unsigned int i = 0; i < height; i++) {
		for (unsigned int j = 0; j < width; j++) {
			heightmap[j + i * height] = glm::vec3((float)(j * vertexWidth) - (float)(width * (vertexWidth * 0.5f)), (float)data[(j + i * height) * 3] * heightScaling / 255, (float)(i * vertexWidth) - (float)(height * (vertexWidth * 0.5f)));
			heightmaptex[j + i * height] = glm::vec2(j/width, i/height);
		}
	}

	indices.resize((width-1) * (height -1) * 6);

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

	glGenVertexArrays(1, &eVertexArrayID);
	glBindVertexArray(eVertexArrayID);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &eVertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, eVertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, width * height * sizeof(glm::vec3), &heightmap[0], GL_STATIC_DRAW);

	glGenBuffers(1, &etexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, etexbuffer);
	glBufferData(GL_ARRAY_BUFFER, width * height * sizeof(glm::vec2), &heightmaptex[0], GL_STATIC_DRAW);

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

}

int initGLFW()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

																   // Open a window and create its OpenGL context
	Window = glfwCreateWindow(width, height, "3DProject", NULL, NULL);
	if (Window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(Window); // Initialize GLEW
	return 0;
}

void createTriangle()
{

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	static const GLfloat gVertexBufferData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &Vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, Vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData, GL_STATIC_DRAW);
}

void createCube()
{
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &Vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, Vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, obj.getVertices().size()*sizeof(glm::vec3), &obj.getVertices()[0], GL_STATIC_DRAW);

	// One color for each vertex. They were generated randomly.
	static const GLfloat g_color_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
}

void createObjectWithTexture()
{
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &Vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, Vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, obj.getVertices().size() * sizeof(glm::vec3), &obj.getVertices()[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &texbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
	glBufferData(GL_ARRAY_BUFFER, obj.getUVs().size() * sizeof(glm::vec2), &obj.getUVs()[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, obj.getNormals().size() * sizeof(glm::vec3), &obj.getNormals()[0], GL_STATIC_DRAW);
}

void render()
{
	glUseProgram(gShaderProgram);

	glUniformMatrix4fv(matrixIDModel, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(matrixIDView, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(matrixIDProjection, 1, GL_FALSE, &Projection[0][0]);
	glUniform3fv(vectorCameraPos, 1, &Cam.GetPos()[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tstTexture);
	glUniform1i(glGetUniformLocation(gShaderProgram, "tex"),0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Vertexbuffer);
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
	GLint loc = glGetAttribLocation(gShaderProgram, "vertexUV");
	glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the Cube!
	glDrawArrays(GL_TRIANGLES, 0,obj.getVertices().size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void terrainRender() {

	glUseProgram(terrainProgram);

	glUniformMatrix4fv(matrixIDView, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(matrixIDProjection, 1, GL_FALSE, &Projection[0][0]);
	glUniform3fv(vectorCameraPos, 1, &Cam.GetPos()[0]);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texi);
	glUniform1i(glGetUniformLocation(terrainProgram, "tex"), 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, eVertexbuffer);
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
	GLint loc = glGetAttribLocation(gShaderProgram, "vertexUV");
	glBindBuffer(GL_ARRAY_BUFFER, etexbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
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


void differedRender() {
	// Use our shader
	glUseProgram(quad_programID);
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < 4; i++)
	{
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, Fbo.GetTexID()[i]);

		glUniform1i(texID[i], i);
	}

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Fbo.GetQuadID());
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	glDisableVertexAttribArray(0);
}

GLuint loadShaders(char const* vertex, char const* fragment, char const* geometry)
{
	char buff[1024];
	memset(buff, 0, 1024);
	const char* shaderTextPtr;
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint ShaderID;

	GLint compileResult = 0;

	std::ifstream shaderFile(vertex);
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	shaderTextPtr = shaderText.c_str();

	glShaderSource(VertexShaderID, 1, &shaderTextPtr, nullptr);

	glCompileShader(VertexShaderID);

	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		glGetShaderInfoLog(VertexShaderID, 1024, nullptr, buff);

		OutputDebugStringA(buff);
	}


	shaderFile.open(fragment);
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	shaderTextPtr = shaderText.c_str();

	glShaderSource(FragmentShaderID, 1, &shaderTextPtr, nullptr);
	glCompileShader(FragmentShaderID);

	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		memset(buff, 0, 1024);
		glGetShaderInfoLog(FragmentShaderID, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	shaderFile.open(geometry);
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	shaderTextPtr = shaderText.c_str();

	glShaderSource(GeometryShaderID, 1, &shaderTextPtr, nullptr);
	glCompileShader(GeometryShaderID);

	glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		memset(buff, 0, 1024);
		glGetShaderInfoLog(GeometryShaderID, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	ShaderID = glCreateProgram();

	glAttachShader(ShaderID, FragmentShaderID);
	glAttachShader(ShaderID, VertexShaderID);
	glAttachShader(ShaderID, GeometryShaderID);
	glLinkProgram(ShaderID);

	compileResult = GL_FALSE;
	glGetProgramiv(ShaderID, GL_LINK_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		memset(buff, 0, 1024);
		glGetProgramInfoLog(ShaderID, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	glDetachShader(ShaderID, VertexShaderID);
	glDetachShader(ShaderID, FragmentShaderID);
	glDetachShader(ShaderID, GeometryShaderID);
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	glDeleteShader(GeometryShaderID);

	return ShaderID;
}

GLuint loadShadersFBO(char const* vertex, char const* fragment)
{
	char buff[1024];
	memset(buff, 0, 1024);
	const char* shaderTextPtr;
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint ShaderID;

	GLint compileResult = 0;

	std::ifstream shaderFile(vertex);
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	shaderTextPtr = shaderText.c_str();

	glShaderSource(VertexShaderID, 1, &shaderTextPtr, nullptr);

	glCompileShader(VertexShaderID);

	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		glGetShaderInfoLog(VertexShaderID, 1024, nullptr, buff);

		OutputDebugStringA(buff);
	}


	shaderFile.open(fragment);
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	shaderTextPtr = shaderText.c_str();

	glShaderSource(FragmentShaderID, 1, &shaderTextPtr, nullptr);
	glCompileShader(FragmentShaderID);

	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		memset(buff, 0, 1024);
		glGetShaderInfoLog(FragmentShaderID, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	ShaderID = glCreateProgram();

	glAttachShader(ShaderID, FragmentShaderID);
	glAttachShader(ShaderID, VertexShaderID);
	glLinkProgram(ShaderID);
	compileResult = GL_FALSE;
	glGetProgramiv(ShaderID, GL_LINK_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		memset(buff, 0, 1024);
		glGetProgramInfoLog(ShaderID, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	glDetachShader(ShaderID, VertexShaderID);
	glDetachShader(ShaderID, FragmentShaderID);
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ShaderID;
}

void makeMatrices()
{
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	View = glm::lookAt(
		Cam.GetPos(), // Camera is at (4,3,3), in World Space
		Cam.GetPos() + Cam.GetTarget(), // and looks at the origin
		Cam.GetUp()  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	//glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around
}

void movementToCamera(float dt)
{
	if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Cam.OnKeyboard(0, dt, moveSpeed);
	}

	if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Cam.OnKeyboard(1, dt, moveSpeed);
	}

	double xPos, yPos;
	glfwGetCursorPos(Window, &xPos, &yPos);

	glfwSetInputMode(Window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		Cam.OnMouse(xPos, yPos, dt, mouseSpeed);
	}
	else {
		Cam.SetMousePos(glm::vec2(xPos, yPos));
	}

	if (orbit == false) {
		float h = 0;
		float x = Cam.GetPos().x;
		float z = Cam.GetPos().z;
		int index = ((int)(x + 25) * 4) + ((int)(z + 25) * 4) * heightmapWidth;
		if (x >= -25 & x <= 25 & z >= -25 & z <= 25)
		{
			float y1 = heightmap[index].y;

			float y2 = heightmap[index + 1].y;

			float y3 = heightmap[index + heightmapWidth].y;

			float y4 = heightmap[index + heightmapWidth + 1].y;


			float xfrac = x - floor(x);

			float zfrac = z - floor(z);



			float yleft = y1 + (y2 - y1) * xfrac;

			float yright = y3 + (y4 - y3) * xfrac;



			h = yleft + (yright - yleft) * zfrac + 1;
		}
		Cam.setCamPos(glm::vec3(x, h, z));
	}
}

void guiWindow(bool showImguiWindow[])
{
	ImGui_ImplGlfwGL3_NewFrame();

	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	{
		ImGui::SetWindowSize(ImVec2(480, 220));
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats as a color
		for (int i = 0; i < 4; i++) {
			if (ImGui::ImageButton((GLuint*)Fbo.GetTexID()[i], ImVec2(102, 77), ImVec2(0, 1), ImVec2(1, 0)))
			{
				showImguiWindow[i] = true;
			}
			if (i != 3) {
				ImGui::SameLine();
			}
		}
		if (ImGui::Button("Options"))
			optionWindow ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)\nHorisontal: %f\nVertical: %f", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate, Cam.GetAngles().x, Cam.GetAngles().y);
		ImGui::Text("Camera Position: %.2f x, %.2f y, %.2f z", Cam.GetPos().x, Cam.GetPos().y, Cam.GetPos().z);
	}
	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name the window.

	for (int i = 0; i < 4; i++)
	{
		if (showImguiWindow[i])
		{
			ImGui::Begin("Texture", &showImguiWindow[i]);
			ImGui::Image((GLuint*)Fbo.GetTexID()[i], ImVec2(1024, 768), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
		}
	}


	if (optionWindow)
	{
		ImGui::Begin("options", &optionWindow);
		ImGui::Text("These are the options");
		ImGui::SliderFloat("MouseSpeed", &mouseSpeed, 0.5f, 10.0f);
		ImGui::SliderFloat("MoveSpeed", &moveSpeed, 1.0f, 100.0f);
		if (ImGui::Button("Read Heightmap")) {
			createHeightMap();
		}
		if (orbit == true) {
			if (ImGui::Button("Walk")) {
				orbit = false;
			}
		}
		else {
			if (ImGui::Button("Orbit")) {
				orbit = true;
			}
		}
		ImGui::End();
	}
	ImGui::Render();

}

void mainLoop()
{
	bool showImGuiWindow[4] = { false };
	double time = glfwGetTime();
	float lastTime = 0;
	float timer = 5;

	Model *= glm::translate(glm::vec3(0, 3, 0));

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(Window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		time = glfwGetTime();
		float deltaTime = (float)time - lastTime;

		movementToCamera(deltaTime);
		Model *= glm::rotate(0.1f* (float)deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
		View = glm::mat4(glm::lookAt(Cam.GetPos(), Cam.GetPos() + Cam.GetTarget(), Cam.GetUp()));

		Fbo.BindFBO();
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		matrixIDModel = glGetUniformLocation(gShaderProgram, "Model");
		matrixIDView = glGetUniformLocation(gShaderProgram, "View");
		matrixIDProjection = glGetUniformLocation(gShaderProgram, "Projection");
		render();

		matrixIDModel = glGetUniformLocation(terrainProgram, "Model");
		matrixIDView = glGetUniformLocation(terrainProgram, "View");
		matrixIDProjection = glGetUniformLocation(terrainProgram, "Projection");
		terrainRender();

		Fbo.UnbindFBO(width, height);

		differedRender();

		guiWindow(showImGuiWindow);
		// Swap buffers
		glfwSwapBuffers(Window);
		glfwPollEvents();
		lastTime = (float)time;
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(Window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(Window) == 0);
}

int main()
{
	obj.readOBJFile("Furret.obj");
	if (initGLFW() == -1)
	{
		return -1;
	}

	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	//imguiInit();
	ImGui_ImplGlfwGL3_Init(Window, true);

	// initialize/create framebufferobject
	//createFrameBuffer();
	Fbo.Init();
	// Reads the shaders and makes a program out of them.
	gShaderProgram = loadShaders("VertexShader.glsl","FragmentShader.glsl", "GeometryShader.glsl");
	quad_programID = loadShadersFBO("vertexFBO.glsl", "fragmentFBO.glsl");
	terrainProgram = loadShaders("tVertex.glsl", "tFragment.glsl", "tGeometry.glsl");

	// Creates the vertices and color for the cube, binds to layout locations
	//createCube();
	createObjectWithTexture();
	createHeightMap();
	// sets values to world, view and projection matrices and gets the uniform "WVP"s id.
	makeMatrices();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	//Make sure we dont change the camera direction when program starts
	double x, y;
	glfwGetCursorPos(Window, &x, &y);
	Cam.SetMousePos(glm::vec2(x, y));

	tstTexture = loadImage(obj.getTexturePath());
	texi = loadImage("red.bmp");

	quad_programID = loadShadersFBO("vertexFBO.glsl", "fragmentFBO.glsl");
	texID[0] = glGetUniformLocation(quad_programID, "colorTexture");
	texID[1] = glGetUniformLocation(quad_programID, "normalTexture");
	texID[2] = glGetUniformLocation(quad_programID, "positionTexture");
	texID[3] = glGetUniformLocation(quad_programID, "depthTexture");

	cubeTexID = glGetUniformLocation(gShaderProgram, "tex");


	// does it need explanation?

	mainLoop();

	return 0;
}