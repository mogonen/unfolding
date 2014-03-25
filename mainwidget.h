/****************************************************************************
 **
 ** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the QtCore module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
 **     of its contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "basemesh.h"
#include "extendedmesh.h"
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFunctions>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <GL/glu.h>

class QBasicTimer;
class QGLShaderProgram;

class GeometryEngine;

class MainWidget : public QGLWidget, protected QGLFunctions
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    virtual ~MainWidget();

    void Reset() {
        translateX = 0.0f;
        translateY = 0.0f;
        translateZ = -5.0f;
        rotation = QQuaternion(1.0, 0.0, 0.0, 0.0);
        baseMesh = NULL;
        triangleMesh = NULL;
        extendedMesh = NULL;
        isBaseMeshReady = false;
        isTriangleMeshReady = false;
        isFoldedMeshReady = false;
        isUnfoldedMeshReady = false;
    }

    bool SetBaseMesh(BaseMesh* mesh) {
        isBaseMeshReady = true;
        baseMesh = mesh;
        return true;
    }

    bool SetTriangleMesh(BaseMesh* mesh) {
        isTriangleMeshReady = true;
        triangleMesh = mesh;
        return true;
    }

    bool SetExtendedMesh(ExtendedMesh* mesh) {
        isFoldedMeshReady = true;
        isUnfoldedMeshReady = true;
        extendedMesh = mesh;
        return true;
    }

    void ShowOrignal(bool status) { isShowOriginal = status; }
    void ShowTriangle(bool status) { isShowTriangle = status; }
    void ShowFolded(bool status) { isShowFolded = status; }
    void ShowUnfolded(bool status) { isShowUnfolded = status; }

    void ShowVertexData(bool status) { isShowVertexData = status; }
    void ShowShaded(bool status) { isShowShading = status; }

signals:

public slots:
    void keyPressEvent(QKeyEvent *);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* e);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void initShaders();
    void initTextures();

private:

    //handling selection etc starts
    void ComputeGlobalSelectionBox();
    void ModelViewTransform(float *input, double *modelview);
    bool IsInSelectBox (const QVector3D& pt);
    void SelectNonGL(QMap<int, int> &indices, bool add = false, bool remove= false);
    void LoadGLParameters();
    void QtProject(const QVector3D& pos_world, const QMatrix4x4& modelview, const QMatrix4x4& projection, int* viewport, QVector3D& pos_camera);
    bool QtUnProject(const QVector3D& pos_screen, const QMatrix4x4& modelview, const QMatrix4x4& projection, int* viewport, QVector3D& pos_world);
    void SelectFace(QMouseEvent *e, QMap<int, int>& m_selectedFaces, bool add = false, bool remove = false);
    int GLPickAFace(GLdouble winX,  GLdouble winY);
    void UTILS_color_hue_to_rgb (float hue, float *rgb);
    void TranslateFolded(float x, float y, float z);
    void TransformFolded(const QMatrix4x4& transformation);
    void TransformUnfolded(const QMatrix4x4& transformation);

    //find a transfroatmion between folded and unfolded face of index ind
    bool FindFaceTransformation(Face* f1, Face* f2, QMatrix4x4& transf);

    int m_isSelectionMode;
    bool m_bShowSelectionBox;
    QMap<int, int> m_selectedFaces;

    QMap<int, int> m_selectedIndices;
    GLint m_GLviewport[4];
    GLdouble m_GLmodelview[16];
    GLdouble m_GLprojection[16];
    bool m_CameraChanged;
    bool m_CurrentControl;
    QMatrix4x4 m_ControlMatrix;

    struct SelectionBox
    {
        GLdouble corner_win[4];
        GLdouble corner_global[12];

    }m_SelectingBox;
    //handling selection etc ends


    QGLShaderProgram *program;
    GeometryEngine *geometries;

    GLuint texture;

    QMatrix4x4 modelview;
    QMatrix4x4 projection;

    QVector2D mousePressPosition;
    QVector3D rotationAxis;  // Rotation axis is perpendicular to the mouse position difference
    qreal angularChange;  // Drag to rotate
    QQuaternion rotation;
    qreal translateX;  // Translate object in X, Y, Z directions
    qreal translateY;
    qreal translateZ;

    BaseMesh* baseMesh;  // NOT OWNED
    BaseMesh* triangleMesh; // NOT OWNED
    ExtendedMesh* extendedMesh; // NOT OWNED

    bool isBaseMeshReady;
    bool isTriangleMeshReady;
    bool isFoldedMeshReady;
    bool isUnfoldedMeshReady;

    bool isShowOriginal;
    bool isShowTriangle;
    bool isShowFolded;
    bool isShowUnfolded;
    bool isShowShading;
    bool isShowVertexData;
};

#endif // MAINWIDGET_H
