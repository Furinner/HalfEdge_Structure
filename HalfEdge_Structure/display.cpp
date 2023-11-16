#include "display.h"

VertDisplay::VertDisplay() {
    float points[] = {
        -3.f, -3.f, 0.f,
        1.f, 1.f, 0.f
    };
    float cols[] = {
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f
    };
    unsigned int indices[] = {
        0, 1
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &bufPos);
    glGenBuffers(1, &bufCol);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cols), cols, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(10);
}

void VertDisplay::update(int v1idx, int v2idx, uPtr<Model>& model) {
	this->v1idx = v1idx;
	this->v2idx = v2idx;
	if ((v1idx != -1) || (v2idx != -1)) {
		render = true;
    }
    else {
        render = false;
    }
    if (render) {
        //重新载入数值
        std::vector<vec3> pos;
        std::vector<vec3> col;
        std::vector<int> idx;
        if (v1idx != -1) {
            pos.push_back(model->vertices[v1idx]->pos);
            col.push_back(vec3(0.f, 1.f, 0.f));
        }
        if (v2idx != -1) {
            pos.push_back(model->vertices[v2idx]->pos);
            col.push_back(vec3(1.f, 0.f, 0.f));
        }
        for (int i = 0; i < pos.size(); ++i) {
            idx.push_back(i);
        }
        glBindBuffer(GL_ARRAY_BUFFER, bufPos);
        glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(vec3), pos.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, bufCol);
        glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(vec3), col.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void VertDisplay::draw() {
    if (render) {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_POINTS, 2, GL_UNSIGNED_INT, 0);
    }
}

void VertDisplay::dontRender() {
    render = false;
}



LoopDisplay::LoopDisplay() {
    float points[] = {
        -2.f, -1.f, 0.f,
        2.f, 1.f, 0.f,
        2.5f, 1.5f, 0.f
    };
    float cols[] = {
        1.f, 0.f, 0.f,
        0.f, 0.f, 1.f,
        0.f, 1.f, 0.f
    };
    unsigned int indices[] = {
        0, 1, 1, 2, 2, 1, 1, 0
    };
    //设置点1的VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &bufPos);
    glGenBuffers(1, &bufCol);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cols), cols, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLfloat lineWidthRange[2] = { 0.0f, 0.0f };
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
    lineWidth = ((lineWidthRange[0] + lineWidthRange[1])/2.5f);
}

void LoopDisplay::update(int lp, uPtr<Model>& model) {
	this->lp = lp;
	if (lp != -1) {
		render = true;
    }
    else {
        render = false;
    }
    if (render) {
        //重新载入数值
        Loop* loop = model->loops[lp].get();
        HalfEdge* origHe = loop->ledg;
        if (origHe == nullptr) {
            render = false;
            return;
        }
        std::vector<vec3> pos;
        std::vector<vec3> col;
        std::vector<int> idx;
        HalfEdge* currHe = origHe;
        do {
            pos.push_back(currHe->hev->pos);
            currHe = currHe->nxt;
        } while (currHe != origHe);
        pos.push_back(pos[0]);
        for (int i = 0; i < pos.size() - 1; ++i) {
            idx.push_back(i);
            idx.push_back(i + 1);
        }
        float incr = 1.f / static_cast<float>(pos.size()-1);
        for (int i = 0; i < pos.size(); ++i) {
            col.push_back(vec3(1.f - i * incr));
        }
        glBindBuffer(GL_ARRAY_BUFFER, bufPos);
        glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(vec3), pos.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, bufCol);
        glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(vec3), col.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        numOfVer = idx.size();
    }
}

void LoopDisplay::draw() {
    if (render) {
        glLineWidth(lineWidth);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_LINES, numOfVer, GL_UNSIGNED_INT, 0);
    }
}

void LoopDisplay::dontRender() {
    render = false;
}



EdgeDisplay::EdgeDisplay() {
    float points[] = {
        0.f, 0.f, 0.f,
        1.f, 1.f, 1.f
    };
    float cols[] = {
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
    };
    unsigned int indices[] = {
        0, 1
    };
    //设置点1的VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &bufPos);
    glGenBuffers(1, &bufCol);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cols), cols, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLfloat lineWidthRange[2] = { 0.0f, 0.0f };
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
    lineWidth = ((lineWidthRange[0] + lineWidthRange[1]) / 2.f);
}

