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
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"

#pragma warning(disable:4996)

int width = 1024;
int height = 768;

glm::mat4 Projection;
glm::mat4 Model;

GLFWwindow* Window;
GLuint VertexArrayID;
GLuint Vertexbuffer;
GLuint colorbuffer;
GLuint gShaderProgram;
GLuint MatrixID;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

Camera cam;

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

	glm::mat4 mvp = Projection*glm::mat4(glm::lookAt(cam.GetPos(), cam.GetPos() + cam.GetTarget(), cam.GetUp()))*Model;

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
}

void loadShaders()
{
	char buff[1024];
	memset(buff, 0, 1024);
	const char* shaderTextPtr;
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint compileResult = 0;

	std::ifstream shaderFile("VertexShader.glsl");
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


	shaderFile.open("FragmentShader.glsl");
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

	gShaderProgram = glCreateProgram();

	glAttachShader(gShaderProgram, FragmentShaderID);
	glAttachShader(gShaderProgram, VertexShaderID);
	glLinkProgram(gShaderProgram);
	compileResult = GL_FALSE;
	glGetProgramiv(gShaderProgram, GL_LINK_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		memset(buff, 0, 1024);
		glGetProgramInfoLog(gShaderProgram, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	glDetachShader(gShaderProgram, VertexShaderID);
	glDetachShader(gShaderProgram, FragmentShaderID);
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}

glm::mat4 makeMatrices()
{
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		cam.GetPos(), // Camera is at (4,3,3), in World Space
		cam.GetPos() + cam.GetTarget(), // and looks at the origin
		cam.GetUp()  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	MatrixID = glGetUniformLocation(gShaderProgram, "MVP");
	return mvp;
}

void movementToCamera(float dt)
{
	if (glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		cam.OnKeyboard(0, dt);
	}

	if (glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		cam.OnKeyboard(1, dt);
	}

	if (glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		cam.OnKeyboard(2, dt);
	}

	if (glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		cam.OnKeyboard(3, dt);
	}

	double xPos, yPos;
	glfwGetCursorPos(Window, &xPos, &yPos);
	
	cam.OnMouse(xPos, yPos, dt);
}

void guiWindow(bool * showAnotherWindow)
{
	ImGui_ImplGlfwGL3_NewFrame();

	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");                           // Some text (you can use a format string too)
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float as a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats as a color
		if (ImGui::Button("Another Window"))
			*showAnotherWindow ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)\nHorisontal: %f\nVertical: %f", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate,cam.GetAngles().x,cam.GetAngles().y);
	}

	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name the window.
	if (*showAnotherWindow)
	{
		ImGui::Begin("Another Window", showAnotherWindow);
		ImGui::Text("Hello from another window!");
		ImGui::End();
	}

	ImGui::Render();
}

void mainLoop()
{
	bool show_another_window = false;
	double time = glfwGetTime();
	float lastTime = 0;

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(Window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		time = glfwGetTime();
		float deltaTime = (float)time - lastTime;

		movementToCamera(deltaTime);

		Model *= glm::rotate(0.05f* (float)deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));

		render();

		guiWindow(&show_another_window);
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

	// Reads the shaders and makes a program out of them.
	loadShaders();

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
	cam.SetMousePos(glm::vec2(x, y));

	int tst = loadImage("tstTex.bmp");
	// does it need explanation?
	mainLoop();

	return 0;
}