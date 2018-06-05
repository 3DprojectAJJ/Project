#include "Camera.h"

Camera::Camera(double inHorizontalAngle, double inVerticalAngle, glm::vec3 pos, double speed)
{
	this->horizontalAngle = glm::radians(inHorizontalAngle);
	this->verticalAngle = glm::radians(inVerticalAngle);
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

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
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

glm::vec3 Camera::getPos() const
{
	return pos;
}

glm::vec3 Camera::getForward() const
{
	return forward;
}

glm::mat4 Camera::viewMat()
{
	return glm::lookAt(pos,pos + forward,up);
}

void Camera::setCameraPos(glm::vec3 pos)
{
	this->pos = pos;
}
