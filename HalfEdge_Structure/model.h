#pragma once
#include <glad/glad.h>; 
#include <vector>
#include "global.h"

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
    void setupModel();
    void draw();

    //Euler Operations
    Solid* mvfs(vec3 vpos);
    HalfEdge* mev(Vertex* v1, Loop* lp, vec3 v2Pos);
    Face* mef(Vertex* v1, Vertex* v2);
    Loop* kemr(Loop* lp);
private:
    //  渲染数据
    unsigned int VAO, VBO, EBO;
    // 顶点渲染数据
    unsigned int vVAO1, vVBO1, vEBO1;
    unsigned int vVAO2, vVBO2;
};