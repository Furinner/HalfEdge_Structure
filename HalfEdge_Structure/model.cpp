#include "global.h"
#include "model.h"

Vertex::Vertex(vec3 pos) {
    this->pos = pos;
}

Model::Model() {
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



Solid* Model::mvfs(vec3 vpos, vec3 nor) {
    vertices.push_back(mkU<Vertex>(vpos));

    uPtr<Loop> loop = mkU<Loop>();
    uPtr<Face> face = mkU<Face>();
    uPtr<Solid> solid = mkU<Solid>();

    solid->sface = face.get();

    face->fsolid = solid.get();
    face->floop = loop.get();
    face->nor = nor;
    face->nextf = face.get();

    loop->lface = face.get();
    loop->nextl = loop.get();
    
    solids.push_back(std::move(solid));
    faces.push_back(std::move(face));
    loops.push_back(std::move(loop));
    
    return solids.back().get();
}

Vertex* Model::mev(Vertex* v1, Loop* lp, vec3 v2Pos) {
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
    Vertex* returnV = v2.get();
    vertices.push_back(std::move(v2));
    halfEdges.push_back(std::move(he1));
    halfEdges.push_back(std::move(he2));
    edges.push_back(std::move(eg));

    return returnV;
}

Face* Model::mef(Vertex* v1, Vertex* v2, vec3 newFNor) {
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
    lp->nextl = lp.get();
    f->floop = lp.get();
    f->nor = newFNor;
    f->fsolid = v1->vhe->wloop->lface->fsolid;
    if (f->fsolid->sface != nullptr) {
        f->nextf = f->fsolid->sface->nextf;
        f->fsolid->sface->nextf = f.get();
    }
    else {
        f->fsolid->sface = f.get();
    }
    v1->vhe->wloop->ledg = he1.get();
    //这里定义v1走到v2为内环，逆时针走向，面指外，且为原来的面
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
    //最后设定v1v2的vhe，全部跟随原来的面
    //v2->vhe = he1.get(); //v2跟随原面，即不变
    v1->vhe = he1.get(); //v1跟随原面

    Face* returnF = f.get();
    halfEdges.push_back(std::move(he1));
    halfEdges.push_back(std::move(he2));
    edges.push_back(std::move(eg));
    loops.push_back(std::move(lp));
    faces.push_back(std::move(f));

    return returnF;
}

void Model::kemr(Edge* edg, Loop* l) {
    uPtr<Loop> lp = mkU<Loop>();
    lp->lface = edg->he1->wloop->lface;
    if (l->nextl == l) {
        l->nextl = lp.get();
        lp->nextl = l;
    }
    else {
        lp->nextl = l->nextl;
        l->nextl = lp.get();
    }

    //选择edg的he2的走向作为原loop，另一个作为新loop。
    HalfEdge* he2 = edg->he2;
    HalfEdge* he1 = edg->he1;
    HalfEdge* currHe = he2;
    while (currHe->nxt != he1) {
        currHe = currHe->nxt;
    }
    currHe->nxt = he2->nxt;
    l->ledg = currHe;
    //处理新环
    currHe = he1;
    while (currHe->nxt != he2) {
        currHe = currHe->nxt;
        currHe->wloop = lp.get();
    }
    currHe->wloop = lp.get();
    lp->ledg = currHe;
    currHe->nxt = he1->nxt;
    //删除he1，he2
    int he1Idx = -1;
    int he2Idx = -1;
    for (int i = 0; i < int(halfEdges.size()); ++i) {
        if ((he1Idx != -1) && (he2Idx != -1)) {
            break;
        }
        if (halfEdges[i].get() == he1) {
            he1Idx = i;
        }
        if (halfEdges[i].get() == he2) {
            he2Idx = i;
        }
    }
    if (he1Idx > he2Idx) {
        halfEdges.erase(std::remove(halfEdges.begin(), halfEdges.end(), halfEdges[he1Idx]));
        halfEdges.erase(std::remove(halfEdges.begin(), halfEdges.end(), halfEdges[he2Idx]));
    }
    else {
        halfEdges.erase(std::remove(halfEdges.begin(), halfEdges.end(), halfEdges[he2Idx]));
        halfEdges.erase(std::remove(halfEdges.begin(), halfEdges.end(), halfEdges[he1Idx]));
    }
    //删除edg
    for (int i = 0; i < edges.size(); ++i) {
        if (edges[i].get() == edg) {
            edges.erase(std::remove(edges.begin(), edges.end(), edges[i]));
            break;
        }
    }
    //lp接上
    loops.push_back(std::move(lp));
}

void Model::kfmrh(Face* f1, Face* f2) {
    //let f1 become an inner loop of f2
    Loop* origLp = f1->floop;
    Loop* currLp = origLp;
    do {
        currLp->lface = f2;
        Loop* nxtLp = currLp->nextl;
        currLp->nextl = f2->floop->nextl;
        f2->floop->nextl = currLp;
        currLp = nxtLp;
    } while (currLp != origLp);
    //如果f1的体指向f1，让体指向f1的nextf
    if (f1->fsolid->sface == f1) {
        if (f1->nextf == f1) {
            //那么这个solid也没有存在的必要了，因为没有f了
            for (int i = 0; i < solids.size(); ++i) {
                if (solids[i].get() == f1->fsolid) {
                    solids.erase(std::remove(solids.begin(), solids.end(), solids[i]));
                    break;
                }
            }
        }
        else {
            f1->fsolid->sface = f1->nextf;
        }
    }
    //删除f1
    for (int i = 0; i < faces.size(); ++i) {
        if (faces[i].get() == f1) {
            faces.erase(std::remove(faces.begin(), faces.end(), faces[i]));
            break;
        }
    }
}

void Model::sweeping(Face* f, vec3 dir, float length) {
    Loop* origLp = f->floop;
    Loop* currLp = origLp;
    do {
        HalfEdge* origHe = currLp->ledg;
        HalfEdge* currHe = origHe;
        if (currHe == nullptr) {
            continue;
        }
        //计算要循环多少次
        int loopCnt = 0;
        do {
            loopCnt += 1;
            currHe = currHe->nxt;
        } while (currHe != origHe);
        //
        Vertex* firstV = origHe->hev;
        Vertex* firstNewV = mev(firstV, currLp, firstV->pos + glm::normalize(dir) * length);
        Vertex* preV = firstV;
        Vertex* prevNewV = firstNewV;
        for (int i = 0; i < loopCnt; ++i) {
            Vertex* currV;
            Vertex* currNewV;
            if (i == (loopCnt - 1)) {
                currV = firstV;
                currNewV = firstNewV;
            }
            else {
                currHe = currHe->nxt;
                currV = currHe->hev;
                currNewV = mev(currV, currLp, currV->pos + glm::normalize(dir) * length);
            }
            mef(prevNewV, currNewV, glm::cross(currV->pos - preV->pos, currNewV->pos - currV->pos));
            preV = currV;
            prevNewV = currNewV;
        }
        currLp = currLp->nextl;
    } while (currLp != origLp);
}