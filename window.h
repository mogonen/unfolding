#ifndef WINDOW_H
#define WINDOW_H

#include "extendedmesh.h"
#include "mainwidget.h"
#include "basemesh.h"
#include "output.h"

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QWidget>
#include <QMap>
#include <QApplication>
#include <QFileDialog>
#include <QString>
#include <QtGui>
#include <QCheckBox>
#include <QSlider>
#include <QToolBar>
#include <QMenuBar>
#include <QAction>
#include <QMenu>


class Window : public QWidget {
    Q_OBJECT

public:
    Window();
    ~Window() { Clear(); }

    void createAllGuiObjects();
    QGridLayout* createLayout(QGridLayout *layout);
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

private slots:
    void LoadOBJFile();
    void TriangulateMesh();
    void ForExtendedMesh();
    void SetScale(int k);
    void OutputFoldedOBJ();
    void OutputUnfoldedOBJ();
    void OutputFoldedINF();
    void OutputUnFoldedINF();
    void ShowOriginal();
    void ShowTriangle();
    void ShowFolded();
    void ShowUnfolded();


private:
    BaseMesh* CreateBaseMesh();
    BaseMesh* CreateTriangleMesh(BaseMesh* mesh);
    ExtendedMesh* CreateExtendedMesh(BaseMesh* triMesh);
    void Clear();

private:

    QMenu *fileMenu;    // Load OBJ(Open), S  , unfolded OBJ (Save) , Save as INF(Export)
    QMenu *editMenu;    // Triangulate Image, Make one Piece(Unfold onject)
    QMenu *helpMenu;
    QMenu *viewMenu;    // holds show original, trimesh, folded mesh, unfolded

    QToolBar *fileToolBar;  // Load OBJ(Open), Save As folded OBJ , unfolded OBJ (Save) , Save as INF(Export)
    QToolBar *editToolBar;  // Triangulate Image, Make one Piece(Unfold onject)
    QToolBar *viewToolBar;  // holds show original, trimesh, folded mesh, unfolded

    QAction *openAct;
    QAction *saveFoldedObj;
    QAction *saveUnfoldedObj;
    QAction *exportFoldedINF;
    QAction *exportUnfoldedINF;
    QAction *exitAct;

    QAction *pasteAct;
    QAction *aboutAct;
    QAction *aboutQtAct;


    QPushButton* loadOBJ;
    QPushButton* triangulate;
    QPushButton* onePiece;

    QPushButton* foldedOBJ;
    QPushButton* unfoldedOBJ;
    QPushButton* foldedINF;
    QPushButton* unfoldedINF;
    QCheckBox* showOriginal;
    QCheckBox* showTriangle;
    QCheckBox* showFolded;
    QCheckBox* showUnfolded;
    QSlider* scaleSetting;
    QGridLayout* layout;

    MainWidget* mainwidget;  // OWNED
    BaseMesh* baseMesh;  // OWNED
    BaseMesh* triangleMesh; // OWNED
    ExtendedMesh* extendedMesh; // OWNED
    Output* output; // OWNED
    QString fileName;

    qreal scaleRatio;
};

#endif // WINDOW_H
