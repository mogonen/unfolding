#include "face.h"
#include "vertex.h"
#include <QDebug>

void Vertex::CalValue()
{
    m_value = 0;
    for(int i=0;i<m_edges.size();i++)
    {
        m_value += m_edges[i]->GetFace(this)->FindFaceAngle(this);
    }
}

Edge * Vertex::FindConnectingEdge(Vertex* other_v)
{
    for(int i=0;i<m_edges.size();i++)
    {
        if(m_edges[i]->GetStart()==other_v||m_edges[i]->GetEnd()==other_v)
            return m_edges[i];
    }
    return NULL;
}

void Vertex::OrderEdges()
{
    m_faces.resize(m_edges.size());
    m_faces[0] = m_edges[0]->GetFace(this);
    for(int i=0;i<m_edges.size()-1;i++)
    {
        Face* ind_face = m_edges[i]->GetOtherFace(this);
        for(int j=i+1;j<m_edges.size();j++)
        {
            if(ind_face == m_edges[j]->GetFace(this))
            {
                Edge* temp_e = m_edges[j];
                m_edges[j] = m_edges[i+1];
                m_edges[i+1] = temp_e;
                m_faces[i+1] = ind_face;
                break;
            }
        }
    }
}
