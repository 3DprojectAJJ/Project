#ifndef CAMERA_H
#define CAMERA_H
#include <GLM\vec3.hpp>
#include <GLM\glm.hpp>

class Camera
{
public:
	Camera();
	Camera(const glm::vec3& pos, const glm::vec3 & target, const glm::vec3 up, const glm::vec2 mousePos);
	void SetMousePos(glm::vec2 mousePos);
	bool OnKeyboard(int key, float dt);
	void OnMouse(double x, double y, float dt, float mouseSpeed);
	const glm::vec3 GetPos() const;
	const glm::vec3 GetTarget() const;
	const glm::vec3 GetUp() const;
	const glm::vec2 GetAngles() const;
private:
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 up;
	glm::vec2 lastMousePos;
	float horizontalAngle = 4.0f;
	float verticalAngle = -0.5f;
};

#endif

