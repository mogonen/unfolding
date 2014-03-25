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

#include "mainwidget.h"

#include "geometryengine.h"

#include <QtOpenGL/QGLShaderProgram>

#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include <math.h>
#include <locale.h>
#include <iostream>



MainWidget::MainWidget(QWidget *parent) :
    QGLWidget(parent),
    program(new QGLShaderProgram),
    geometries(new GeometryEngine)
{
    translateX = 0.0f;
    translateY = 0.0f;
    translateZ = -5.0f;
    isBaseMeshReady = false;
    isTriangleMeshReady = false;
    isUnfoldedMeshReady = false;
    isFoldedMeshReady = false;

    isShowOriginal = true;
    isShowTriangle = true;
    isShowFolded = true;
    isShowUnfolded = true;
    isShowVertexData = true;
    isShowShading = true;

    m_isSelectionMode = false;
    m_CurrentControl = false;
}

MainWidget::~MainWidget()
{
    delete program; program = 0;
    delete geometries; geometries = 0;

    //    deleteTexture(texture);
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Saving mouse press position
    mousePressPosition = QVector2D(e->pos());

    //Load new ModelView/Projection/Viewport parameter if there is any change of camera
    if(m_CameraChanged)
        LoadGLParameters();

    if((e->modifiers() & Qt::ShiftModifier))
    {
        GLdouble winX, winY;               // Holds Our X, Y and Z Coordinates

        winX = e->x();                  // Holds The Mouse X Coordinate
        winY = e->y();                  // Holds The Mouse Y Coordinate
        winY = (float)m_GLviewport[3] - winY;           // Subtract The Current Mouse Y Coordinate From The Screen Height.


        m_SelectingBox.corner_win[0] = winX;
        m_SelectingBox.corner_win[1] = winY;
    }
    if((e->buttons() & Qt::RightButton))
    {
        if(e->modifiers()&Qt::ControlModifier)
            SelectFace(e, m_selectedFaces, true);
        else
            if(e->modifiers()&Qt::AltModifier)
                SelectFace(e, m_selectedFaces, false, true);
            else
                SelectFace(e, m_selectedFaces);
    }
    updateGL();
}

void MainWidget::mouseMoveEvent(QMouseEvent *e)
{

    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->pos()) - mousePressPosition;

    //mesh controls
    if((e->modifiers() & Qt::ShiftModifier) && (e->buttons() & Qt::LeftButton))
    {
        GLdouble winX, winY;               // Holds Our X, Y and Z Coordinates

        winX = e->x();                  // Holds The Mouse X Coordinate
        winY = e->y();                  // Holds The Mouse Y Coordinate
        winY = (float)m_GLviewport[3] - winY;           // Subtract The Current Mouse Y Coordinate From The Screen Height.

        m_SelectingBox.corner_win[2] = winX;
        m_SelectingBox.corner_win[3] = winY;
        ComputeGlobalSelectionBox();
        if((e->modifiers() & Qt::ControlModifier))
            SelectNonGL(m_selectedIndices, true);
        else
            if((e->modifiers() & Qt::AltModifier))
                SelectNonGL(m_selectedIndices, true,true);
            else
                SelectNonGL(m_selectedIndices);
        m_bShowSelectionBox = true;
    }
    else
    {
        // Rotation axis is perpendicular to the mouse position difference
        // vector
        QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
        // Accelerate angular speed relative to the length of the mouse sweep
        qreal acc = diff.length() / 4.0;
        // Calculate new rotation axis as weighted sum
        rotationAxis = (rotationAxis * angularChange + n * acc).normalized();
        // Change rotation angle
        angularChange = acc;

        //mesh control
        if(m_CurrentControl)
        {
            if( (e->buttons() & Qt::RightButton) )
            {
                m_ControlMatrix.translate(rotation.conjugate().rotatedVector(QVector3D(-diff.x()/translateZ/20, diff.y()/translateZ/20, 0.0)));
            }
            else
            {
                QQuaternion temp = QQuaternion::fromAxisAndAngle(rotation.conjugate().rotatedVector(rotationAxis), angularChange);
                m_ControlMatrix.rotate(temp);
            }
            TransformFolded(m_ControlMatrix);
            m_ControlMatrix.setToIdentity();
        }
        else
        //camera controls
        {
            if((e->modifiers() & Qt::AltModifier) ) {
                translateX += diff.x()/100;
                translateY -= diff.y()/100;
            }
            else
                if(e->modifiers() & Qt::ControlModifier)
                {
                    translateZ += diff.x()/100-diff.y()/100;
                }
                else{
                    rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularChange) * rotation;
                }
        }
        mousePressPosition = QVector2D(e->pos());
        // Update scene
        m_CameraChanged = true;
    }

    updateGL();
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if((e->modifiers() & Qt::ShiftModifier))
    {
        if((e->modifiers() & Qt::ControlModifier))
            SelectNonGL(m_selectedIndices, true);
        else
            if((e->modifiers() & Qt::AltModifier))
                SelectNonGL(m_selectedIndices, true,true);
            else
                SelectNonGL(m_selectedIndices);
    }

    m_bShowSelectionBox = false;
    updateGL();
}

