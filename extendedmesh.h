#ifndef ONEPIECEMESH_H
#define ONEPIECEMESH_H

#include "basemesh.h"
#include "edge.h"
#include "extendedface.h"
#include <QVector>
#include <QMap>
#include <QStack>
#include <QQueue>
#include <QSet>
#include <assert.h>
#include <QMessageBox>

class ExtendedMesh : public BaseMesh{
public:
    ExtendedMesh();
    bool RebuildMesh(qreal scale, bool rebuild = false);
    bool AddFace(QVector <Vertex*>& vertices, QVector<int> edge_flags);
//    QVector <ExtendedFace* > GestExtFaces() { return m_faces; }
//    QVector <ExtendedEdge* > GetExtEdges() { return m_edges; }
    bool BuildFoldedMesh();
    bool BuildUnFoldedMesh();
    BaseMesh* GetFoldedMesh() { return foldedMesh; }
    BaseMesh* GetUnFoldedMesh() { return unfoldedMesh; }


private:
    bool InitialExtendedMesh();
    bool MST(ExtendedFace* first);

    bool PutZPlane(ExtendedFace* face);
    bool UnfoldBranch(ExtendedFace* face, ExtendedEdge* edge, QVector3D normal);
    bool UnfoldFace(ExtendedFace* last, ExtendedFace* next, ExtendedEdge* edge, QVector3D normal);
//    ExtendedEdge* EdgeExisting(Vertex* s, Vertex* e) {
//        for (int i=0; i<m_edges.size(); ++i) {
//            if ((m_edges[i]->GetEnd() == e && m_edges[i]->GetStart() == s) ||
//                (m_edges[i]->GetEnd() == s && m_edges[i]->GetStart() == e))
//                return (ExtendedEdge*)(m_edges[i]);
//        }
//        return NULL;
//    }
    QVector3D GetEdgeVerticalVector(const QVector3D& start, const QVector3D& end, ExtendedFace* face, QVector3D normal);
    QVector3D GetFirstUnfoldedPos(const QVector3D& oldPos, ExtendedFace* face);
    Vertex* GetCorrespondingVertex(QVector3D& pos, QVector <QPair<QVector3D, Vertex*> >& table, BaseMesh* mesh);
    bool Unfolding();
    int GetFoldedFaceOrderID(const QVector3D& vertex, ExtendedFace* face);

private:
    enum RANGE {MAX_VALUE = 100, MIN_VALUE = 30};
    qreal m_scale;

    //m_triangleMesh is wrongly used without any initailization, m_tirangleMesh removed(Youyou Wang 11/04/2013)
//    BaseMesh* m_triangleMesh;
    bool isInitialized;
    bool isFoldedMeshReady;

    ExtendedFace* root;
//    QVector <ExtendedEdge* > m_edges;
//    QVector <ExtendedFace* > m_faces;

    BaseMesh* foldedMesh;
    BaseMesh* unfoldedMesh;
    QVector <ExtendedFace* > MSTQueue;

};

#endif // ONEPIECEMESH_H
