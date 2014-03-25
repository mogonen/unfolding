#include "extendedmesh.h"
#include <QMessageBox>
#include <QMatrix4x4>
#include <QDebug>
#include <queue>


#include <iostream>
#include <math.h>

ExtendedMesh::ExtendedMesh() {
    m_scale = 1.0;
    isInitialized = false;
    isFoldedMeshReady = false;
    foldedMesh = NULL;
    unfoldedMesh = NULL;
}

bool ExtendedMesh::RebuildMesh(qreal scale, bool rebuild) {

    //bug m_triangleMesh is null, m_tirangleMesh removed(Youyou Wang 11/04/2013)
//    if (m_triangleMesh == NULL || scale> MAX_VALUE/100.0 || scale <MIN_VALUE/100.0)
//        return false;
    if (scale> MAX_VALUE/100.0 || scale <MIN_VALUE/100.0)
        return false;
    else
        if (m_scale == scale)
            return false;

    for (int i=0; i<m_faces.size(); ++i) {
        ((ExtendedFace *)m_faces[i])->BuildSmallerFace(scale);
        QVector <Edge* > edges = m_faces[i]->GetEdges();
        for (int j=0; j<edges.size(); ++j) {
            ExtendedFace* f = (ExtendedFace *)(edges[j]->GetAnotherFace(m_faces[i]));
            ((ExtendedEdge* )edges[j])->BuildConnector((ExtendedFace*)m_faces[i], f, scale);
        }
    }

    m_scale = scale;
    if(rebuild)
    {
        BuildFoldedMesh();
        BuildUnFoldedMesh();
    }
    return true;
}

bool ExtendedMesh::BuildFoldedMesh() {
    if (isInitialized == false)
        InitialExtendedMesh();

    if (foldedMesh != NULL) {
        try {
            delete foldedMesh;
        } catch(...) {
            QMessageBox::critical(NULL, "Error", "Invalid pointer.");
            return false;
        }
    }

    std::cout << "Creating folded base mesh" << std::endl;
    try {
        foldedMesh = new BaseMesh();
    } catch(...) {
        QMessageBox::critical(NULL, "Error", "Not enough memory!");
        return false;
    }
    QVector <QPair <QVector3D, Vertex*> > vertexTable;

    // create folded faces
    for (int i=0; i<m_faces.size(); ++i) {
        QVector <QVector3D> vts = ((ExtendedFace*)m_faces[i])->GetFoldedSmallerFace();
        QVector <Vertex* > vface;
        for (int j=0; j<vts.size(); ++j) {
            Vertex* v = GetCorrespondingVertex(vts[j], vertexTable, foldedMesh);
            vface.push_back(v);
        }
        foldedMesh->AddFace(vface);
    }

    // create folded branches
    for (int i=0; i<m_edges.size();++i) {
        QVector <Face*> faces = m_edges[i]->IncidentFaces();
        assert(faces.size() == 2);
        ExtendedFace* parent = NULL;

        if (((ExtendedFace*) faces[0])->IsChildFace((ExtendedFace*)faces[1]) == true)
            parent = (ExtendedFace*) faces[0];
        else if (((ExtendedFace*)faces[1])->IsChildFace((ExtendedFace*)faces[0]) == true)
            parent = (ExtendedFace*) faces[1];
        else
            continue;
        QVector <QVector <QVector3D> > curves = ((ExtendedEdge*) m_edges[i])->GetFoldedConnector(parent);
        for (int j=0; j<CONNECTOR_SIZE-1; ++j) {
            QVector <Vertex*> quad;
            Vertex* v = NULL;
            v = GetCorrespondingVertex(curves[0][j], vertexTable, foldedMesh);
            quad.push_back(v);

            v = GetCorrespondingVertex(curves[0][j+1], vertexTable, foldedMesh);
            quad.push_back(v);

            v = GetCorrespondingVertex(curves[1][j+1], vertexTable, foldedMesh);
            quad.push_back(v);

            v = GetCorrespondingVertex(curves[1][j], vertexTable, foldedMesh);
            quad.push_back(v);
            foldedMesh->AddFace(quad);
        }
    }
    return true;
}

