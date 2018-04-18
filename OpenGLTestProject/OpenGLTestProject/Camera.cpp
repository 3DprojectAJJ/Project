#include "Camera.h"

Camera::Camera(glm::vec3 up, glm::vec3 forward, glm::vec3 pos, double speed)
{
	this->up = up;
	this->forward = forward;
	this->right = glm::cross(forward, up);
	this->pos = pos;
	this->speed = speed;
}

Camera::Camera(double horisontalAngle, double verticalAngle, glm::vec3 pos, double speed)
{
	horizontalAngle = horisontalAngle;
	this->verticalAngle = verticalAngle;
	this->pos = pos;
	this->speed = speed;
	forward = glm::normalize(glm::vec3(cos(verticalAngle)*sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle)));
	right = glm::vec3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f));
	up = glm::cross(right, forward);
}

Camera::~Camera()
{
	//nothing
}

void Camera::update(GLFWwindow * window, float dt)
{
	//glm::vec4 d = viewMat()*glm::vec4(pos,1.0);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		pos += forward*dt*speed;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		pos -= forward * dt*speed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		pos += right * dt*speed;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		pos -= right * dt*speed;
	}

	double posX;
	double posY;
	glfwGetCursorPos(window, &posX, &posY);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		horizontalAngle += (speed / 10)*dt*float(mousePos.x - posX);
		verticalAngle += (speed / 10)*dt*float(mousePos.y - posY);

		if (horizontalAngle > 3.14 * 2)
			horizontalAngle -= 3.14 * 2;
		else if (horizontalAngle < 0)
			horizontalAngle += 3.14 * 2;
		if (verticalAngle > 3.14 * 2)
			verticalAngle -= 3.14 * 2;
		else if (verticalAngle < 0)
			verticalAngle += 3.14 * 2;

		forward = glm::normalize(glm::vec3(cos(verticalAngle)*sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle)));
		right = glm::vec3(sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f));

		up = glm::cross(right, forward);
	}

	mousePos = glm::vec2(posX, posY);
}

glm::mat4 Camera::viewMat()
{
	return glm::lookAt(pos,pos + forward,up);
}
