#include "Camera.h"

Camera::Camera()
{
	pos = glm::vec3(4, 3, 3);
	target = glm::vec3(0, 0, 0) - pos;
	up = glm::vec3(0, 1, 0);
}

Camera::Camera(const glm::vec3 & pos, const glm::vec3 & target, const glm::vec3 up)
{
	this->pos = pos;
	this->target = target;
	this->up = up;
}

bool Camera::OnKeyboard(int key, float dt)
{
	bool res = false;
	glm::vec3 movVec;
	switch (key)
	{
	case 0:
		movVec= target;
		movVec /= (float)movVec.length();
		pos += movVec*dt;
		res = true;
		break;
	case 1:
		movVec = target;
		movVec /= (float)movVec.length();
		pos -= movVec*dt;
		res = true;
		break;
	case 2:
		glm::vec3 left = glm::cross(up, target);
		left = left /(float)left.length();
		pos += left*dt;
		res = true;
		break;
	case 3:
		glm::vec3 right = glm::cross(target, up);
		right = right / (float)left.length();
		pos += right*dt;
		res = true;
		break;
	}

	return res;
}

void Camera::OnMouse(int x, int y)
{
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
