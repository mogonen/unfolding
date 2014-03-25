#ifndef FOLDEDFACE_H
#define FOLDEDFACE_H

#include "face.h"
#include "extendededge.h"
#include <QVector>
#include <QVector3D>

class ExtendedEdge;

class ExtendedFace : public Face {
public:
    ExtendedFace(QVector <Vertex* > inVertices, QVector <ExtendedEdge* >& inEdges);

    bool BuildSmallerFace(qreal scale);

    QVector <QVector3D> GetFoldedSmallerFace() { return smallerFace; }

//    QVector <ExtendedEdge* >& GetEdges() { return ExtendedFace::m_edges; }

    ExtendedEdge* SharedEdge(ExtendedFace* face);

    void ClearNeighbors(){treeChildren.clear();treeNotChildren.clear();}
    void AddTreeChildFace(ExtendedFace* face, float angle) { treeChildren.push_back(face); treeForwardingAngle.push_back(angle);}
    void AddTreeNotChildFace(ExtendedFace* face) {treeNotChildren.push_back(face); }

    QVector <ExtendedFace*> GetTreeChildrenFaces() { return treeChildren; }
    QVector <ExtendedFace*> GetNotTreeChildrenFaces() { return treeNotChildren; }

    void SetUnfoldedSmallerFace(QVector3D v, int id) {
        if (id>2 || id<0)
            return;
        unfoldedSmallerFace[id] = v;
    }
    QVector <QVector3D > GetUnfoldedSmallerFace() { return unfoldedSmallerFace;}
    void ClearUnfoldedSmallerFace() {unfoldedSmallerFace.clear(); unfoldedSmallerFace.resize(3); }

    bool IsChildFace(ExtendedFace* f) {
        if (f == NULL) return false;
        for (int i=0; i<treeChildren.size(); ++i)
            if (treeChildren[i] == f) return true;
        return false;
    }

private:
    Face* face;
    QVector <ExtendedFace* > treeChildren;
    QVector <ExtendedFace* > treeNotChildren;
    QVector <float> treeForwardingAngle;
    QVector <QVector3D > smallerFace;
    QVector <QVector3D > unfoldedSmallerFace;
//    QVector <ExtendedEdge*> m_edges;
};

#endif // FOLDEDFACE_H
