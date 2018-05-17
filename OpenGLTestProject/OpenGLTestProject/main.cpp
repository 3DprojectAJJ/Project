#include "MeshHandler.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "ShaderHandler.h"
#include "Terrain.h"
#include "FrontBackRender.h"
#include "Particle.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"

#include <GLFW\glfw3.h>
#include <fstream>
#include <GLM\glm.hpp>
#include <GLM\matrix.hpp>
#include <GLM\\mat4x4.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM\gtx\transform.hpp>
#include <crtdbg.h>
#pragma warning(disable:4996)
#define GLM_ENABLE_EXPERIMENTAL
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

GLuint Vertexbuffer;
GLuint VertexArrayID;
GLFWwindow * Window;

GLFWwindow * init()
{
	// initializes glfw, this is needs to be done before doing anything related to glfw.
	glfwInit();

	// gives some "hints" for the window.
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

	// create the window
	GLFWwindow * window = glfwCreateWindow(1024, 720, "Test", NULL, NULL);
	// Makes the currently selected window into the recently created window
	glfwMakeContextCurrent(window);
	//makes sure that we have the experimental version of glew so that the debugger won't complain
	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
	}
	return window;
}
void initImgui(GLFWwindow * window)
{
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();
}

float distance(glm::vec3 p0, glm::vec3 p1)
{
	glm::vec3 vec = (p0 - p1) * (p0 - p1);
	return vec.x + vec.y + vec.z;
}

