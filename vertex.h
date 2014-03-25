#ifndef VERTEX_H
#define VERTEX_H

#include "edge.h"
#include "face.h"
#include <QVector3D>
#include <QVector2D>
#include <QVector>
#include <QtMath>


class Edge;
class Face;

class Vertex : public QVector3D {
public:
    Vertex(qreal x, qreal y, qreal z) : QVector3D(x, y, z) { m_value=0; }
    QVector<Edge*>& GetIncidentEdges() {
        return m_edges;
    }
    QVector<Face*>& GetIncidentFaces(){
        return m_faces;
    }

    bool AddEdge(Edge* e) {
        m_edges.push_back(e);
        return true;
    }

    bool EraseEdge(Edge* e) {
        for (int i=0; i<m_edges.size(); ++i)
            if (m_edges[i] == e) {
                m_edges.erase(m_edges.begin()+i);
                return true;
            }
        return false;
    }

    Edge *FindConnectingEdge(Vertex *other_v);
    //put all the edges in topological oder
    void OrderEdges();


    void CalValue();
    float Value(){return m_value;}
    bool& GetFlag() {return m_flag;}

    //only used for copy
    Vertex* copy_indicator;
protected:
    bool m_flag;
    float m_value;
    QVector<Edge*> m_edges;
    QVector<Face *> m_faces;
};


#endif // VERTEX_H
