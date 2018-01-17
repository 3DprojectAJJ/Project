#include <Windows.h>

#include <GL\glew.h>

#include <GLFW\glfw3.h>
#include <GL\GL.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

GLuint gShaderProgram = 0;


void CreateShaders()
{
	// Local buffer to store error string when compiling.
	char errorBuffer[1024];
	memset(errorBuffer, 0, 1024);
	GLint compileResult = 0;

	// Creating vertex shader name and store it in vs
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	// Opening the VertexShader.glsl file and insert it into a string
	std::ifstream shaderFile("VertexShader.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	const char* shaderTextPtr = shaderText.c_str();

	// Asking GL to use the string as a shader code source
	glShaderSource(vs, 1, &shaderTextPtr, nullptr);

	// Try to complie this shader source
	glCompileShader(vs);


	// Checking for compilation error
	compileResult = GL_FALSE;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		glGetShaderInfoLog(vs, 1024, nullptr, errorBuffer);
		OutputDebugStringA(errorBuffer);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	shaderFile.open("Fragment.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	shaderTextPtr = shaderText.c_str();

	glShaderSource(fs, 1, &shaderTextPtr, nullptr);

	glCompileShader(fs);

	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) 
	{
		glGetShaderInfoLog(fs, 1024, nullptr, errorBuffer);
		OutputDebugStringA(errorBuffer);
	}

	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);

	shaderFile.open("Geometry.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	shaderTextPtr = shaderText.c_str();

	glShaderSource(gs, 1, &shaderTextPtr, nullptr);

	glCompileShader(gs);

	compileResult = GL_FALSE;
	glGetShaderiv(gs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		glGetShaderInfoLog(gs, 1024, nullptr, errorBuffer);
		OutputDebugStringA(errorBuffer);
	}

	gShaderProgram = glCreateProgram();
	glAttachShader(gShaderProgram, fs);
	glAttachShader(gShaderProgram, vs);
	glAttachShader(gShaderProgram, gs);
	glLinkProgram(gShaderProgram);

	compileResult = GL_FALSE;
	glGetProgramiv(gShaderProgram, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		memset(errorBuffer, 0, 1024);
		glGetProgramInfoLog(gShaderProgram, 1024, nullptr, errorBuffer);
		OutputDebugStringA(errorBuffer);
	}

	glDetachShader(gShaderProgram, vs);
	glDetachShader(gShaderProgram, fs);
	glDetachShader(gShaderProgram, gs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);

	glUseProgram(gShaderProgram);
}

int main() 
{
=======
GLuint VertexArrayID;
GLuint Vertexbuffer;

void createTriangle()
{
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

int main() {


	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}


	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

																   // Open a window and create its OpenGL context
	GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
	window = glfwCreateWindow(1024, 768, "3DProj", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = true; // Needed in core profile

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize glew\n");
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);


	while (glfwWindowShouldClose(window) == 0)

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	createTriangle();

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{

		glfwPollEvents();
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
		glfwSwapBuffers(window);
	}


	return 0;
}