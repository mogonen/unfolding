#ifndef EDGE_H
#define EDGE_H

#include "vertex.h"
#include "face.h"
#include <QVector>


class Face;

class Edge {
public:
    Edge() { m_weight = -1; m_flag = 0;}
    virtual ~Edge() { }
    Edge(Vertex* inStart, Vertex* inEnd) {
        m_start = inStart;
        m_end = inEnd;
        m_center = CalculateCenter();
        m_weight = -1;
        m_flag = 0;
    }
    Vertex* GetStart();
    Vertex* GetEnd();


    //get the corresponding forwading face from a vertex
    Face *GetFace(Vertex *V){
        if(GetStart()==V)
            return m_faces[0];
        if(GetEnd()==V)
            return m_faces[1];
        return NULL;
    }

    //get the corresponding backwarding face from a vertex
    Face *GetOtherFace(Vertex *V){
        if(GetStart()==V)
            return m_faces[1];
        if(GetEnd()==V)
            return m_faces[0];
        return NULL;
    }

    virtual bool AddIncidentFace(Face* f) {
        m_faces.push_back(f);
        return true;
    }

    QVector <Face*> IncidentFaces() { return m_faces;}
    virtual Face* GetAnotherFace(Face* f) {
        for (int i=0; i<m_faces.size(); ++i)
            if (m_faces[i] != f)
                return m_faces[i];
        return NULL;
    }

    QVector3D GetCenter() { return m_center; }

    QVector3D CalculateCenter() {
        qreal x = (m_start->x() + m_end->x())/2.0;
        qreal y = (m_start->y() + m_end->y())/2.0;
        qreal z = (m_start->z() + m_end->z())/2.0;

        QVector3D center = QVector3D(x, y, z);
        return center;
    }
    float Weight(){return m_weight;}
    void CalWeight()
    {
//        m_weight = (GetStart()->Value()+GetEnd()->Value())*(*GetStart()-*GetEnd()).length();
        m_weight = (GetStart()->Value()+GetEnd()->Value())/2;
//        m_weight = (GetStart()->Value()>GetEnd()->Value())?GetStart()->Value():GetEnd()->Value();
    }
    int &GetFlag(){return m_flag;}
public:
    Vertex* m_start;
    Vertex* m_end;
    QVector3D m_center;

protected:
    //utility flag
    //0- this edge can be cut, 1- this edge should be cut, -1 - this edge should not be cut
    int m_flag;
    float m_weight;
    QVector <Face*> m_faces;
};


inline Vertex* Edge::GetStart() {return m_start;}
inline Vertex* Edge::GetEnd() {return m_end;}

#endif // EDGE_H