bool ExtendedMesh::Unfolding() {

    if (MSTQueue.empty())
        return false;

    ExtendedFace* first = MSTQueue[0];
    // unfold the mesh onto Z plane
    QVector3D normal = QVector3D(0, 0, 1);
    PutZPlane(first);

    for (int i=0; i<MSTQueue.size(); ++i) {
        ExtendedFace* face = MSTQueue[i];
        QVector <ExtendedFace* > childrenFaces = face->GetTreeChildrenFaces();
        for (int j=0; j<childrenFaces.size(); ++j) {
            ExtendedEdge* edge = face->SharedEdge(childrenFaces[j]);
            UnfoldBranch(face, edge, normal);
            UnfoldFace(face, childrenFaces[j], edge, normal);
        }
        /*
        QVector <ExtendedFace* > notChildrenFaces = face->GetNotTreeChildrenFaces();
        for (int j=0; j<notChildrenFaces.size(); ++j) {
            ExtendedEdge* edge = face->SharedEdge(notChildrenFaces[j]);
            UnfoldBranch(face, edge, normal);
        }*/
    }
    return true;
}

Vertex* ExtendedMesh::GetCorrespondingVertex(QVector3D& pos, QVector <QPair <QVector3D, Vertex*> >& table, BaseMesh* mesh) {
    for (int i=0; i<table.size(); ++i) {
        if (table[i].first == pos) return table[i].second;
    }
    Vertex* v = mesh->AddVertex(pos.x(), pos.y(), pos.z());
    table.push_back(QPair <QVector3D, Vertex*> (pos, v));
    return v;
}

bool ExtendedMesh::BuildUnFoldedMesh() {
    if (isInitialized == false)
        InitialExtendedMesh();
    if (unfoldedMesh != NULL) {
        try {
            delete unfoldedMesh;
        } catch(...) {
            QMessageBox::critical(NULL, "Error", "Invalid pointer.");
            return false;
        }
    }

    std::cout << "Unfolding.." << std::endl;
    Unfolding();
    if (MSTQueue.empty())
        return false;
    unfoldedMesh = new BaseMesh();
    QVector <QPair <QVector3D, Vertex*> > vertexTable;

    //copy the same structure from folded to unfolded mesh
    for (int i=0; i<m_faces.size(); ++i) {
        ExtendedFace* face = (ExtendedFace*)m_faces[i];
        // Add each face
        QVector <QVector3D> unfoldFace = face->GetUnfoldedSmallerFace();
        QVector <Vertex*> oneFace;
        for (int j=0; j<unfoldFace.size(); ++j) {
            Vertex* v = GetCorrespondingVertex(unfoldFace[j], vertexTable, unfoldedMesh);
            oneFace.push_back(v);
        }
        unfoldedMesh->AddFace(oneFace);
    }
//    {
//        // Add each connector

//        QVector <ExtendedFace* > children = face->GetTreeChildrenFaces();
//        qDebug()<<children.size();
//        for (int j=0; j<children.size(); ++j) {
//            ExtendedEdge* edge = face->SharedEdge(children[j]);
//            QVector <QVector <QVector3D> > curves = edge->GetUnfoldedConnector(face);
//            if (curves.size() != 2) continue;
//            int size = curves[0].size();
//            for (int k=0; k<size-1; ++k) {
//                QVector <Vertex*> quad;
//                Vertex* s1 = GetCorrespondingVertex(curves[0][k], vertexTable, unfoldedMesh);
//                Vertex* s2 = GetCorrespondingVertex(curves[0][k+1], vertexTable, unfoldedMesh);
//                Vertex* s3 = GetCorrespondingVertex(curves[1][k+1], vertexTable, unfoldedMesh);
//                Vertex* s4 = GetCorrespondingVertex(curves[1][k], vertexTable, unfoldedMesh);
//                quad.push_back(s1);
//                quad.push_back(s2);
//                quad.push_back(s3);
//                quad.push_back(s4);
//                unfoldedMesh->AddFace(quad);
//            }
//        }
//    }

    // create folded branches
    for (int i=0; i<m_edges.size();++i) {
        QVector <Face*> faces = m_edges[i]->IncidentFaces();
        assert(faces.size() == 2);
        ExtendedFace* parent = NULL;

        if (((ExtendedFace*) faces[0])->IsChildFace((ExtendedFace*)faces[1]) == true)
            parent = (ExtendedFace*) faces[0];
        else if (((ExtendedFace*)faces[1])->IsChildFace((ExtendedFace*)faces[0]) == true)
            parent = (ExtendedFace*) faces[1];
        else
            continue;
        QVector <QVector <QVector3D> > curves = ((ExtendedEdge*) m_edges[i])->GetUnfoldedConnector(parent);
        for (int j=0; j<CONNECTOR_SIZE-1; ++j) {
            QVector <Vertex*> quad;
            Vertex* v = NULL;
            v = GetCorrespondingVertex(curves[0][j], vertexTable, unfoldedMesh);
            quad.push_back(v);

            v = GetCorrespondingVertex(curves[0][j+1], vertexTable, unfoldedMesh);
            quad.push_back(v);

            v = GetCorrespondingVertex(curves[1][j+1], vertexTable, unfoldedMesh);
            quad.push_back(v);

            v = GetCorrespondingVertex(curves[1][j], vertexTable, unfoldedMesh);
            quad.push_back(v);
            unfoldedMesh->AddFace(quad);
        }
    }
    return true;
}

