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

 #include "geometryengine.h"

 GeometryEngine::GeometryEngine()
 {
 }

 GeometryEngine::~GeometryEngine()
 {
 }

 void GeometryEngine::init()
 {
     initializeGLFunctions();
     glEnable(GL_COLOR_MATERIAL);	// color material
     glLineWidth(2.0);
     glPointSize(8.0);

 }

 void UTILS_color_hue_to_rgb (float hue, float *rgb)
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

void GeometryEngine::drawBaseMesh(BaseMesh* mesh, QGLWidget* glWidget, unsigned int flags)
{
    if (mesh == NULL) return;
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);


    QVector<Edge *> edges = mesh->GetEdges();
    for(int i=0;i<edges.size();i++)
    {
        float UT_color[3];
        if(edges[i]->GetFlag()==1)
            glColor3f(1.0,0.0,0.0);
        else
            if(edges[i]->GetFlag()==-1)
                glColor3f(0.0,1.0,0.0);
            else
                glColor3f(0.0,0.0,1.0);

        Vertex* start_v = edges[i]->GetStart();
        Vertex* end_v = edges[i]->GetEnd();
        glBegin(GL_LINES);
        glVertex3f(start_v->x(),start_v->y(),start_v->z());
        glVertex3f(end_v->x(),end_v->y(),end_v->z());
        glEnd();

//        if(glWidget)
//        {
//            glDisable(GL_DEPTH_TEST);
//            glColor3f(0.0, 0.0, 0.0);
//            QFont m_font;
//            m_font.setPointSize(15);
//            QString cur = QString("%1").arg(edges[i]->Weight(),0,'f',3);
//            glWidget->renderText((start_v->x()+end_v->x())/2, (start_v->y()+end_v->y())/2, (start_v->z()+end_v->z())/2,cur,m_font);
//        }
    }


    float UT_color[3];
    QVector <Vertex*> verts = mesh->GetVertices();
    glBegin(GL_POINTS);
    for (int i=0; i<verts.size(); ++i){
        UTILS_color_hue_to_rgb(verts[i]->Value()/720.0,UT_color);
        glColor3fv(UT_color);
        glVertex3f(verts[i]->x(), verts[i]->y(), verts[i]->z());
    }
    glEnd();


    if (flags&SHADED)
    {
        QVector <Face*> faces = mesh->GetFaces();
        for (int i=0; i<faces.size(); ++i)  {
            QVector <Vertex* > vertices = faces[i]->GetVertices();
            float UT_color[3];
            glBegin(GL_POLYGON);

            for (int j=0; j<vertices.size(); ++j)
            {
                glColor4f(vertices[j]->Value(), vertices[j]->Value(), vertices[j]->Value(), 1);
                UTILS_color_hue_to_rgb(vertices[j]->Value()/720.0,UT_color);
                glColor3fv(UT_color);
                glVertex3f(vertices[j]->x(), vertices[j]->y(), vertices[j]->z());
            }
            glEnd();
        }
    }

    if (flags&VERTEX_DATA)
    {

        glDisable(GL_DEPTH_TEST);
        glColor3f(0.0, 0.0, 0.0);
        QFont m_font;
        m_font.setPointSize(15);

        for (int i=0; i<verts.size(); ++i)  {
            QString cur = QString("%1").arg(verts[i]->Value(),0,'f',3);
            glWidget->renderText(verts[i]->x(), verts[i]->y(), verts[i]->z(),cur,m_font);
        }
    }



/*
    QVector <Face*> faces = mesh->GetFaces();
    for (int i=0; i<faces.size(); ++i)  {
        QVector <Vertex* > vertices = faces[i]->GetVertices();
        float UT_color[3];
//        glBegin(GL_LINE_LOOP);

//        for (int j=0; j<vertices.size(); ++j)
//        {
////            glColor4f(vertices[j]->Value(), vertices[j]->Value(), vertices[j]->Value(), 1);
//            UTILS_color_hue_to_rgb(vertices[j]->Value()/720.0,UT_color);
//            glColor3fv(UT_color);
//            glVertex3f(vertices[j]->x(), vertices[j]->y(), vertices[j]->z());
//        }
//        glEnd();
        glBegin(GL_POINTS);
        for (int j=0; j<vertices.size(); ++j)
        {
//            glColor4f(vertices[j]->Value(), vertices[j]->Value(), vertices[j]->Value(), 1);
            UTILS_color_hue_to_rgb(vertices[j]->Value()/720.0,UT_color);
            glColor3fv(UT_color);
            glVertex3f(vertices[j]->x(), vertices[j]->y(), vertices[j]->z());
        }
        glEnd();


//        glColor4f(1.0, 1.0, 1.0, 0.5);
//        for (int j=0; j<faces.size(); ++j)
//        {
//            glBegin(GL_POLYGON);
//            QVector<Vertex*> f_vertices = faces[i]->GetVertices();
//            for(int k=0;k<f_vertices.size();k++)
//            {
//                UTILS_color_hue_to_rgb(f_vertices[k]->Value()/720.0,UT_color);
//                glColor3fv(UT_color);
//                glVertex3f(f_vertices[k]->x(), f_vertices[k]->y(), f_vertices[k]->z());
//            }
//            glEnd();
//        }


        if(glWidget && (flags&VERTEX_DATA))
        {
            glDisable(GL_DEPTH_TEST);
            glColor3f(0.0, 0.0, 0.0);
            QFont m_font;
            m_font.setPointSize(15);
            for (int j=0; j<vertices.size(); ++j)
            {
                QString cur = QString("%1").arg(vertices[j]->Value(),0,'f',3);
                glWidget->renderText(vertices[j]->x(), vertices[j]->y(), vertices[j]->z(),cur,m_font);
            }
        }

    }*/
}


