#pragma once
#include <glad/glad.h> 
#include "global.h"
#include "model.h"
#include "camera.h"
#include <vector>
#include "CDT/CDT.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class VertDisplay {
public:
	VertDisplay();
	void update(int v1idx, int v2idx, uPtr<Model>& model);
	void draw();
	void dontRender();
private:
	int v1idx = -1;
	int v2idx = -1;
	unsigned int VAO, bufPos, bufCol, EBO;
	bool render = false;
};


class LoopDisplay {
public:
	LoopDisplay();
	void update(int lp, uPtr<Model>& model);
	void draw();
	void dontRender();
private:
	int numOfVer = 0;
	int lp = -1;
	unsigned int VAO, bufPos, bufCol, EBO;
	float lineWidth;
	bool render = false;
};


class EdgeDisplay {
public:
	EdgeDisplay();
	void update(int edgIdx, uPtr<Model>& model);
	void draw();
	void dontRender();
private:
	int edgIdx = -1;
	unsigned int VAO, bufPos, bufCol, EBO;
	float lineWidth;
	bool render = false;
};


class FaceDisplay {
public:
	FaceDisplay();
	void update(int faceIdx1, int faceIdx2, uPtr<Model>& model, uPtr<Camera>& cam, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT);
	void draw();
	void dontRender();
private:
	unsigned int VAO, bufPos, bufNor, bufCol, EBO;
	int f1idx = -1;
	int f2idx = -1;
	bool render = false;
	int drawElement = 0;
};


