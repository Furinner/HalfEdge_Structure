#pragma once
#include <glad/glad.h>; 
#include <vector>
#include "global.h"
#include "CDT/CDT.h"
#include "camera.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

class Vertex;
class HalfEdge;
class Edge;
class Loop;
class Face;
class Solid;

class Vertex {
public:
    vec3 pos;

    HalfEdge* vhe;

    Vertex(vec3);
};

class HalfEdge {
public:
    HalfEdge* nxt;
    Vertex* hev;
    Loop* wloop;
    Edge* hee;
};

class Edge {
public:
    HalfEdge* he1;
    HalfEdge* he2;
};

class Loop {
public:
    Loop* nextl;
    HalfEdge* ledg;
    Face* lface;
};

class Face {
public:
    Face* nextf;
    Loop* floop;
    Solid* fsolid;

    vec3 nor;
};

class Solid {
public:
    Solid* nexts;
    Face* sface;
};

class Model {
public:

    std::vector<uPtr<Solid>> solids;
    std::vector<uPtr<Face>> faces;
    std::vector<uPtr<Loop>> loops;
    std::vector<uPtr<Edge>> edges;
    std::vector<uPtr<HalfEdge>> halfEdges;
    std::vector<uPtr<Vertex>> vertices;

    Model();
    void update(int sIdx, uPtr<Camera>& cam);
    void draw();

    //Euler Operations
    Solid* mvfs(vec3 vpos, vec3 nor);
    Vertex* mev(Vertex* v1, Loop* lp, vec3 v2Pos);
    Face* mef(Vertex* v1, Vertex* v2, vec3 newFNor);
    void kemr(Edge* edg, Loop* lp);
    void kfmrh(Face* f1, Face* f2);

    //Sweeping
    void sweeping(Face* f, vec3 dir, float length);
private:
    //  渲染数据
    unsigned int VAO, bufPos, bufNor, bufCol, EBO;
    bool render = false;
    int sIdx = - 1;
    int drawElement = 0;
    void getFaceData(Face* f1, uPtr<Camera>& cam, std::vector<vec3>& pos, std::vector<vec3>& nor, std::vector<vec3>& col, std::vector<int>& idx);
};