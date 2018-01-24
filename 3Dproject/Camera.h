#ifndef CAMERA_H
#define CAMERA_H
#include <GLM\vec3.hpp>
#include <GLM\glm.hpp>

class Camera
{
public:
	Camera();
	Camera(const glm::vec3& pos, const glm::vec3 & target, const glm::vec3 up);
	bool OnKeyboard(int key);
	const glm::vec3 GetPos() const;
	const glm::vec3 GetTarget() const;
	const glm::vec3 GetUp() const;

private:
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 up;
};

#endif

