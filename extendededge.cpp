#include "extendededge.h"
#include <iostream>
#include <math.h>


bool ExtendedEdge::BuildConnector(ExtendedFace* parent, ExtendedFace* child, qreal scale) {
    // if the edge is not a connector, return
  //  if (m_is_connector == false) return false;
    // if the scale is not in range, return

    if (scale < 0.0 || scale > 1.0) return false;

    // clear the former curvers
    m_connector[parent].clear();

    QVector3D parentCenter = parent->GetCenter();
    QVector3D childCenter = child->GetCenter();

    // Create two Bezier curves
    QVector <QVector3D> firstCurve = BezierCurve(parentCenter, childCenter, m_start, scale, CONNECTOR_SIZE);
    QVector <QVector3D> secondCurve = BezierCurve(parentCenter, childCenter, m_end, scale, CONNECTOR_SIZE);

    QVector <Vertex* > parentVertex = parent->GetVertices();
    bool isFirstFound = false;
    bool isSecondFound = false;
    for (int i=0; i<parentVertex.size(); ++i) {
        if (parentVertex[i] == m_start) {
            isFirstFound = true;
            break;
        } else if (parentVertex[i] == m_end) {
            isSecondFound = true;
            break;
        }
    }
    if (isFirstFound) {
        m_connector[parent].push_back(firstCurve);
        m_connector[parent].push_back(secondCurve);
    } else {
        m_connector[parent].push_back(secondCurve);
        m_connector[parent].push_back(firstCurve);
    }

    return true;
}

QVector <QVector3D> ExtendedEdge::BezierCurve(QVector3D parentCenter, QVector3D childCenter, Vertex* endPoint, qreal scale, int size) {
    QVector <QVector3D> curve;
    qreal x = (1.0-scale)*parentCenter.x() + scale*endPoint->x();
    qreal y = (1.0-scale)*parentCenter.y() + scale*endPoint->y();
    qreal z = (1.0-scale)*parentCenter.z() + scale*endPoint->z();
    curve.push_back(QVector3D(x,y,z));
    x = (1.0-scale)*m_center.x() + scale*endPoint->x();
    y = (1.0-scale)*m_center.y() + scale*endPoint->y();
    z = (1.0-scale)*m_center.z() + scale*endPoint->z();
    curve.push_back(QVector3D(x,y,z));
    x = (1.0-scale)*childCenter.x() + scale*endPoint->x();
    y = (1.0-scale)*childCenter.y() + scale*endPoint->y();
    z = (1.0-scale)*childCenter.z() + scale*endPoint->z();
    curve.push_back(QVector3D(x,y,z));

    while (curve.size() < size) {
        QVector <QVector3D> tempCurve;
        int current_size = curve.size();
        std::swap(tempCurve, curve);
        curve.push_back(tempCurve[0]);
        for (int i=0; i<current_size-1; ++i) {
            qreal x = (tempCurve[i].x() + tempCurve[i+1].x())/2.0;
            qreal y = (tempCurve[i].y() + tempCurve[i+1].y())/2.0;
            qreal z = (tempCurve[i].z() + tempCurve[i+1].z())/2.0;
            curve.push_back(QVector3D(x, y, z));
        }
        curve.push_back(tempCurve[current_size-1]);
    }
    return curve;
}


QVector <QVector <qreal> > ExtendedEdge::ComputeCurveAngles(Face* f) {
    QVector <QVector <qreal> > curveAngles;
    assert(m_connector[f].size()==2);
    int size = m_connector[f][0].size();
    QVector <qreal> startCurve;
    QVector <qreal> endCurve;

    for (int i=0; i<size-1; ++i) {
        QVector3D s1 = m_connector[f][0][i+1] - m_connector[f][0][i];
        QVector3D s2 = m_connector[f][1][i] - m_connector[f][0][i];
        qreal angle = acos((s1.dotProduct(s1, s2))/(s1.length()*s2.length()));
        startCurve.push_back(angle);
    }
    for (int i=0; i<size-1; ++i) {
        QVector3D s1 = m_connector[f][1][i+1] - m_connector[f][1][i];
        QVector3D s2 = m_connector[f][0][i] - m_connector[f][1][i];
        qreal angle = acos((s1.dotProduct(s1, s2))/(s1.length()*s2.length()));
        endCurve.push_back(angle);
    }
    curveAngles.push_back(startCurve);
    curveAngles.push_back(endCurve);
    return curveAngles;
}



























