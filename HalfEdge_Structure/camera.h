#pragma once
#include "global.h"
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera {

public:

	vec3 position;
	vec3 front;
	vec3 up;
	vec3 right;
	float fov;

	// camera options
	float movementSpeed;
	float mouseSensitivity;

	Camera();

	mat4 getViewMatrix();
	void keyboardMovement(Camera_Movement dir, float deltaTime);
	void mouseMovement(float xOffset, float yOffset);


private:
	void rotateOnUpGlobal(float degree);
	void rotateOnRightLocal(float degree);
};