QVector3D ExtendedMesh::GetEdgeVerticalVector(const QVector3D& start, const QVector3D& end, ExtendedFace *face, QVector3D normal) {
    if (!face) return QVector3D(0, 0, 0);
    QVector3D U = end - start;
    QVector3D vertical = normal.crossProduct(normal, U);

    QVector <QVector3D> vts = face->GetUnfoldedSmallerFace();
    assert(vts.size() >= 3);
    QVector3D center = (vts[0] + vts[1] + vts[2])/3;
    QVector3D V = center - start;
    if (vertical.dotProduct(vertical, V)>0)
       vertical = -vertical;

    vertical.normalize();
    return vertical;
}

QVector3D ExtendedMesh::GetFirstUnfoldedPos(const QVector3D &oldPos, ExtendedFace *face) {
    int id = -1;
    QVector <QVector3D> foldedSmallFace = face->GetFoldedSmallerFace();
    for (int i=0; i<foldedSmallFace.size(); ++i) {
        if (foldedSmallFace[i] == oldPos)
            id = i;
    }
    QVector <QVector3D> unfoldedSmallFace = face->GetUnfoldedSmallerFace();
    return unfoldedSmallFace[id];
}

bool ExtendedMesh::UnfoldBranch(ExtendedFace *face, ExtendedEdge *edge, QVector3D normal) {
    if (!face || !edge)
        return false;

    edge->ClearUnfoldedConnector(face);
    QVector <QVector <QVector3D> > foldedCurves = edge->GetFoldedConnector(face);
    QVector <QVector3D> unfoldedStartCurve;
    QVector <QVector3D> unfoldedEndCurve;

    QVector3D firstVertex = GetFirstUnfoldedPos(foldedCurves[0][0], face);
    QVector3D secondVertex = GetFirstUnfoldedPos(foldedCurves[1][0], face);

    unfoldedStartCurve.push_back(firstVertex);
    unfoldedEndCurve.push_back(secondVertex);

    QVector <QVector <qreal> > curveAngles = edge->ComputeCurveAngles(face);
    QVector<qreal> SAngle = curveAngles[0];
    QVector<qreal> EAngle = curveAngles[1];

    int size = foldedCurves[0].size();

    QVector3D lastVertical = GetEdgeVerticalVector(unfoldedStartCurve[0], unfoldedEndCurve[0], face, normal);

    QVector3D nowVertical;
    for (int i=0; i<size-1; ++i) {
        QVector3D sEdge = unfoldedEndCurve[i]-unfoldedStartCurve[i];
        sEdge.normalize();
        QVector3D eEdge = unfoldedStartCurve[i]-unfoldedEndCurve[i];
        eEdge.normalize();

        nowVertical = normal.crossProduct(normal, sEdge);
        if (nowVertical.dotProduct(nowVertical, lastVertical) < 0)
            nowVertical = -nowVertical;

        qreal sL = QVector3D(foldedCurves[0][i+1]-foldedCurves[0][i]).length();
        qreal shL = sL*sin(SAngle[i]);
        qreal swL = sL*cos(SAngle[i]);
        QVector3D sNextPos = unfoldedStartCurve[i] + shL*nowVertical + swL*sEdge;
        unfoldedStartCurve.push_back(sNextPos);

        qreal eL = QVector3D(foldedCurves[1][i+1]-foldedCurves[1][i]).length();
        qreal ehL = eL*sin(EAngle[i]);
        qreal ewL = eL*cos(EAngle[i]);
        QVector3D eNextPos = unfoldedEndCurve[i] + ehL*nowVertical + ewL*eEdge;
        unfoldedEndCurve.push_back(eNextPos);

        lastVertical = nowVertical;
    }

    edge->AddUnfoldedConnector(face, unfoldedStartCurve);
    edge->AddUnfoldedConnector(face, unfoldedEndCurve);
    return true;
}

