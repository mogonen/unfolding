#ifndef EXTENDEDEDGE_H
#define EXTENDEDEDGE_H

#include "extendedface.h"
#include <QVector>
#include <QVector3D>
#include <QMap>

#define CONNECTOR_SIZE 7  //must be larger or equal than 3

class ExtendedFace;

class ExtendedEdge : public Edge {
public:
    ExtendedEdge(Vertex* inStart, Vertex* inEnd, int flag = 0) {
        m_start = inStart;
        m_end = inEnd;
        m_is_connector = false;
        m_center = CalculateCenter();
        m_flag = flag;
    }

//    bool AddIncidentFace(ExtendedFace* f) {
//        m_faces.push_back(f);
//        return true;
//    }

    void SetAsConnector() { m_is_connector = true; }
    bool IsConnector() { return m_is_connector; }

    bool BuildConnector(ExtendedFace* parent, ExtendedFace* child, qreal scale);

//    QVector <ExtendedFace*> IncidentFaces() { return m_faces;}


//    ExtendedFace* GetAnotherFace(ExtendedFace* f) {
//        int i=0;
//        for (; i<m_faces.size(); ++i)
//            if (m_faces[i] == f)
//                break;
//        if (i == m_faces.size()) return NULL;
//        for (i=0; i<m_faces.size(); ++i)
//            if (m_faces[i] != f)
//                return m_faces[i];
//        return NULL;
//    }

    QVector <QVector <QVector3D> >& GetFoldedConnector(Face* f)
    {
        return m_connector[f];
    }
    QVector <QVector <QVector3D> >& GetUnfoldedConnector(Face* f) { return unfolded_m_connector[f]; }
    void AddUnfoldedConnector(Face* f, QVector <QVector3D>& curve) { unfolded_m_connector[f].push_back(curve); }
    void ClearUnfoldedConnector(Face* f) { unfolded_m_connector[f].clear(); }
    QVector <QVector <qreal> > ComputeCurveAngles(Face* f);
    //Edge value is the average of curavture of two vertices
    float Value(){return (GetStart()->Value()+GetEnd()->Value())/2;}
private:
    QVector <QVector3D> BezierCurve(QVector3D parentCenter, QVector3D childCenter, Vertex* endPoint, qreal scale, int size);


private:
    bool m_is_connector;
    QMap <Face*, QVector <QVector <QVector3D > > > m_connector;
    QMap <Face*, QVector <QVector <QVector3D > > > unfolded_m_connector;
//    QVector <ExtendedFace*> m_faces;

};

#endif // EXTENDEDEDGE_H



