std::vector<glm::vec2> sort(std::vector<glm::vec3> *entities, glm::vec3 cameraPos, GLuint program)
{
	std::vector<glm::vec2> order;

	for (int i = 0; i < entities->size(); i++) {
		order.push_back(glm::vec2(distance(cameraPos, entities->at(i)), i));
	}
	for (int i = 0; i < entities->size() - 1; i++) {
		int index = i;
		for (int j = i + 1; j < entities->size(); j++) {
			if (order[i].x > order[j].x) {
				index = j;
			}
		}
		if (index != i) {
			iter_swap(order.begin() + i, order.begin() + index);
		}
	}
	return order;
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// initializes the window and GLEW
	Window = init();

	initImgui(Window);

	// Enables the automatic depth test, we won't have to worry about it no more
	glEnable(GL_DEPTH_TEST);

	// Creates necessary variables
	// Camera, Horizontal angle, Vertical Angle, Position
	Camera camera(180, -30, glm::vec3(0, 7, 10));
	// A mesh for a quad
	Mesh quad("quad.obj");
	// A mesh for a triangle
	Mesh triangle("basicTriangle.obj");
	// A handler of shaders that stores all our shaderprograms.
	ShaderHandler programs;
	// The framebuffer object that will be used for deferred rendering.
	Framebuffer fbo;

	FrontBackRender frontBackRender;

	// Adds three shaders that are used for the first renderpass
	if(!programs.addShader("VertexShader.glsl", GL_VERTEX_SHADER)){
	OutputDebugStringA("VertexShader failed to compile\n");
	}
	if(!programs.addShader("GeometryShader.glsl", GL_GEOMETRY_SHADER)){
	OutputDebugStringA("GeometryShader failed to compile\n");
	}
	if(!programs.addShader("FragmentShader.glsl", GL_FRAGMENT_SHADER)){
	OutputDebugStringA("FragmentShader failed to compile\n");
	}
	// creates a shaderprogram out of the earlier added shaders
	programs.createProgram();

	// Adds two shaders that are used for the second renderpass
	if(!programs.addShader("FBOVertexShader.glsl", GL_VERTEX_SHADER)){
		OutputDebugStringA("FBOVertexShader failed to compile\n");
	}
	if(!programs.addShader("FBOFragmentShader.glsl", GL_FRAGMENT_SHADER)){
		OutputDebugStringA("FBOFragmentShader failed to compile\n");
	}
	// creates a shaderprogram out of the earlier added shaders
	programs.createProgram();

	if (!programs.addShader("ParticleVertexShader.glsl", GL_VERTEX_SHADER)) {
		OutputDebugStringA("ParticleVertexShader failed to compile\n");
	}
	if (!programs.addShader("ParticleFragmentShader.glsl", GL_FRAGMENT_SHADER)){
	OutputDebugStringA("ParticleFragmentShader failed to compile\n");
	}
	// creates a shaderprogram out of the earlier added shaders
	programs.createProgram();

	fbo.getUniform(programs.getProgramID(1));

	// sets the quads matrix so that the mesh moves 5 floats to the right on the x-axis
	quad.setPosition(glm::vec3(-2.5, 5, 0));
	triangle.setPosition(glm::vec3(2.5, 5, 0));
	triangle.setRotation(glm::vec3(0, -45, 0));
	quad.setRotation(glm::vec3(0, 45, 0));
	// Reads the obj files so that the quad and triangle get their vertices
	
	Terrain terrain("heightmap.bmp");

	ParticleEmitter particle(glm::vec3(0, 4, 0), glm::vec3(0, 0.1, 0), 5, glm::vec3(128, 128, 128));

	std::vector<Entity*> entities;

	// Makes buffers so that the meshes becomes ready to be drawn.
	quad.makeBuffer(programs.getProgramID(0));
	triangle.makeBuffer(programs.getProgramID(0));

	entities.push_back(&quad);
	entities.push_back(&triangle);
	entities.push_back(&terrain);

	// Sets the initial cameraview value to the viewmatrix
	view = camera.viewMat();

	// Adds a value to the projectionmatrix
	projection = glm::perspective(
		glm::radians(45.0f), 
		(float)1280 / (float)720, 
		0.1f, 
		100.0f);

	// Make IDs for the viewmatrix and the projectionmatrix so we don't have to get these in the loop
	GLuint viewID = glGetUniformLocation(programs.getProgramID(0), "view");
	GLuint projID = glGetUniformLocation(programs.getProgramID(0), "projection");

	GLuint PviewID = glGetUniformLocation(programs.getProgramID(2), "view");
	GLuint PprojID = glGetUniformLocation(programs.getProgramID(2), "projection");

	// Initializes the fbo, so that it can be used in the draw passes
	fbo.init();

	// Necessary variables to track time while in the loop
	double dt = 0;
	double curr = 0;
	double last = 0;

	bool * showImguiWindow = new bool[fbo.nrOfTextures()];

	fbo.addLight(glm::vec3(0, 25, 15), glm::vec4(1, 0, 0.2f, 150));
	fbo.addLight(glm::vec3(15, 25, 0), glm::vec4(0, 0.2f, 1, 500));
	fbo.addLight(glm::vec3(0, 25, -15), glm::vec4(1, 0, 0.2f, 150));
	fbo.addLight(glm::vec3(-15, 25, 0), glm::vec4(0, 0.2f, 1, 500));

	do
	{
		ImGui_ImplGlfwGL3_NewFrame();
		// gets the current time
		curr = glfwGetTime();
		// gets the difference since last time call
		dt = curr - last;
		// saves the current time for next run
		last = curr;
		// sets the glfwtimer to the current time, it will continue to count from this value
		glfwSetTime(curr);
		
		// clears the scene and sets the background to the inputted color
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		// Binds fbo so that the program draws to fbo textures
		fbo.bindFBO();
		// clears the fbo textures from earlier draws.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Asks GL to use the first program (the one where we draw meshes to fbo texxtures)
		glUseProgram(programs.getProgramID(0));

		// Set uniform variables of view and projection matrices in shader program to the following
		glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projID, 1, GL_FALSE, &projection[0][0]);

		glUniformMatrix4fv(PviewID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(PprojID, 1, GL_FALSE, &projection[0][0]);

		// Drawcall
		//quad.draw(programs.getProgramID(0));
		//frontBackRender.render(&entities, camera.getPos(), programs.getProgramID(0));
		std::vector<glm::vec3> pos;
		for (int i = 0; i < entities.size(); i++) {
			pos.push_back(entities.at(i)->getPosition());
		}
		std::vector<glm::vec2> order = sort(&pos, camera.getPos(), programs.getProgramID(0));

		for (int i = 0; i < entities.size(); i++) {
			entities.at(i)->draw(programs.getProgramID(0));
		}
		particle.update(programs.getProgramID(2), camera.getPos(), dt);





		//unbinds the fbo, we now draw to the window or default fbo instead
		fbo.unbindFBO(1024, 720);

		// clears the window from earlier draws.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draws the quad to the window
		fbo.draw(programs.getProgramID(1));


		{
			ImGui::SetWindowSize(ImVec2(480, 220));
			for (int i = 0; i < fbo.nrOfTextures() - 1; i++) {
				if (ImGui::ImageButton((GLuint*)fbo.getTexID()[i], ImVec2(102, 77), ImVec2(0, 1), ImVec2(1, 0)))
				{
					showImguiWindow[i] = true;
				}
				if (i != 3) {
					ImGui::SameLine();
				}
			}
			if (ImGui::Button("Options"))
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Camera Position: %.2f x, %.2f y, %.2f z", camera.getPos().x, camera.getPos().y, camera.getPos().z);
		}

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		// Swaps buffers so that the window actually shows the new scene
		glfwSwapBuffers(Window);
		// polls events so we can react to them.
		glfwPollEvents();

		// Updates the camera position and view direction
		camera.update(Window, dt);
		view = camera.viewMat();

		// if the user presses the "x" on the window or the esc key, the loop stops.
	} while (glfwGetKey(Window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(Window) == 0);

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	//The window closes
	glfwDestroyWindow(Window);
	//makes sure that glfw does not leave memoryleaks
	glfwTerminate();
	delete[] showImguiWindow;
	return 0;
}