int ExtendedMesh::GetFoldedFaceOrderID(const QVector3D& vertex, ExtendedFace* face) {
    if (!face) return -1;
    QVector <QVector3D> foldedSmallerFaces = face->GetFoldedSmallerFace();
    for (int i=0; i<foldedSmallerFaces.size(); ++i) {
        if (foldedSmallerFaces[i] == vertex) return i;
    }
    return -1;
}

bool ExtendedMesh::UnfoldFace(ExtendedFace *last, ExtendedFace *next, ExtendedEdge *edge, QVector3D normal) {
    next->ClearUnfoldedSmallerFace();
    assert(next->GetUnfoldedSmallerFace().size() == 3);
    QVector <QVector <QVector3D> > unfoldedCurves = edge->GetUnfoldedConnector(last);
    assert(unfoldedCurves.size() == 2);
    int size = unfoldedCurves[0].size();
    QVector3D first = unfoldedCurves[0][size-1];
    QVector3D second = unfoldedCurves[1][size-1];

    // To keep the vertex order in unfolded face is the same with the folded face
    QVector <QVector <QVector3D> > foldedCurves = edge->GetFoldedConnector(last);
    assert(foldedCurves.size() == 2);
    size = foldedCurves[0].size();
    QVector3D foldedFirst = foldedCurves[0][size-1];
    QVector3D foldedSecond = foldedCurves[1][size-1];

    int firstID = GetFoldedFaceOrderID(foldedFirst, next);
    int secondID = GetFoldedFaceOrderID(foldedSecond, next);
    int thirdID = 3-firstID-secondID;

//    QVector <QVector3D > foldedVts = next->GetFoldedSmallerFace();
//    for (int i=0; i<foldedVts.size(); ++i) {
//        if (foldedVts[i] != foldedFirst && foldedVts[i] != foldedSecond) {
//            thirdID = i;
//            break;
//        }
//    }

    QVector3D center = (unfoldedCurves[0][0]+unfoldedCurves[0][1]+unfoldedCurves[0][2]+unfoldedCurves[0][3])/4.0;
    QVector3D C = center - first;
    QVector3D U = second - first;
    U.normalize();
    QVector3D vertical = normal.crossProduct(normal, U);
    if (vertical.dotProduct(vertical, C)>0)
        vertical = -vertical;
    vertical.normalize();


    QVector3D s1 = next->GetFoldedSmallerFace()[thirdID] - next->GetFoldedSmallerFace()[firstID];
    QVector3D s2 = next->GetFoldedSmallerFace()[secondID] - next->GetFoldedSmallerFace()[firstID];

    qreal angle = acos((s1.dotProduct(s1, s2))/(s1.length()*s2.length()));
    qreal L = s1.length();
    qreal hL = L*sin(angle);
    if(sin(angle)<0)
    {
        printf("WARNING!!!!!!!!!!!!!!!!! sin theta is negetive!!!!!!!!!! ");
    }
    qreal wL = L*cos(angle);

    QVector3D third = first + wL*U + hL*vertical;

    next->SetUnfoldedSmallerFace(first, firstID);
    next->SetUnfoldedSmallerFace(second, secondID);
    next->SetUnfoldedSmallerFace(third, thirdID);
    return true;
}


bool ExtendedMesh::PutZPlane(ExtendedFace* face) {
    if (!face) return false;

    face->ClearUnfoldedSmallerFace();
    QVector <QVector3D > foldedTriangle = face->GetFoldedSmallerFace();
    assert(foldedTriangle.size() >= 3);
    QVector3D d1 = foldedTriangle[1] - foldedTriangle[0];
    QVector3D d2 = foldedTriangle[2] - foldedTriangle[0];
    double theta = acos(d1.dotProduct(d1, d2)/(d1.length()*d2.length()));
    double l = d1.length();

    face->SetUnfoldedSmallerFace(QVector3D(0.0, 0.0, 0.0), 0);
    face->SetUnfoldedSmallerFace(QVector3D(l*cos(theta), l*sin(theta), 0.0), 1);
    face->SetUnfoldedSmallerFace(QVector3D(d2.length(), 0.0, 0.0), 2);
    return true;
}

