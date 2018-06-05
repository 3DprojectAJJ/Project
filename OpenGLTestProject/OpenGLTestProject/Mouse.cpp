#include "Mouse.h"

Mouse::Mouse()
{
	pickedIndex = -1;

	program.addShader("MouseVertexShader.glsl", GL_VERTEX_SHADER);
	program.addShader("MouseFragmentShader.glsl", GL_FRAGMENT_SHADER);

	program.createProgram();

	// Adds two shaders that are used for the second renderpass
	if (!program.addShader("FBOVertexShader.glsl", GL_VERTEX_SHADER)) {
		OutputDebugStringA("FBOVertexShader failed to compile\n");
	}
	if (!program.addShader("FBOFragmentShader.glsl", GL_FRAGMENT_SHADER)) {
		OutputDebugStringA("FBOFragmentShader failed to compile\n");
	}
	// creates a shaderprogram out of the earlier added shaders
	program.createProgram();

	framebuffer.getUniform(program.getProgramID(1));

	framebuffer.init();
}

void Mouse::update(GLFWwindow * window, glm::mat4 view, glm::mat4 projection)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !ImGui::IsAnyItemHovered()) {
		framebuffer.bindFBO();

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program.getProgramID(0));
		glUniformMatrix4fv(glGetUniformLocation(program.getProgramID(0), "mview"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program.getProgramID(0), "mprojection"), 1, GL_FALSE, &projection[0][0]);
		for (int i = 0; i < entities.size(); i++) {
			glUniform1f(glGetUniformLocation(program.getProgramID(0), "index"), (float)(i + 1)/255.0f);

			entities[i]->draw(program.getProgramID(0));
		}

		double xPos, yPos;
		int width, height;

		unsigned char data[4];

		glfwGetCursorPos(window, &xPos, &yPos);
		glfwGetWindowSize(window, &width, &height);

		glFlush();
		glFinish();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (xPos >= 0 && yPos >= 0 && xPos <= width && yPos <= height) {
			glReadPixels(xPos, height - yPos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
			if(data[0] != 0)
			pickedIndex = (int)(data[0] - 1);
		}
		framebuffer.unbindFBO(1024, 720);
	}

	{
		if (ImGui::ImageButton((GLuint*)framebuffer.getTexID()[0], ImVec2(102, 77), ImVec2(0, 1), ImVec2(1, 0)))
		{
			ImGui::NewLine;
		}
		if (pickedIndex != -1) {
			ImGui::Text("Picked Index: %i", pickedIndex);
			float data[3] = { entities[pickedIndex]->getPosition().x, entities[pickedIndex]->getPosition().y, entities[pickedIndex]->getPosition().z };
			if (ImGui::InputFloat3("Position", data, 2)) {
				entities[pickedIndex]->setPosition(glm::vec3(data[0], data[1], data[2]));
			}

			ImGui::NewLine;

			data[0] = entities[pickedIndex]->getRotation().x;
			data[1] = entities[pickedIndex]->getRotation().y;
			data[2] = entities[pickedIndex]->getRotation().z;

			if (ImGui::InputFloat3("Rotation", data, 2)) {
				entities[pickedIndex]->setRotation(glm::vec3(data[0], data[1], data[2]));
			}

			ImGui::NewLine;

			data[0] = entities[pickedIndex]->getScale().x;
			data[1] = entities[pickedIndex]->getScale().y;
			data[2] = entities[pickedIndex]->getScale().z;

			if (ImGui::InputFloat3("Scale", data, 2)) {
				entities[pickedIndex]->setScale(glm::vec3(data[0], data[1], data[2]));
			}
		}
			ImGui::NewLine;
	}
}

void Mouse::loadEntity(Entity *object)
{
	entities.push_back(object);
}
