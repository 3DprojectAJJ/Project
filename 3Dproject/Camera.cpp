#include "Camera.h"

Camera::Camera()
{
	pos = glm::vec3(10, 5, 5);
	target = glm::vec3(0, 0, 0) - pos;
	up = glm::vec3(0, 1, 0);
}

Camera::Camera(const glm::vec3 & pos, const glm::vec3 & target, const glm::vec3 up, const glm::vec2 mousePos)
{
	lastMousePos = mousePos;
	this->pos = pos;
	this->target = target;
	this->up = up;
}

void Camera::SetMousePos(glm::vec2 mousePos)
{
	lastMousePos = mousePos;
}

void Camera::setCamPos(glm::vec3 camPos)
{
	pos = camPos;
}

bool Camera::OnKeyboard(int key, float dt, float moveSpeed)
{
	bool res = false;
	glm::vec3 movVec;
	switch (key)
	{
	case 0: //W key, Zoom in
		movVec = target;
		movVec /= (float)movVec.length();
		pos += movVec*dt*moveSpeed;
		res = true;
		break;
	case 1://S key, Zoom Out
		movVec = target;
		movVec /= (float)movVec.length();
		pos -= movVec*dt*moveSpeed;
		res = true;
		break;
	}

	return res;
}

void Camera::OnMouse(double x, double y , float dt, float mouseSpeed)
{
	horizontalAngle += (mouseSpeed/10)*dt*float(lastMousePos.x - x);
	verticalAngle += (mouseSpeed/10)*dt*float(lastMousePos.y - y);

	if (horizontalAngle > 3.14 * 2)
		horizontalAngle -= 3.14 * 2;
	else if (horizontalAngle < 0)
		horizontalAngle += 3.14 * 2;
	if (verticalAngle > 3.14 * 2)
		verticalAngle -= 3.14 * 2;
	else if(verticalAngle < 0)
		verticalAngle += 3.14 * 2;

	lastMousePos.x = x;
	lastMousePos.y = y;

	target = glm::normalize(glm::vec3(cos(verticalAngle)*sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle)));

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	up = glm::cross(right, target);
}

const glm::vec3 Camera::GetPos() const
{
	return pos;
}

const glm::vec3 Camera::GetTarget() const
{
	return target;
}

const glm::vec3 Camera::GetUp() const
{
	return up;
}

const glm::vec2 Camera::GetAngles() const
{
	return glm::vec2(horizontalAngle,verticalAngle);
}