bool ExtendedMesh::AddFace(QVector <Vertex*>& vertices, QVector<int> edge_flags) {
    if (vertices.size()<2)
        return false;

    QVector <ExtendedEdge* > currentEdges;
    for (int i=0; i<vertices.size(); ++i) {
        int firstIndex = i;
        int lastIndex = (i == vertices.size()-1) ? 0 : i+1;
        ExtendedEdge* current = (ExtendedEdge*) EdgeExisting(vertices[firstIndex], vertices[lastIndex]);
        if (!current) {
            try {
                ExtendedEdge* edge = new ExtendedEdge(vertices[firstIndex], vertices[lastIndex],edge_flags[i]);
                m_edges.push_back(edge);
                currentEdges.push_back(edge);
            } catch(...) {
                return false;
            }
        } else
            currentEdges.push_back(current);
    }
    try {
        ExtendedFace* face = new ExtendedFace(vertices, currentEdges);
        m_faces.push_back(face);
    } catch(...) {
        return false;
    }
    return true;
}

bool ExtendedMesh::InitialExtendedMesh() {
    if (m_faces.size()<1)
        return false;

    std::cout << "Initializing extended mesh!" << std::endl;
    root = (ExtendedFace*) m_faces[0];

    MST(root); // create the minial spinning tree rooted at m_faces[0]
    RebuildMesh(0.8);
    isInitialized = true;
    return true;
}

bool ExtendedMesh::MST(ExtendedFace *first) {
    if (first == NULL) return false;
    std::cout << "Creating MST..." << std::endl;
    // clear the saved MST queue
    for(int i=0;i<MSTQueue.size();i++)
        MSTQueue[i]->ClearNeighbors();
    MSTQueue.clear();

    QQueue <Face*> queue;
    QSet <Face*> faceUsed;
    queue.push_back(first);
    faceUsed.insert(first);

    while (!queue.empty()) {
        ExtendedFace* current = (ExtendedFace*)queue.first();
        queue.pop_front();
        MSTQueue.push_back(current);
        QVector <Edge*> edges = current->GetEdges();
//        QVector <Face*> incidentFaces;
//        incidentFaces.reserve(edges.size());

        for (int i=0; i<edges.size(); ++i)
        {

            if(edges[i]->GetFlag()!=1)
            {
                Face* ind_face = edges[i]->GetAnotherFace(current);

                if (faceUsed.find(ind_face) == faceUsed.end()) {
                    faceUsed.insert(ind_face);
                    queue.push_back(ind_face);
                    current->AddTreeChildFace((ExtendedFace*)ind_face, 0);
                }
                else
                {
                    if(!edges[i]->GetFlag()==-1)
                        current->AddTreeChildFace((ExtendedFace*)ind_face, 0);
                }

            }
        }
//        for (int i=0; i<incidentFaces.size(); ++i) {
//            if (faceUsed.find(incidentFaces[i]) == faceUsed.end()) {
//                current->AddTreeChildFace((ExtendedFace*)incidentFaces[i], 0);
//                queue.push_back(incidentFaces[i]);
////                ((ExtendedEdge*)edges[i])->SetAsConnector();
//                faceUsed.insert(incidentFaces[i]);
//            }
////            else
////                current->AddTreeNotChildFace((ExtendedFace*)incidentFaces[i]);
//        }
    }



//    QQueue <Face*> queue;
//    QSet <Face*> faceUsed;
//    queue.push_back(first);
//    faceUsed.insert(first);

//    while (!queue.empty()) {
//        ExtendedFace* current = (ExtendedFace*)queue.first();
//        queue.pop_front();
//        MSTQueue.push_back(current);
//        QVector <Edge*> edges = current->GetEdges();
//        QVector <Face*> incidentFaces(edges.size(), NULL);

//        for (int i=0; i<edges.size(); ++i)
//            incidentFaces[i] = edges[i]->GetAnotherFace(current);
//        for (int i=0; i<incidentFaces.size(); ++i) {
//            if (faceUsed.find(incidentFaces[i]) == faceUsed.end()) {
//                current->AddTreeChildFace((ExtendedFace*)incidentFaces[i]);
//                queue.push_back(incidentFaces[i]);
//                ((ExtendedEdge*)edges[i])->SetAsConnector();
//                faceUsed.insert(incidentFaces[i]);
//            } else
//                current->AddTreeNotChildFace((ExtendedFace*)incidentFaces[i]);
//        }
//    }
    return true;
}


