#pragma once
#include <glad/glad.h> 
#include "global.h"
#include "model.h"
#include <vector>

class VertDisplay {
public:
	VertDisplay();
	void update(int v1idx, int v2idx, uPtr<Model>& model);
	void draw();
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
private:
	int edgIdx = -1;
	unsigned int VAO, bufPos, bufCol, EBO;
	float lineWidth;
	bool render = false;
};