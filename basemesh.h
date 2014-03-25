#ifndef MESH_H
#define MESH_H

#include "face.h"
#include <QVector>
#include <QMessageBox>
#include <iostream>
#include <queue>


class BaseMesh{
public:
    BaseMesh() {}
    ~BaseMesh() { Clear(); }

    void Clear() {
        try {
        for (int i=0; i<m_edges.size(); ++i)
            delete m_edges[i];
        for (int i=0; i<m_faces.size(); ++i)
            delete m_faces[i];
        for (int i=0; i<m_vertices.size(); ++i)
            delete m_vertices[i];
        } catch(...) {
            QMessageBox::critical(NULL, "critical", "Invalid Pointer");
            return;
        }

        m_edges.clear();
        m_vertices.clear();
        m_faces.clear();
    }

    Vertex* AddVertex(qreal x, qreal y, qreal z) {
        try {
            Vertex* v = new Vertex(x, y, z);
            m_vertices.push_back(v);
            return v;
        } catch(...) {
            return NULL;
        }
    }

    Vertex* AddVertex(Vertex* v) {
        if (v == NULL) return NULL;
        return AddVertex(v->x(), v->y(), v->z());
    }

    virtual bool AddFace(QVector <Vertex*>& vertices) {
        if (vertices.size()<2) return false;
        QVector <Edge* > currentEdges;
        for (int i=0; i<vertices.size(); ++i) {
            int firstIndex = i;
            int lastIndex = (i == vertices.size()-1) ? 0 : i+1;
            Edge* current = EdgeExisting(vertices[firstIndex], vertices[lastIndex]);
            if (!current) {
                try {
                    Edge* edge = new Edge(vertices[firstIndex], vertices[lastIndex]);
                    m_edges.push_back(edge);
                    currentEdges.push_back(edge);
                    vertices[firstIndex]->GetIncidentEdges().push_back(edge);
                    vertices[lastIndex]->GetIncidentEdges().push_back(edge);

                } catch(...) {
                    return false;
                }
            }
            else
                currentEdges.push_back(current);
        }
        try {
            Face* face = new Face(vertices, currentEdges);
            m_faces.push_back(face);
        } catch(...) {
            return false;
        }
        return true;
    }

//    BaseMesh *Clone()
//    {
//        BaseMesh nb = new BaseMesh();
//        nb
//    }
    void EdgeMST()
    {
        std::cout << "Creating Edge MST..." << std::endl;
        std::priority_queue<Edge*, std::vector<Edge *>, CompareEdge> AdjacentEdges;
        //reset the flag for all the vertices and edges
        for(int i=0;i<m_vertices.size();i++)
            m_vertices[i]->GetFlag() = false;
        for(int i=0;i<m_edges.size();i++)
            m_edges[i]->GetFlag() = false;

        //randomly chose a vertex
        Vertex* root = m_vertices[0];
        QVector<Edge *> IncidentEdges = root->GetIncidentEdges();
        for(int i=0;i<IncidentEdges.size();i++)
            AdjacentEdges.push(IncidentEdges[i]);
        while(AdjacentEdges.size())
        {
            Edge* shortest = AdjacentEdges.top();
            AdjacentEdges.pop();
            //if the vertex of the shortest edges is not chosen
            Vertex* new_vertex = NULL;
            if(!shortest->GetStart()->GetFlag())
                new_vertex = shortest->GetStart();
            else
                if(!shortest->GetEnd()->GetFlag())
                    new_vertex = shortest->GetEnd();
            if(new_vertex)
            {
                //choose this vertices
                new_vertex->GetFlag() = true;
                //choose this edge
                shortest->GetFlag() = true;
                //add new adjanct edges
                QVector<Edge *> IncidentEdges = new_vertex->GetIncidentEdges();
                for(int i=0;i<IncidentEdges.size();i++)
                {
                    if(!IncidentEdges[i]->GetStart()->GetFlag()||!IncidentEdges[i]->GetEnd()->GetFlag())
                        AdjacentEdges.push(IncidentEdges[i]);
                }
            }
        }
    }

