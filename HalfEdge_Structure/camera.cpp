#include "camera.h"
#include <iostream>

Camera::Camera()
	:position(vec3(0.f, 0.f, 20.f)), up(vec3(0.f, 1.f, 0.f)), front(vec3(0.f, 0.f, -1.f)), right(1.f, 0.f, 0.f),
	movementSpeed(2.5f), mouseSensitivity(0.1f), fov(45.f)
{};


mat4 Camera::getViewMatrix()
{
	//create a transformation matrix to transform to camera space (The View Matrix)
	return glm::lookAt(position, position + front, up);
}

void Camera::keyboardMovement(Camera_Movement dir, float deltaTime) {
	float velocity = movementSpeed * deltaTime;
	if (dir == FORWARD) {
		position += front * velocity;
	}
	if (dir == BACKWARD)
		position -= front * velocity;
	if (dir == LEFT)
		position -= right * velocity;
	if (dir == RIGHT)
		position += right * velocity;
	if (dir == UP)
		position += up * velocity;
	if (dir == DOWN)
		position -= up * velocity;
}

void Camera::mouseMovement(float xOffset, float yOffset) {
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	rotateOnUpGlobal(-xOffset);
	rotateOnRightLocal(yOffset);
}


void Camera::rotateOnUpGlobal(float degree) {
	float rad = glm::radians(degree);
	mat4 rot = glm::rotate(mat4(1.f), rad, vec3(0, 1, 0));
	front = vec3(rot * vec4(front, 0.f));
	right = vec3(rot * vec4(right, 0.f));
	up = vec3(rot * vec4(up, 0.f));
}

void Camera::rotateOnRightLocal(float degree) {
	float rad = glm::radians(degree);
	mat4 rot = glm::rotate(mat4(1.f), rad, right);
	front = vec3(rot * vec4(front, 0.f));
	up = vec3(rot * vec4(up, 0.f));
}
