#include "Mesh.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "ShaderHandler.h"
#include "Terrain.h"
#include "FrontBackRender.h"
#include "Particle.h"
#include "Mouse.h"

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

GLuint configShaderMatrices(Framebuffer *fbo, ShaderHandler * programs, std::vector<Entity*> entities)
{
	fbo->shadowInit();
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f, 4.0f, 3.0f), glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMat = lightProjection * lightView;

	fbo->setLightSpaceMatrix(lightSpaceMat);

	if (!programs->addShader("ShadowVertexShader.glsl", GL_VERTEX_SHADER)) {
		OutputDebugStringA("ShadowVertexShader failed to compile\n");
	}
	if (!programs->addShader("ShadowFragmentShader.glsl", GL_FRAGMENT_SHADER)) {
		OutputDebugStringA("ShadowFragmentShader failed to com,pile\n");
	}

	programs->createProgram();

	glUseProgram(programs->getProgramID(programs->getSize() - 1));
	glUniformMatrix4fv(glGetUniformLocation(programs->getProgramID(programs->getSize() - 1), "lightSpaceMat"), 1, GL_FALSE, &lightSpaceMat[0][0]);

	fbo->bindShadowFBO();
	glClear(GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < entities.size(); i++)
	{
		entities[i]->draw(programs->getProgramID(programs->getSize() - 1));
	}
	fbo->unbindFBO(1024, 720);
	return fbo->getDepthMap();
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
	Camera camera(180, -20, glm::vec3(0, 3, 7));
	// A mesh for a quad
	Mesh quad("quad.obj");
	Mesh quad1("quad.obj");
	Mesh quad2("quad.obj");
	Mesh quad3("quad.obj");
	// A mesh for a triangle
	Mesh triangle("basicTriangle.obj");
	// A handler of shaders that stores all our shaderprograms.
	ShaderHandler programs;
	// The framebuffer object that will be used for deferred rendering.
	Framebuffer fbo;
	Mouse mouse;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	bool orbit = false;
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

	if (!programs.addShader("ParticleVertexShader.glsl", GL_VERTEX_SHADER)) {
		OutputDebugStringA("ParticleVertexShader failed to compile\n");
	}
	if (!programs.addShader("ParticleFragmentShader.glsl", GL_FRAGMENT_SHADER)) {
		OutputDebugStringA("ParticleFragmentShader failed to compile\n");
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

	fbo.getUniform(programs.getProgramID(2));

	// sets the quads matrix so that the mesh moves 5 floats to the right on the x-axis
	quad.setPosition(glm::vec3(-4, 1, -3));
	quad.setRotation(glm::vec3(0, 0, 0));
	quad1.setPosition(glm::vec3(-2, 1, -3));
	quad1.setRotation(glm::vec3(0, 0, 0));
	quad2.setPosition(glm::vec3(0, 1, -3));
	quad2.setRotation(glm::vec3(0, 0, 0));
	quad3.setPosition(glm::vec3(2, 1, -3));
	quad3.setRotation(glm::vec3(0, 0, 0));
	triangle.setPosition(glm::vec3(4, 1, -1));
	triangle.setRotation(glm::vec3(0, -45, 0));

	Terrain terrain("heightmap.bmp");

	ParticleEmitter particle(glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 2, glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(1.0f, 0.8f, 0.15f));
	std::vector<Entity*> entities;

	// Makes buffers so that the meshes becomes ready to be drawn.
	quad.loadNormalMap(programs.getProgramID(0), "normal.bmp");
	quad.makeBuffer(programs.getProgramID(0));
	quad1.loadNormalMap(programs.getProgramID(0), "normal.bmp");
	quad1.makeBuffer(programs.getProgramID(0));
	quad2.loadNormalMap(programs.getProgramID(0), "normal.bmp");
	quad2.makeBuffer(programs.getProgramID(0));
	quad3.loadNormalMap(programs.getProgramID(0), "normal.bmp");
	quad3.makeBuffer(programs.getProgramID(0));


	triangle.makeBuffer(programs.getProgramID(0));
	//terrain.loadNormalMap(programs.getProgramID(0), "groundNormal.bmp");

	entities.push_back(&quad);
	entities.push_back(&quad1);
	entities.push_back(&quad2);
	entities.push_back(&quad3);
	entities.push_back(&triangle);
	entities.push_back(&terrain);

	mouse.loadEntity(&quad);
	mouse.loadEntity(&quad1);
	mouse.loadEntity(&quad2);
	mouse.loadEntity(&quad3);
	mouse.loadEntity(&triangle);

	// Sets the initial cameraview value to the viewmatrix
	view = camera.viewMat();

	// Adds a value to the projectionmatrix
	projection = glm::perspective(glm::radians(45.0f), (float)1280 / (float)720, 0.1f, 100.0f);

	// Make IDs for the viewmatrix and the projectionmatrix so we don't have to get these in the loop

	GLuint viewID = glGetUniformLocation(programs.getProgramID(0), "view");
	GLuint projID = glGetUniformLocation(programs.getProgramID(0), "projection");

	GLuint pViewID = glGetUniformLocation(programs.getProgramID(1), "pView");
	GLuint pProjID = glGetUniformLocation(programs.getProgramID(1), "pProjection");

	// Initializes the fbo, so that it can be used in the draw passes
	fbo.init();

	// Necessary variables to track time while in the loop
	double dt = 0;
	double curr = 0;
	double last = 0;

	bool * showImguiWindow = new bool[fbo.nrOfTextures()];

	fbo.addLight(glm::vec3(0, 2.0f, 2.0f), glm::vec4(1, 0.5f, 0.1f, 50));

	configShaderMatrices(&fbo, &programs, entities);


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

		// Drawcall
		frontBackRender.render(&entities, camera.getPos(), programs.getProgramID(0));
		


		glUseProgram(programs.getProgramID(1));

		glUniformMatrix4fv(pViewID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(pProjID, 1, GL_FALSE, &projection[0][0]);

		particle.update(programs.getProgramID(1), &camera, dt);


		//unbinds the fbo, we now draw to the window or default fbo instead
		fbo.unbindFBO(1024, 720);

		mouse.update(Window, view, projection);

		// clears the window from earlier draws.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draws the quad to the window
		fbo.draw(programs.getProgramID(2), camera.getPos());


		{		
			ImGui::SetWindowSize(ImVec2(400, 420));
			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::ImageButton((GLuint*)fbo.getDepthMap(), ImVec2(102, 77), ImVec2(0, 1), ImVec2(1, 0));
			for (int i = 0; i < fbo.nrOfTextures() - 1; i++) {
				if (ImGui::ImageButton((GLuint*)fbo.getTexID()[i], ImVec2(102, 77), ImVec2(0, 1), ImVec2(1, 0)))
				{
					showImguiWindow[i] = true;
				}
				if (i != 2) {
					ImGui::SameLine();
				}
			}

			ImGui::NewLine();
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
		if (glfwGetKey(Window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			orbit = !orbit;
		}
		if (orbit == false) {

			float h = 0;

			float x = camera.getPos().x;

			float z = camera.getPos().z;

			int index = ((int)(x + 25) * 4) + ((int)(z + 25) * 4) * terrain.getWidth();

			if (x >= -25 & x <= 25 & z >= -25 & z <= 25)

			{
				float y1 = terrain.getHeight((unsigned int)index);

				float y2 = terrain.getHeight((unsigned int)index + 1);

				float y3 = terrain.getHeight((unsigned int)index + terrain.getWidth());

				float y4 = terrain.getHeight((unsigned int)index + terrain.getWidth() + 1);

				float xfrac = x - floor(x);

				float zfrac = z - floor(z);

				float yleft = y1 + (y2 - y1) * xfrac;

				float yright = y3 + (y4 - y3) * xfrac;

				h = yleft + (yright - yleft) * zfrac + 1;

			}
			camera.setCameraPos(glm::vec3(x, h, z));
		}

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