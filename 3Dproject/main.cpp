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
GLuint Vertexbuffer;
GLuint colorbuffer;
GLuint gShaderProgram;
GLuint quad_programID;
GLuint matrixIDModel;
GLuint matrixIDView;
GLuint matrixIDProjection;

GLuint texID[4];

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

Camera Cam;
FBO Fbo;
GBuffer Gbo;

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
	// Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat gVertexBufferData[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &Vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, Vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData, GL_STATIC_DRAW);

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

void render()
{
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgram);

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
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the Cube!
	glDrawArrays(GL_TRIANGLES, 0, 3*12); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);

	View = glm::mat4(glm::lookAt(Cam.GetPos(), Cam.GetPos() + Cam.GetTarget(), Cam.GetUp()));

	glUniformMatrix4fv(matrixIDModel, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(matrixIDView, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(matrixIDProjection, 1, GL_FALSE, &Projection[0][0]);

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

	matrixIDModel = glGetUniformLocation(gShaderProgram, "Model");
	matrixIDView = glGetUniformLocation(gShaderProgram, "View");
	matrixIDProjection = glGetUniformLocation(gShaderProgram, "Projection");

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
		ImGui::SliderFloat("MoveSpeed", &moveSpeed, 1.0f, 20.0f);
		ImGui::End();
	}
	ImGui::Render();

}

/*void createFrameBuffer() {

	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// The texture we're going to render to
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

								   // Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// The fullscreen quad's FBO
	GLuint quad_VertexArrayID;
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);

	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

}*/

void mainLoop()
{
	bool showImGuiWindow[4] = { false };
	double time = glfwGetTime();
	float lastTime = 0;

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(Window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		time = glfwGetTime();
		float deltaTime = (float)time - lastTime;

		movementToCamera(deltaTime);

		Model *= glm::rotate(0.5f* (float)deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
		/*glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0, 0, width, height);*/
		Fbo.BindFBO();

		render();

		/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);*/
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

	// Creates the vertices and color for the cube, binds to layout locations
	createCube();

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

	//int tst = loadImage("tstTex.bmp");

	quad_programID = loadShadersFBO("vertexFBO.glsl", "fragmentFBO.glsl");
	texID[0] = glGetUniformLocation(quad_programID, "colorTexture");
	texID[1] = glGetUniformLocation(quad_programID, "normalTexture");
	texID[2] = glGetUniformLocation(quad_programID, "positionTexture");
	texID[3] = glGetUniformLocation(quad_programID, "depthTexture");

	// does it need explanation?
	mainLoop();

	return 0;
}