    void ComputeEdgeTree()
    {
        for(int i=0;i<m_faces.size();i++)
        {
            m_faces[i]->ComputeFaceAngle();
        }
        //assign curvature to vertices
        for(int i=0;i<m_vertices.size();i++)
        {
            m_vertices[i]->CalValue();
            m_vertices[i]->OrderEdges();
        }
        //assign a weight value to edges, which is an average of two vertices
        for(int i=0;i<m_edges.size();i++)
        {
            m_edges[i]->CalWeight();
        }
//        this->EdgeMST();
        //Priority is higher than preserved
        AssignPreservedCuts();
        AssignPriorityCuts();
    }

    void AssignPriorityCuts()
    {
        for(int i=0;i<m_vertices.size();i++)
        {
            if(m_vertices[i]->Value()>=361)
            {
                QVector<Face*> Inc_faces = m_vertices[i]->GetIncidentFaces();
                QVector<Edge*> Inc_edges = m_vertices[i]->GetIncidentEdges();
                QVector<float> all_face_angles(2*Inc_faces.size());
                float half_total_sum = 0;
                for(int j=0;j<Inc_faces.size();j++)
                {
                    all_face_angles[j] = (Inc_faces[j]->FindFaceAngle(m_vertices[i]));
                    all_face_angles[j+Inc_faces.size()] = all_face_angles[j];
                    half_total_sum += all_face_angles[j];
                }
                half_total_sum /= 2;
                float cur_sum = all_face_angles[0], best_diff = half_total_sum;
                int best_i = 0, best_j = 0;
                for(int i=0, j=1;i<all_face_angles.size()/2;)
                {
                    if(cur_sum<half_total_sum)
                    {
                        cur_sum += all_face_angles[j];
                        j++;
                    }
                    else
                    {
                        cur_sum -= all_face_angles[i];
                        i++;
                    }
                    if(abs(half_total_sum-cur_sum)<best_diff)
                    {
                        best_diff = abs(half_total_sum-cur_sum);
                        best_i = i;
                        if(j>all_face_angles.size()/2-1)
                            best_j = j - all_face_angles.size()/2;
                        else
                            best_j = j;
                    }
                }
                Inc_edges[best_i]->GetFlag() = 1;
                Inc_edges[best_j]->GetFlag() = 1;
            }
        }

    }


    void AssignPreservedCuts()
    {
        for(int i=0;i<m_vertices.size();i++)
        {
            if(m_vertices[i]->Value()<361&&m_vertices[i]->Value()>359)
            {
                QVector<Edge *> Inc_edges = m_vertices[i]->GetIncidentEdges();
                for(int j=0;j<Inc_edges.size();j++)
                {
                    //this kind of edge must be preserved
                    Inc_edges[j]->GetFlag() = -1;
                }
            }
        }
    }

    virtual QVector <Vertex* > GetVertices() { return m_vertices; }
    virtual QVector <Face* > GetFaces() { return m_faces; }
    virtual QVector <Edge* > GetEdges() { return m_edges; }

protected:
    virtual Edge* EdgeExisting(Vertex* s, Vertex* e) {
        for (int i=0; i<m_edges.size(); ++i) {
            if ((m_edges[i]->GetEnd() == e && m_edges[i]->GetStart() == s) ||
                (m_edges[i]->GetEnd() == s && m_edges[i]->GetStart() == e))
                return m_edges[i];
        }
        return NULL;
    }
    class CompareEdge {
        public:
        bool operator()(Edge *e1, Edge *e2) // Returns true if t1 is earlier than t2
        {
            if (e1->Weight() <= e2->Weight())
               return true;
           else
               return false;
        }
    };





    QVector <Vertex* > m_vertices;
    QVector <Edge* > m_edges;
    QVector <Face* > m_faces;
};

#endif // MESH_H
