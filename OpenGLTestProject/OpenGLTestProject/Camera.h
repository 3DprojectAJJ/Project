#ifndef CAMERA_H
#define CAMERA_H
#include <GLM\vec3.hpp>
#include <GLM\vec2.hpp>
#include <GLFW\glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM\gtx\transform.hpp>

class Camera
{
private:
	glm::vec3 up;
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 pos;
	float speed;
	glm::vec2 mousePos;

	double horizontalAngle;
	double verticalAngle;
public:
	Camera(glm::vec3 up, glm::vec3 forward, glm::vec3 pos = glm::vec3(0, 0, 0), double speed = 10.0f);
	Camera(double horisontalAngle, double verticalAngle, glm::vec3 pos = glm::vec3(0, 0, 0), double speed = 10.0f);
	~Camera();

	void update(GLFWwindow * window, float dt);

	glm::vec3 getPos()const;
	glm::vec3 getForward()const;

	glm::mat4 viewMat();

	void setCameraPos(glm::vec3 pos);
};

#endif