void MainWidget::wheelEvent(QWheelEvent* e) {
    double numSteps = e->delta()/100.0f;
    translateZ += numSteps;
    m_CameraChanged = true;
    updateGL();
}


void MainWidget::initializeGL()
{
    initializeGLFunctions();
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    qglClearColor(Qt::white);

    qDebug() << "Initializing shaders...";
    initShaders();

    //  qDebug() << "Initializing textures...";
    //  initTextures();

    // Enable depth buffer
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_ALWAYS);
    // Enable back face culling
    // glEnable(GL_CULL_FACE);

    qDebug() << "Initializing geometries...";


//    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//    GLfloat mat_shininess[] = { 50.0 };
//    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
//    glClearColor (0.0, 0.0, 0.0, 0.0);
//    glShadeModel (GL_SMOOTH);

//    float colour[4] = {0,0,0,0};

//    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,colour) ;
//    glLightfv(GL_LIGHT0,GL_SPECULAR,colour);
//    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);

    geometries->init();
    m_CameraChanged = false;
    m_bShowSelectionBox = false;
}

void MainWidget::initShaders()
{
    // Overriding system locale until shaders are compiled
    setlocale(LC_NUMERIC, "C");

    // Compiling vertex shader
    if (!program->addShaderFromSourceFile(QGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compiling fragment shader
    if (!program->addShaderFromSourceFile(QGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Linking shader pipeline
//    if (!program->link())
//        close();

    // Binding shader pipeline for use
//    if (!program->bind())
//        close();

    // Restore system locale
    setlocale(LC_ALL, "");
}

void MainWidget::initTextures()
{
    // Loading cube.png to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    texture = bindTexture(QImage(":/cube.png"));

    // Set nearest filtering mode for texture minification
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set bilinear filtering mode for texture magnification
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void MainWidget::resizeGL(int w, int h)
{
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    // Calculate aspect ratio
    qreal aspect = (qreal)w / ((qreal)h?h:1);

    // Set near plane to 1.0, far plane to 7000.0, field of view 45 degrees
    const qreal zNear = 1.0, zFar = 7000.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(projection.constData());
    m_CameraChanged = true;
}

void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(translateX, translateY, translateZ);
    matrix.rotate(rotation);
    modelview = matrix;
//    // Set modelview-projection matrix
//    program->setUniformValue("mvp_matrix", projection * matrix);

    // Using texture unit 0 which contains cube.png
    // program->setUniformValue("texture", 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMultMatrixf(modelview.constData());

//    GLfloat light0_pf[] = { 3.0, 3.0, 3.0, 0.0 };
//    glLightfv(GL_LIGHT0, GL_POSITION, light0_pf);
//    glEnable(GL_LIGHT0);
//    glBegin(GL_POINTS);
//    glColor3f(1.0,1.0,1.0);
//    glVertex3f(3.0,3.0,3.0);
//    glEnd();
    // Draw the base mesh
    unsigned int flags = 0;
    flags = flags | (GeometryEngine::SHADED * isShowShading);
    flags = flags | (GeometryEngine::VERTEX_DATA* isShowVertexData);

    if (isBaseMeshReady && isShowOriginal) {
        geometries->drawBaseMesh(baseMesh, this, flags);
    }
    if (isTriangleMeshReady && isShowTriangle) {
        geometries->drawBaseMesh(triangleMesh,this,flags);
    }

    if (isFoldedMeshReady && isShowFolded) {
        BaseMesh* baseFoldedMesh = extendedMesh->GetFoldedMesh();
        geometries->drawBaseMesh(baseFoldedMesh);
//        qDebug()<<"No. of faces of folded: "<<baseFoldedMesh->GetFaces().size();
//        qDebug()<<"No. of vertices of folded: "<<baseFoldedMesh->GetVertices().size();
//        qDebug()<<"No. of edges of folded: "<<baseFoldedMesh->GetEdges().size();
//        qDebug()<<baseFoldedMesh->GetVertices()[0]->GetIncidentEdges().size();
//        qDebug()<<"";
    }
    if (isUnfoldedMeshReady && isShowUnfolded) {
        BaseMesh* baseUnfoldedMesh = extendedMesh->GetUnFoldedMesh();
        geometries->drawBaseMesh(baseUnfoldedMesh);
//        qDebug()<<"No. of faces of unfolded: "<<baseUnfoldedMesh->GetFaces().size();
//        qDebug()<<"No. of vertices of unfolded: "<<baseUnfoldedMesh->GetVertices().size();
//        qDebug()<<"No. of edges of unfolded: "<<baseUnfoldedMesh->GetEdges().size();
//        qDebug()<<"";
    }

    QMap<int,int>::Iterator it;
    for(it = m_selectedIndices.begin();it!=m_selectedIndices.end();it++)
    {
        int ind = it.value();
        glBegin(GL_POINTS);
        if (isFoldedMeshReady && isShowFolded) {
            glColor3f(1.0,0.0,0.0);
            glVertex3f(extendedMesh->GetFoldedMesh()->GetVertices()[ind]->x(), extendedMesh->GetFoldedMesh()->GetVertices()[ind]->y(), extendedMesh->GetFoldedMesh()->GetVertices()[ind]->z());
        }
        if (isUnfoldedMeshReady && isShowUnfolded) {
            glColor3f(0.0,1.0,0.0);
            glVertex3f(extendedMesh->GetUnFoldedMesh()->GetVertices()[ind]->x(), extendedMesh->GetUnFoldedMesh()->GetVertices()[ind]->y(), extendedMesh->GetUnFoldedMesh()->GetVertices()[ind]->z());
        }
        glEnd();
        glColor4f(1.0,1.0,0.0,1.0);
        if (isFoldedMeshReady && isShowFolded && isUnfoldedMeshReady && isShowUnfolded) {
            glBegin(GL_LINES);
            glVertex3f(extendedMesh->GetFoldedMesh()->GetVertices()[ind]->x(), extendedMesh->GetFoldedMesh()->GetVertices()[ind]->y(), extendedMesh->GetFoldedMesh()->GetVertices()[ind]->z());
            glVertex3f(extendedMesh->GetUnFoldedMesh()->GetVertices()[ind]->x(), extendedMesh->GetUnFoldedMesh()->GetVertices()[ind]->y(), extendedMesh->GetUnFoldedMesh()->GetVertices()[ind]->z());
            glEnd();
        }
    }

//    qDebug()<<test_p;
//    glBegin(GL_POINTS);
//    glVertex3d(test_p.x(),test_p.y(),test_p.z());
//    glEnd();

    glColor4f(0.0, 0.0, 0.0, 1.0);
    if(m_CurrentControl)
    {
        QFont font;
        font.setPointSize(15);
        renderText(10, 20,QString("Foled Mesh Mode"));
    }
    else
    {
        QFont font;
        font.setPointSize(15);
        renderText(10, 20,QString("Camera Mode"));
    }


    if(m_bShowSelectionBox)
    {
        //draw selection box
        glColor4f(0.0, 1.0, 19.0/255, 0.2);
        glBegin(GL_QUADS);
        glVertex3dv(m_SelectingBox.corner_global);
        glVertex3dv(m_SelectingBox.corner_global+3);
        glVertex3dv(m_SelectingBox.corner_global+6);
        glVertex3dv(m_SelectingBox.corner_global+9);
        glEnd();

        //draw selection box
        glColor4f(0.0, 1.0, 19.0/255, 0.5);
        glBegin(GL_LINE_LOOP);
        glVertex3dv(m_SelectingBox.corner_global);
        glVertex3dv(m_SelectingBox.corner_global+3);
        glVertex3dv(m_SelectingBox.corner_global+6);
        glVertex3dv(m_SelectingBox.corner_global+9);
        glEnd();
    }

    if(m_selectedFaces.size())
    {
        QMap<int,int>::Iterator it;
        int m=0;
        for(it = m_selectedFaces.begin();it!=m_selectedFaces.end();it++,m++)
        {
            int ind = it.value();
            float hue_color[4];
            hue_color[3] = 0.5;
//            qDebug()<<ind;
//            qDebug()<<extendedMesh->GetFoldedMesh()->GetFaces().size();
//            qDebug()<<extendedMesh->GetUnFoldedMesh()->GetFaces().size();

            UTILS_color_hue_to_rgb((float)m/(float)(m_selectedFaces.size()), hue_color);
            glColor4fv(hue_color);
            QVector <Vertex* > vertices;
            if (isFoldedMeshReady && isShowFolded && ind<extendedMesh->GetFoldedMesh()->GetFaces().size()) {
                vertices = (extendedMesh->GetFoldedMesh()->GetFaces()[ind])->GetVertices();
                glBegin(GL_POLYGON);
                for (int j=0; j<vertices.size(); ++j)
                    glVertex3f(vertices[j]->x(), vertices[j]->y(), vertices[j]->z());
                glEnd();
            }
//            qDebug()<<"Face No."<<m<<"folded edges:"<<(*vertices[0]-*vertices[1]).length()<<(*vertices[1]-*vertices[2]).length()<<(*vertices[2]-*vertices[0]).length();
            if (isFoldedMeshReady && isShowFolded && ind<extendedMesh->GetUnFoldedMesh()->GetFaces().size()) {
                vertices = (extendedMesh->GetUnFoldedMesh()->GetFaces()[ind])->GetVertices();
                glBegin(GL_POLYGON);
                for (int j=0; j<vertices.size(); ++j)
                    glVertex3f(vertices[j]->x(), vertices[j]->y(), vertices[j]->z());
                glEnd();
            }
//            qDebug()<<"Face No."<<m<<"unfolded edges:"<<(*vertices[0]-*vertices[1]).length()<<(*vertices[1]-*vertices[2]).length()<<(*vertices[2]-*vertices[0]).length();
        }
//        qDebug()<<"";
   }
}


void MainWidget::LoadGLParameters()
{
    //Load openGL ModelView/Project/Viewpot parameters
    glGetIntegerv(GL_VIEWPORT, m_GLviewport);           // Retrieves The Viewport Values (X, Y, Width, Height)
    glGetDoublev(GL_MODELVIEW_MATRIX, m_GLmodelview);       // Retrieve The Modelview Matrix
    glGetDoublev(GL_PROJECTION_MATRIX, m_GLprojection);     // Retrieve The Projection Matrix

    //Not know why, but it solves the problem, maybe some issue with QT
    if(width()<height())
        m_GLviewport[1] = -m_GLviewport[1];

    //After this function is called, set the CameraChanged to be false to prevent recall of this function
    m_CameraChanged = false;

    QMatrix4x4 matrix;
//    qDebug()<<translateX<<translateY<<translateZ<<rotation;
    matrix.translate(translateX, translateY, translateZ);
    matrix.rotate(rotation);
    modelview = matrix;
}


void MainWidget::ComputeGlobalSelectionBox()
{
    GLdouble winX = m_SelectingBox.corner_win[0];
    GLdouble winY = m_SelectingBox.corner_win[1];
    gluUnProject( winX, winY, 0.0001, m_GLmodelview, m_GLprojection, m_GLviewport, m_SelectingBox.corner_global, m_SelectingBox.corner_global+1, m_SelectingBox.corner_global+2);//The new position of the mouse
//    qDebug()<<m_SelectingBox.corner_global[0]<<m_SelectingBox.corner_global[1]<<m_SelectingBox.corner_global[2];
    QVector3D temp_res;
    QtUnProject(QVector3D(winX,winY,0.001),modelview, projection,m_GLviewport,temp_res);
//    qDebug()<<temp_res;
    m_SelectingBox.corner_global[0] = temp_res.x();
    m_SelectingBox.corner_global[1] = temp_res.y();
    m_SelectingBox.corner_global[2] = temp_res.z();

    winX = m_SelectingBox.corner_win[0];
    winY = m_SelectingBox.corner_win[3];
    QtUnProject(QVector3D(winX,winY,0.001),modelview, projection,m_GLviewport,temp_res);
    m_SelectingBox.corner_global[3] = temp_res.x();
    m_SelectingBox.corner_global[4] = temp_res.y();
    m_SelectingBox.corner_global[5] = temp_res.z();

    winX = m_SelectingBox.corner_win[2];
    winY = m_SelectingBox.corner_win[3];
    gluUnProject( winX, winY, 0.0001, m_GLmodelview, m_GLprojection, m_GLviewport, m_SelectingBox.corner_global+6, m_SelectingBox.corner_global+7, m_SelectingBox.corner_global+8);//The new position of the mouse
    winX = m_SelectingBox.corner_win[2];
    winY = m_SelectingBox.corner_win[1];
    gluUnProject( winX, winY, 0.0001, m_GLmodelview, m_GLprojection, m_GLviewport, m_SelectingBox.corner_global+9, m_SelectingBox.corner_global+10, m_SelectingBox.corner_global+11);//The new position of the mouse
}

void MainWidget::ModelViewTransform(float *input, double *modelview)
{
    float x = input[0];
    float y = input[1];
    float z = input[2];
    for(int i=0;i<3;i++)
    {
        input[i] = modelview[i]*x+modelview[i+4]*y+modelview[i+8]*z+modelview[i+12];
    }
}

void MainWidget::QtProject(const QVector3D& pos_world, const QMatrix4x4& modelview, const QMatrix4x4& projection, int* viewport, QVector3D& pos_screen)
{
    QVector4D homo_coord(pos_world, 1.0f);
    QVector3D pos_camera = (projection*modelview*homo_coord).toVector3DAffine();
    pos_screen.setX((pos_camera.x()*0.5+0.5)*viewport[2]+viewport[0]);
    pos_screen.setY((pos_camera.y()*0.5+0.5)*viewport[3]+viewport[1]);
    pos_screen.setZ((1.0+pos_camera.z())*0.5);
}

bool MainWidget::QtUnProject(const QVector3D& pos_screen, const QMatrix4x4& modelview, const QMatrix4x4& projection, int* viewport, QVector3D& pos_world)
{
    bool isInvertible;
    QMatrix4x4 proj_modelview_inv = (projection*modelview).inverted(&isInvertible);
    if(isInvertible)
    {
        QVector3D pos_camera;
        pos_camera.setX((pos_screen.x()-(float)viewport[0])/(float)viewport[2]*2.0-1.0);
        pos_camera.setY((pos_screen.y()-(float)viewport[1])/(float)viewport[3]*2.0-1.0);
        pos_camera.setZ(2.0*pos_camera.z()-1.0);
        pos_world = (proj_modelview_inv*QVector4D(pos_camera, 1.0f)).toVector3DAffine();
    }
    else
    {
        return isInvertible;
    }
}

bool MainWidget::IsInSelectBox (const QVector3D& pt)
{
    double origin_x_ = m_SelectingBox.corner_win[0];
    double origin_y_ = m_SelectingBox.corner_win[1];
    double final_x_ = m_SelectingBox.corner_win[2];
    double final_y_ = m_SelectingBox.corner_win[3];

    double p[3];
    double p_x,p_y,p_z;
//    glhProjectf(pt.x(),pt.y(),pt.z(),m_GLmodelview,m_GLprojection,m_GLviewport,p);

//    p_x = p[0];
//    p_y = p[1];
//    p_z = p[2];
//    qDebug()<<"openglf:"<<p_x<<p_y<<p_z;

//    gluProject(pt.x(),pt.y(),pt.z(),m_GLmodelview,m_GLprojection,m_GLviewport,&p_x,&p_y,&p_z);
//    qDebug()<<"opengl:"<<p_x<<p_y<<p_z;


    QVector3D pos_screen;
    QtProject(QVector3D(pt.x(),pt.y(),pt.z()),modelview, projection, m_GLviewport, pos_screen);
//    qDebug()<<"q:"<<pos_screen;

    if ((pos_screen.x() < std::min(origin_x_, final_x_)) || (pos_screen.x() > std::max(origin_x_, final_x_)))
        return (false);
    // Check the bottom and top
    if ((pos_screen.y() < std::min(origin_y_, final_y_)) || (pos_screen.y() > std::max(origin_y_, final_y_)))
        return (false);
    return (true);
}


void MainWidget::SelectNonGL(QMap<int, int> &Indices, bool add, bool remove)
{
    if(!add)
        Indices.clear();

    QVector <Vertex* > vvect;
    if(isUnfoldedMeshReady)
    {
        vvect = extendedMesh->GetUnFoldedMesh()->GetVertices();
        for(int i=0;i<vvect.size();i++)
        {
            if(IsInSelectBox((*vvect[i])))
            {
                if(remove)
                {
                    QMap<int,int>::iterator it;
                    if((it=Indices.find(i)) != Indices.end())
                        Indices.erase(it);
                }
                else
                {
                    Indices[i] = i;
                }
            }
        }
    }
    if(isFoldedMeshReady)
    {
        vvect = extendedMesh->GetFoldedMesh()->GetVertices();
        for(int i=0;i<vvect.size();i++)
        {
            if(IsInSelectBox((*vvect[i])))
            {
                if(remove)
                {
                    QMap<int,int>::iterator it;
                    if((it=Indices.find(i)) != Indices.end())
                        Indices.erase(it);
                }
                else
                {
                    Indices[i] = i;
                }
            }
        }
    }
}

void MainWidget::SelectFace(QMouseEvent* e, QMap<int, int>& Indices,  bool add, bool remove)
{
    if(!add)
        Indices.clear();
    int PickedFace;
    PickedFace = GLPickAFace(e->pos().x(), e->pos().y());
    if(PickedFace!=-1)
    {
        if(isFoldedMeshReady&&PickedFace>extendedMesh->GetFoldedMesh()->GetFaces().size())
            PickedFace -= extendedMesh->GetFoldedMesh()->GetFaces().size();
        if(remove)
        {
            QMap<int,int>::iterator it;
            if((it=Indices.find(PickedFace)) != Indices.end())
                Indices.erase(it);
        }
        else
        {
            Indices[PickedFace] = PickedFace;
        }
    }
}

int MainWidget::GLPickAFace(GLdouble winX,  GLdouble winY)
{
    glGetIntegerv(GL_VIEWPORT, m_GLviewport);           // Retrieves The Viewport Values (X, Y, Width, Height)

    //GL_SELECT mode
    GLuint *selectBuf = new GLuint[64];
    glSelectBuffer (16, selectBuf);
    (void) glRenderMode (GL_SELECT);

    //initial name buffer
    glInitNames();
    glPushName(-1);

    // Calculate projection transformation
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix ((GLdouble) winX, (GLdouble)(m_GLviewport[3]-winY), (GLdouble) 1.0, (GLdouble) 1.0, m_GLviewport);
    glMultMatrixf(projection.constData());

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(translateX, translateY, translateZ);
    matrix.rotate(rotation);
    modelview = matrix;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMultMatrixf(modelview.constData());

    QVector<Face *> Faces;
    if(isFoldedMeshReady)
    {
        Faces = extendedMesh->GetFoldedMesh()->GetFaces();
        for (int i=0; i<Faces.size(); ++i)  {
            glLoadName(i);
            QVector <Vertex* > vertices = Faces[i]->GetVertices();
            glBegin(GL_POLYGON);
            for (int j=0; j<vertices.size(); ++j)
                glVertex3f(vertices[j]->x(), vertices[j]->y(), vertices[j]->z());
            glEnd();
        }
    }

    if(isUnfoldedMeshReady)
    {
        Faces = extendedMesh->GetUnFoldedMesh()->GetFaces();
        for (int i=0; i<Faces.size(); ++i)  {
            glLoadName(i);
            QVector <Vertex* > vertices = Faces[i]->GetVertices();
            glBegin(GL_POLYGON);
            for (int j=0; j<vertices.size(); ++j)
                glVertex3f(vertices[j]->x(), vertices[j]->y(), vertices[j]->z());
            glEnd();
        }
    }


    glFlush ();
    int num = glRenderMode(GL_RENDER);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    int activeFace;
    if(num<1)
        activeFace = -1;
    else
    {
        activeFace = selectBuf[3];
        int minDepth = selectBuf[1];
        for(int i=1;i<num;i++)
        {
            if(minDepth>selectBuf[i*4+1])
            {
                minDepth = selectBuf[i*4+1];
                activeFace = selectBuf[i*4+3];
            }
        }
    }
    delete selectBuf;
    return activeFace;
}

void MainWidget::UTILS_color_hue_to_rgb (float hue, float *rgb)
{
    static float	RED = 0.0;
    static float	GRN = 1.0 / 3.0;
    static float	BLU = 2.0 / 3.0;

    rgb[0] = (hue < 0.5) ? hue - RED : 1.0 - hue;
    rgb[0] = 1.0 - (3.0 * rgb[0]);
    if (rgb[0] < 0.0) rgb[0] = 0.0;

    rgb[1] = (hue > GRN) ? hue - GRN : GRN - hue;
    rgb[1] = 1.0 - (3.0 * rgb[1]);
    if (rgb[1] < 0.0) rgb[1] = 0.0;

    rgb[2] = (hue > BLU) ? hue - BLU : BLU - hue;
    rgb[2] = 1.0 - (3.0 * rgb[2]);
    if (rgb[2] < 0.0) rgb[2] = 0.0;
    return;
}

void MainWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Q:
        TranslateFolded(0,0,0.1);
        break;
    case Qt::Key_E:
        TranslateFolded(0,0,-0.1);
        break;
    case Qt::Key_A:
        TranslateFolded(0,0.1,0.0);
        break;
    case Qt::Key_D:
        TranslateFolded(0,-0.1,0.0);
        break;
    case Qt::Key_W:
        TranslateFolded(0.1,0,0.0);
        break;
    case Qt::Key_S:
        TranslateFolded(-0.1,0,0.0);
        break;
    case Qt::Key_Tab:
        m_CurrentControl =!m_CurrentControl;
        break;
    }
    updateGL();
}

void MainWidget::TranslateFolded(float x, float y, float z)
{

    if(isFoldedMeshReady)
    {
        QVector<Vertex *> vertices = extendedMesh->GetUnFoldedMesh()->GetVertices();
        for(int i=0;i<vertices.size();i++)
        {
            Vertex *v = vertices[i];
            v->setX(v->x()+x);
            v->setY(v->y()+y);
            v->setZ(v->z()+z);
        }
    }
}


void MainWidget::TransformFolded(const QMatrix4x4& transformation)
{
    if(isFoldedMeshReady)
    {
        //find rough center
        QVector<Vertex *> vertices = extendedMesh->GetFoldedMesh()->GetVertices();

        int step = 1;
        if(vertices.size()>50)
            step = 1+vertices.size()/50;
        QVector3D center(0.0,0.0,0.0);
        for(int i=0;i<vertices.size();i+=step)
        {
            center += *vertices[i];
        }
        center /= vertices.size()/step;
        QMatrix4x4 zero_org;
        zero_org.translate(-center);
        QMatrix4x4 de_zero;
        de_zero.translate(center);
        QMatrix4x4 final_mat = de_zero*transformation*zero_org;
        for(int i=0;i<vertices.size();i++)
        {
            Vertex *v = vertices[i];
            QVector3D tv = (final_mat*QVector4D(*v,1.0)).toVector3DAffine();
            v->setX(tv.x());
            v->setY(tv.y());
            v->setZ(tv.z());
        }
    }
}



//bool ExtendedMesh::FindFaceTransformation(Face* f1, Face* f2, QMatrix4x4& transf)
//{

////    QVector<Vertex *> v1 = f1->GetVertices();
////    QVector<Vertex *> v2 = f2->GetVertices();
//////    if(v1.size()!=v2.size())
//////        return false;
//////    int num = v1.size();

//////    Vertex centroid1(0.0,0.0,0.0),centroid2(0.0,0.0,0.0);
//////    for(int i=0;i<num;i++)
//////    {
//////        centroid1 += (*v1[i]);
//////        centroid2 += (*v2[i]);
//////    }

//////    for(int i=0;i<v1.size();i++)
//////    {
//////        FV1.
//////    }

////    QMatrix4x3 m1,m2;
////    m1.fill();
////    return true;
//}

