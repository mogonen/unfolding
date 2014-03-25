#-------------------------------------------------
 #
 # Project created by QtCreator 2010-06-23T12:55:35
 #
 #-------------------------------------------------

 QT       += core gui
CONFIG += console

 TARGET = cube
 TEMPLATE = app
 SOURCES += main.cpp \
    window.cpp \
    mainwindow.cpp \
    extendedmesh.cpp \
    extendededge.cpp \
    extendedface.cpp \
    output.cpp \
    customdialog.cpp \
    vertex.cpp

     QT += opengl

     SOURCES += mainwidget.cpp \
        geometryengine.cpp

     HEADERS += \
         mainwidget.h \
         geometryengine.h \
    mainwindow.h \
    extendedface.h \
    extendedmesh.h \
    extendededge.h \
    output.h \
    customdialog.h \
    edge.h \
    face.h \
    vertex.h \
    basemesh.h

     RESOURCES += \
         shaders.qrc \
         textures.qrc \
        application.qrc

     OTHER_FILES += \
         vshader.glsl \
         fshader.glsl


HEADERS += \
    window.h