void EdgeDisplay::update(int edgIdx, uPtr<Model>& model) {
    this->edgIdx = edgIdx;
    if (edgIdx != -1) {
        render = true;
    }
    else {
        render = false;
    }
    if (render) {
        //重新载入数值
        Edge* edg = model->edges[edgIdx].get();
        std::vector<vec3> pos{edg->he1->hev->pos, edg->he2->hev->pos};
        std::vector<vec3> col{vec3(0.f, 0.f, 1.f), vec3(0.f, 0.f, 1.f)};
        //这个可以用bufferSubData
        glBindBuffer(GL_ARRAY_BUFFER, bufPos);
        glBufferSubData(GL_ARRAY_BUFFER, 0, pos.size() * sizeof(vec3), pos.data());
        glBindBuffer(GL_ARRAY_BUFFER, bufCol);
        glBufferSubData(GL_ARRAY_BUFFER, 0, col.size() * sizeof(vec3), col.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void EdgeDisplay::draw() {
    if (render) {
        glLineWidth(lineWidth);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
    }
}

void EdgeDisplay::dontRender() {
    render = false;
}



FaceDisplay::FaceDisplay() {
    float points[] = {
        0.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 1.f, 1.f
    };
    float nors[] = {
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
    };
    float cols[] = {
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
    };
    unsigned int indices[] = {
        0, 1, 2
    };
    //设置点1的VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &bufPos);
    glGenBuffers(1, &bufNor);
    glGenBuffers(1, &bufCol);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nors), nors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cols), cols, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void FaceDisplay::update(int f1idx, int f2idx, uPtr<Model>& model, uPtr<Camera>& cam, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT) {
    this->f1idx = f1idx;
    this->f2idx = f2idx;
    int f1VIdx = 0;
    int f2VIdx = 0;
    if ((f1idx != -1) || (f2idx != -1)) {
        render = true;
    }
    else {
        render = false;
    }
    if (render) {
        try {
            std::vector<vec3> pos;
            std::vector<vec3> nor;
            std::vector<vec3> col;
            std::vector<int> idx;
            mat4 camProjMat = glm::perspective(glm::radians(cam->fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
            mat4 camViewMat = cam->getViewMatrix();
            mat4 projView = camProjMat * camViewMat;
            if (f1idx != -1) {
                //创建CDT
                CDT::Triangulation<float> cdt;
                std::vector<CDT::V2d<float>> f1pos;
                CDT::EdgeVec edges;
                Face* f1 = model->faces[f1idx].get();
                //iterate all loops of this face
                Loop* lp1 = f1->floop;
                Loop* currlp = lp1;
                do {
                    int origF1VIdx = f1VIdx;
                    HalfEdge* origHe = currlp->ledg;
                    HalfEdge* currHe = origHe;
                    if (currHe == nullptr) {
                        render = false;
                        return;
                    }
                    do {
                        vec2 vInCam = vec2(projView * vec4(currHe->hev->pos, 1.f));
                        CDT::V2d<float> tempV{ vInCam.x, vInCam.y };
                        f1pos.push_back(tempV);
                        pos.push_back(currHe->hev->pos);
                        nor.push_back(f1->nor);
                        col.push_back(vec3(1.f, 0.f, 0.f));
                        edges.push_back(CDT::Edge(f1VIdx, f1VIdx + 1));
                        f1VIdx += 1;
                        currHe = currHe->nxt;
                    } while (currHe != origHe);
                    edges.pop_back();
                    edges.push_back(CDT::Edge(f1VIdx - 1, origF1VIdx));
                    currlp = currlp->nextl;
                } while (currlp != lp1);
                cdt.insertVertices(f1pos);
                cdt.insertEdges(edges);
                cdt.eraseOuterTrianglesAndHoles();
                for (int i = 0; i < cdt.triangles.size(); ++i) {
                    idx.push_back(cdt.triangles[i].vertices[0]);
                    idx.push_back(cdt.triangles[i].vertices[1]);
                    idx.push_back(cdt.triangles[i].vertices[2]);
                }
            }
            if (f2idx != -1) {
                //创建CDT
                int idx_offset = pos.size();
                CDT::Triangulation<float> cdt;
                std::vector<CDT::V2d<float>> f1pos;
                CDT::EdgeVec edges;
                Face* f2 = model->faces[f2idx].get();
                //iterate all loops of this face
                Loop* lp1 = f2->floop;
                Loop* currlp = lp1;
                do {
                    int origF2VIdx = f2VIdx;
                    HalfEdge* origHe = currlp->ledg;
                    HalfEdge* currHe = origHe;
                    if (currHe == nullptr) {
                        render = false;
                        return;
                    }
                    do {
                        vec2 vInCam = vec2(projView * vec4(currHe->hev->pos, 1.f));
                        CDT::V2d<float> tempV{ vInCam.x, vInCam.y };
                        f1pos.push_back(tempV);
                        pos.push_back(currHe->hev->pos);
                        nor.push_back(f2->nor);
                        col.push_back(vec3(0.f, 1.f, 0.f));
                        edges.push_back(CDT::Edge(f2VIdx, f2VIdx + 1));
                        f2VIdx += 1;
                        currHe = currHe->nxt;
                    } while (currHe != origHe);
                    edges.pop_back();
                    edges.push_back(CDT::Edge(f2VIdx - 1, origF2VIdx));
                    currlp = currlp->nextl;
                } while (currlp != lp1);
                cdt.insertVertices(f1pos);
                cdt.insertEdges(edges);
                cdt.eraseOuterTrianglesAndHoles();
                for (int i = 0; i < cdt.triangles.size(); ++i) {
                    idx.push_back(cdt.triangles[i].vertices[0] + idx_offset);
                    idx.push_back(cdt.triangles[i].vertices[1] + idx_offset);
                    idx.push_back(cdt.triangles[i].vertices[2] + idx_offset);
                }
            }
            glBindBuffer(GL_ARRAY_BUFFER, bufPos);
            glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(vec3), pos.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, bufNor);
            glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(vec3), nor.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, bufCol);
            glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(vec3), col.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);
            drawElement = idx.size();
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        catch (...) {
            render = false;
        }
    }
}

void FaceDisplay::draw() {
    if (render) {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, drawElement, GL_UNSIGNED_INT, 0);
    }
}

void FaceDisplay::dontRender() {
    render = false;
}