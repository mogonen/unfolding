#include "extendedface.h"


ExtendedFace::ExtendedFace(QVector <Vertex* > inVertices, QVector <ExtendedEdge* >& inEdges) {
    m_edges.reserve(inEdges.size());
    for(int i=0;i<inEdges.size();i++)
        m_edges.push_back(inEdges[i]);
    m_vertices = inVertices;
    for (int i=0; i<inEdges.size(); ++i)
        inEdges[i]->AddIncidentFace(this);
    CalculateCenter();
    CalculateNorm();
    BuildSmallerFace(1.0);
    unfoldedSmallerFace.resize(3);
}

bool ExtendedFace::BuildSmallerFace(qreal scale) {
    if (scale < 0.0 || scale > 1.0) return false;
    // clear the former smaller faces
    smallerFace.clear();
    for (int i=0; i<m_vertices.size(); ++i) {
        qreal x = (1.0-scale)*m_center.x() + (scale)*m_vertices[i]->x();
        qreal y = (1.0-scale)*m_center.y() + (scale)*m_vertices[i]->y();
        qreal z = (1.0-scale)*m_center.z() + (scale)*m_vertices[i]->z();
        smallerFace.push_back(QVector3D(x, y, z));
    }
    return true;
}


ExtendedEdge* ExtendedFace::SharedEdge(ExtendedFace* face) {
    if (!face) return NULL;
    for (int i=0; i<m_edges.size(); ++i)
        if (m_edges[i]->GetAnotherFace(this) == (Face *)face)
            return (ExtendedEdge*)m_edges[i];
    return NULL;
}
