#include "global.h"
#include "model.h"

Vertex::Vertex(vec3 pos) {
    this->pos = pos;
}

Model::Model() {
    float points[] = { 
        -1.f, -1.f, 0.f,
        1.f, 1.f, 0.f
    };
    unsigned int indices[] = {
        0, 1
    };
    //设置点1的VAO
    glGenVertexArrays(1, &vVAO1);
    glGenBuffers(1, &vVBO1);
    glGenBuffers(1, &vEBO1);

    glBindVertexArray(vVAO1);
    glBindBuffer(GL_ARRAY_BUFFER, vVBO1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vEBO1);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Model::setupModel() {

    float vertices[] = {
         5.f,  1.f, 0.0f,  // top right 
         1.f, -1.f, 0.0f,  // bottom right 
        -1.f, -1.f, 0.0f,  // bottom left 
        -1.f,  1.f, 0.0f   // top left  
    };
    unsigned int indices[] = {  // note that we start from 0! 
        0, 1, 3,  // first Triangle 
        1, 2, 3   // second Triangle 
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s). 
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

}

void Model::draw() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //index有几个顶点就绘制几个，一个长方形就是6个顶点
}



Solid* Model::mvfs(vec3 vpos) {
    vertices.push_back(mkU<Vertex>(vpos));

    uPtr<Loop> loop = mkU<Loop>();
    uPtr<Face> face = mkU<Face>();
    uPtr<Solid> solid = mkU<Solid>();
    
    solid->sface = face.get();
    
    face->fsolid = solid.get();
    face->floop = loop.get();

    loop->lface = face.get();
    
    solids.push_back(std::move(solid));
    faces.push_back(std::move(face));
    loops.push_back(std::move(loop));
    
    return solids.back().get();
}

HalfEdge* Model::mev(Vertex* v1, Loop* lp, vec3 v2Pos) {
    uPtr<HalfEdge> he1 = mkU<HalfEdge>();
    uPtr<HalfEdge> he2 = mkU<HalfEdge>();
    uPtr<Vertex> v2 = mkU<Vertex>(v2Pos);
    uPtr<Edge> eg = mkU<Edge>();

    //固定设置
    eg->he1 = he1.get();
    eg->he2 = he2.get();
    he1->hee = eg.get();
    he2->hee = eg.get();
    he1->hev = v1;
    he2->hev = v2.get();
    he1->nxt = he2.get();
    he1->wloop = lp;
    he2->wloop = lp;
    if (lp->ledg == nullptr) {
        //如果为null说明是刚构建的loop，可能是刚mvfs，互相指即可
        he2->nxt = he1.get();
        lp->ledg = he1.get();
        v1->vhe = he1.get();
        v2->vhe = he2.get();
    }
    else {
        HalfEdge* he;
        for (he = lp->ledg; he->nxt->hev != v1; he = he->nxt) {}
        he2->nxt = he->nxt; he->nxt = he1.get();
        v2->vhe = he2.get();
    }

    //计入链表
    HalfEdge* returnHe = he1.get();
    vertices.push_back(std::move(v2));
    halfEdges.push_back(std::move(he1));
    halfEdges.push_back(std::move(he2));
    edges.push_back(std::move(eg));

    return returnHe;
}

Face* Model::mef(Vertex* v1, Vertex* v2) {
    uPtr<HalfEdge> he1 = mkU<HalfEdge>();
    uPtr<HalfEdge> he2 = mkU<HalfEdge>();
    uPtr<Edge> eg = mkU<Edge>();
    uPtr<Loop> lp = mkU<Loop>();
    uPtr<Face> f = mkU<Face>();
    he1->hee = eg.get();
    he1->hev = v1;
    he1->wloop = v1->vhe->wloop;
    he2->hee = eg.get();
    he2->hev = v2;
    he2->wloop = lp.get();
    eg->he1 = he1.get();
    eg->he2 = he2.get();
    lp->lface = f.get();
    lp->ledg = he2.get();
    f->floop = lp.get();
    v1->vhe->wloop->ledg = he1.get();
    //这里定义v1走到v2为内环，逆时针走向，面指外
    //v2指的he一定往回走，所以取v2的he往回
    HalfEdge* heTmp = v2->vhe;
    while (heTmp->nxt->hev != v1) {
        heTmp = heTmp->nxt;
    }
    //更改所有顺时针走向he的loop
    HalfEdge* heTmp2 = heTmp->nxt;
    while (heTmp2->nxt->hev != v2) {
        heTmp2->wloop = lp.get();
        heTmp2 = heTmp2->nxt;
    }
    heTmp2->wloop = lp.get();
    //逆时针设定为原loop和面
    heTmp->nxt = he1.get();
    he1->nxt = v2->vhe;
    //顺时针设定为新面
    heTmp2->nxt = he2.get();
    he2->nxt = v1->vhe;
    //最后设定v1v2的vhe
    v2->vhe = he2.get(); //v2跟随顺时针面
    v1->vhe = he1.get(); //v1跟随逆时针面

    Face* returnF = f.get();
    halfEdges.push_back(std::move(he1));
    halfEdges.push_back(std::move(he2));
    edges.push_back(std::move(eg));
    loops.push_back(std::move(lp));
    faces.push_back(std::move(f));

    return returnF;
}

Loop* kemr(Loop* lp) {
    return nullptr;
}