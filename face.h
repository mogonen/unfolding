#ifndef FACE_H
#define FACE_H

#include "edge.h"
#include <QVector>
#include <QVector3D>
#include <assert.h>
#include <qmath.h>

class Vertex;
class Edge;
class Face {
public:
    Face() {}
    virtual ~Face() {}

    Face(QVector <Vertex* > inVertices, QVector <Edge* >& inEdges) {
        m_edges = inEdges;
        m_vertices = inVertices;
        for (int i=0; i<inEdges.size(); ++i)
            inEdges[i]->AddIncidentFace(this);
        CalculateCenter();
        CalculateNorm();
    }

    QVector <Vertex* >& GetVertices() { return m_vertices;}
    QVector <float>& GetFaceAngles() {return m_faceangles;}
    QVector <Edge* >& GetEdges() { return Face::m_edges; }
    QVector3D GetCenter() { return m_center; }
    //Find the angle around inq
    float FindFaceAngle(Vertex* inq)
    {
        if(m_faceangles.size()==m_vertices.size())
        {
            for(int i=0;i<m_vertices.size();i++)
            {
                if(m_vertices[i]==inq)
                    return m_faceangles[i];
            }
        }
        return false;
    }

    bool ComputeFaceAngle()
    {
        if(m_vertices.size()>=3)
        {
            Vertex *inq, *p_inq, *n_inq;
            m_faceangles.resize(m_vertices.size());
            for(int i=0;i<m_vertices.size();i++)
            {
                inq = m_vertices[i];
                if(i==0)
                    p_inq = m_vertices.last();
                else
                    p_inq = m_vertices[i-1];
                if(i==m_vertices.size()-1)
                    n_inq = m_vertices.first();
                else
                    n_inq = m_vertices[i+1];
                //A,B,C
                QVector3D AB= (*p_inq - *inq).normalized();
                QVector3D CB= (*n_inq - *inq).normalized();
                float angle = qAcos(QVector3D::dotProduct(AB,CB));
                if(angle>M_PI)
                    angle = angle-M_PI;
                if(angle<0)
                    angle = angle+M_PI;
                m_faceangles[i] = angle*180/M_PI;
            }
            CalculateNorm();
            return true;
        }
        else
            return false;
    }
protected:
    void CalculateCenter() {
        int size = m_vertices.size();
        qreal x = 0.0;
        qreal y = 0.0;
        qreal z = 0.0;
        for (int i=0; i<size; ++i) {
            x += m_vertices[i]->x();
            y += m_vertices[i]->y();
            z += m_vertices[i]->z();
        }
        x = x/size;
        y = y/size;
        z = z/size;
        m_center = QVector3D(x, y, z);
    }

    void CalculateNorm() {
        m_norm = QVector3D(0, 0, 0);
        assert(m_vertices.size() >= 3);
        QVector3D U = *m_vertices[1] - *m_vertices[0];
        QVector3D V = *m_vertices[2] - *m_vertices[1];

        m_norm = U.crossProduct(U, V);
        m_norm.normalize();
    }

    //Find the facial neighbors of the vertex
    bool FindFaceNeighbor(Vertex* inq, Vertex** p_inq, Vertex** n_inq)
    {
        for(int i=0;i<m_vertices.size();i++)
        {
            if(m_vertices[i] == inq)
            {
                if(i == 0)
                    *p_inq = m_vertices[m_vertices.size()-1];
                else
                    *p_inq = m_vertices[i-1];
                if(i == m_vertices.size()-1)
                    *n_inq = m_vertices[0];
                else
                    *n_inq = m_vertices[i+1];
                return true;
            }
        }
        return false;
    }


public:
    Face *m_copyIndicator;
protected:
    QVector <Vertex*> m_vertices;
    QVector <Edge*> m_edges;
    QVector <float> m_faceangles;
    QVector3D m_norm;
    QVector3D m_center;

};

#endif // FACE_H
