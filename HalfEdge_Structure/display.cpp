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