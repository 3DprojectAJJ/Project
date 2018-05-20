#ifndef MOUSE_H
#define MOUSE_H

#include "Entity.h"
#include "ShaderHandler.h"
#include "Framebuffer.h"

#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"

#include <iostream>
#include <GLM\vec3.hpp>
#include <GLM\vec2.hpp>
#include <GLFW\glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM\gtx\transform.hpp>

class Mouse {
private:
	ShaderHandler program;
	Framebuffer framebuffer;

	int pickedIndex;

	std::vector<Entity*> entities;

public:
	Mouse();

	void update(GLFWwindow * window, glm::mat4 view, glm::mat4 projection);

	void loadEntity(Entity *object);

};

#endif // !MOUSE_H
