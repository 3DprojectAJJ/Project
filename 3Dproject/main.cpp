#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <windows.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

GLFWwindow* Window;
GLuint VertexArrayID;
GLuint Vertexbuffer;
GLuint gShaderProgram;

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

void render()
{
	glClearColor(0.0f, 0.0f, 0.9f, 0.0f);
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
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
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

int main()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

																   // Open a window and create its OpenGL context
	Window = glfwCreateWindow(1024, 768, "3DProject", NULL, NULL);
	if (Window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(Window); // Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(Window, GLFW_STICKY_KEYS, GL_TRUE);
	loadShaders();
	createTriangle();
	do {
		// Draw nothing, see you in tutorial 2 !
		render();
		// Swap buffers
		glfwSwapBuffers(Window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(Window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(Window) == 0);

	return 0;
}