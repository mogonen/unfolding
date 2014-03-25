#include <QtWidgets>
#include "window.h"
#include <iostream>


void Window::createAllGuiObjects()
{
    loadOBJ = new QPushButton("Load OBJ", this);
    triangulate = new QPushButton("Triangulate", this);
    onePiece = new QPushButton("Make One Piece", this);

    showOriginal = new QCheckBox("Show original", this);
    showTriangle = new QCheckBox("Show trimesh", this);
    showFolded = new QCheckBox("Show folded", this);
    showUnfolded = new QCheckBox("Show unfolded", this);
    showOriginal->setCheckState(Qt::Checked);
    showTriangle->setCheckState(Qt::Checked);
    showFolded->setCheckState(Qt::Checked);
    showUnfolded->setCheckState(Qt::Checked);

    foldedOBJ = new QPushButton("Output Folded OBJ", this);
    unfoldedOBJ = new QPushButton("Output Unfolded OBJ", this);
    foldedINF = new QPushButton("Output Folded INF", this);
    unfoldedINF = new QPushButton("Output Unfolded INF", this);
    scaleSetting = new QSlider(Qt::Vertical, this);
    output = new Output();

    scaleSetting->setRange(20, 70);
    scaleSetting->setValue(20);
}

void Window::createActions()
{

//    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(LoadOBJFile()));



       saveFoldedObj = new QAction(tr("Save"), this);
       saveFoldedObj->setStatusTip(tr("Save Folded"));
       connect(saveFoldedObj, SIGNAL(triggered()), qApp, SLOT(OutputFoldedOBJ()));

}

QGridLayout* Window::createLayout(QGridLayout *layout)
{
    layout->addWidget(mainwidget, 0, 0, 12, 7);

    layout->addWidget(loadOBJ, 0, 8);
    layout->addWidget(triangulate, 1, 8);
    layout->addWidget(onePiece, 2, 8);
    layout->addWidget(scaleSetting, 3, 8);

    layout->addWidget(showOriginal, 4, 8);
    layout->addWidget(showTriangle, 5, 8);
    layout->addWidget(showFolded, 6, 8);
    layout->addWidget(showUnfolded, 7, 8);

    layout->addWidget(foldedOBJ, 9, 8);
    layout->addWidget(unfoldedOBJ, 10, 8);
    layout->addWidget(foldedINF, 11, 8);
    layout->addWidget(unfoldedINF, 12, 8);
    return layout;
}
void Window::createMenus()
{
////    fileMenu = menuBar()->addMenu(tr("&File"));
//////    fileMenu->addAction(newAct);
////    fileMenu->addAction(openAct);
////    fileMenu->addAction(saveFoldedObj);
//////    fileMenu->addAction(saveAsAct);
////    fileMenu->addSeparator();
////    fileMenu->addAction(exitAct);

////    editMenu = menuBar()->addMenu(tr("&Edit"));
////    editMenu->addAction(cutAct);
////    editMenu->addAction(copyAct);
//////    editMenu->addAction(pasteAct);

//    menuBar()->addSeparator();

////    helpMenu = menuBar()->addMenu(tr("&Help"));
//////    helpMenu->addAction(aboutAct);
////    helpMenu->addAction(aboutQtAct);
}

void Window::createToolBars()
{
//    fileToolBar = addToolBar(tr("File"));
////    fileToolBar->addAction(newAct);
//    fileToolBar->addAction(openAct);
//    fileToolBar->addAction(saveFoldedObj);

//    editToolBar = addToolBar(tr("Edit"));
//    editToolBar->addAction(cutAct);
//    editToolBar->addAction(copyAct);
////    editToolBar->addAction(pasteAct);
}

void Window::createStatusBar()
{
//    statusBar()->showMessage(tr("Ready"));
}

Window::Window() {
    setWindowTitle(tr("Object Unfolding"));

    mainwidget = new MainWidget();

//    createActions();
//    createMenus();
//    createToolBars();
//    createStatusBar();

    createAllGuiObjects();

//    setCentralWidget(mainwidget);
    // Window layout
    layout = new QGridLayout;

    layout = createLayout(layout);

    connect(loadOBJ, SIGNAL(clicked()), this, SLOT(LoadOBJFile()));
    connect(triangulate, SIGNAL(clicked()), this, SLOT(TriangulateMesh()));
    connect(onePiece, SIGNAL(clicked()), this, SLOT(ForExtendedMesh()));
    connect(scaleSetting, SIGNAL(valueChanged(int)), this, SLOT(SetScale(int)));

    connect(showOriginal, SIGNAL(clicked()), this, SLOT(ShowOriginal()));
    connect(showTriangle, SIGNAL(clicked()), this, SLOT(ShowTriangle()));
    connect(showFolded, SIGNAL(clicked()), this, SLOT(ShowFolded()));
    connect(showUnfolded, SIGNAL(clicked()), this, SLOT(ShowUnfolded()));

    connect(foldedOBJ, SIGNAL(clicked()), this, SLOT(OutputFoldedOBJ()));
    connect(unfoldedOBJ, SIGNAL(clicked()), this, SLOT(OutputUnfoldedOBJ()));
    connect(foldedINF, SIGNAL(clicked()), this, SLOT(OutputFoldedINF()));
    connect(unfoldedINF, SIGNAL(clicked()), this, SLOT(OutputUnFoldedINF()));

    setLayout(layout);
    baseMesh = NULL;
    triangleMesh = NULL;
    extendedMesh = NULL;
    scaleRatio = 1.0;
}



void Window::Clear() {
    // delete the existing base meshes
    if (baseMesh != NULL) {
        try {
            delete baseMesh;
        } catch(...) {
            QMessageBox::information(this, tr("Error"), "The base mesh does not exist!");
        }

        baseMesh = NULL;
    }

    if (triangleMesh != NULL) {
        try {
            delete triangleMesh;
        } catch(...) {
            QMessageBox::information(this, tr("Error"), "The triangle mesh does not exist!");
        }

        triangleMesh = NULL;
    }

    if (extendedMesh != NULL) {
        try {
            delete extendedMesh;
        } catch(...) {
            QMessageBox::information(this, tr("Error"), "The extended mesh does not exist!");
        }

        extendedMesh = NULL;
    }

    // reset the mainwidget
    mainwidget->Reset();
}

BaseMesh* Window::CreateBaseMesh() {
    BaseMesh* mesh = NULL;
    try {
        mesh = new BaseMesh();
    } catch(...) {
        QMessageBox::information(this, tr("Error"), "No enough memory to create a new mesh.");
        return NULL;
    }
    return mesh;
}

void Window::LoadOBJFile(){
    fileName = QFileDialog::getOpenFileName(this, tr("Open OBJ File"), "", tr("OBJ File(*.obj);;All Files(*)"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }

    // clear existing mesh data and reset the program
    Clear();
    // create a new base mesh
    baseMesh = CreateBaseMesh();

    // If the file is loaded sucessfully
    QTextStream in(&file);
    while (!in.atEnd()) {
        // Read file line by line
        QString line = in.readLine();
        // Separate the line with whitespaces
        QStringList item = line.split(" ");
        if (item.size()>0) {
            // Add a new vertex
            if (item[0] == "v" && item.size() == 4)
                baseMesh->AddVertex(item[1].toFloat(), item[2].toFloat(), item[3].toFloat());
            // Add a new face
            else if (item[0] == "f" && item.size()>3) {
                QVector <Vertex* > allVertices = baseMesh->GetVertices();
                // incident vertices in each face
                QVector <Vertex* > incidentVertices;
                for (int i=1; i<item.size(); ++i) {
                    // Consider two different formats of faces
                    QStringList subItem = item[i].split("/");
                    int index = subItem[0].toInt()-1;
                    if (index<0 || index>allVertices.size()) {
                        QMessageBox::critical(this, "critical", "Errors in the OBJ file!");
                        return;
                    }
                    // Get all incident vertices in order
                    incidentVertices.push_back(allVertices[index]);
                }
                // Create a new face
                baseMesh->AddFace(incidentVertices);
            }
        }
    }
    std::cout << "The number of Edges in the OBJ file is: " << baseMesh->GetEdges().size() << std::endl;
    std::cout << "The number of Faces in the OBJ file is: " << baseMesh->GetFaces().size() << std::endl;
    std::cout << "The number of Vertices in the OBJ file is: " << baseMesh->GetVertices().size() << std::endl;

    mainwidget->SetBaseMesh(baseMesh);
    mainwidget->updateGL();
    return;
}

void Window::TriangulateMesh() {
    if (baseMesh == NULL || triangleMesh != NULL) return;

    if (!triangleMesh) {
        try {
            delete triangleMesh;
        } catch(...) {
            QMessageBox::information(this, tr("Error"), "Invalid Pointer for triangleMesh");
        }
        triangleMesh = NULL;
    }

    triangleMesh = CreateTriangleMesh(baseMesh);
    if (triangleMesh == NULL) {
        QMessageBox::information(this, tr("Error"), "Triangulate the base mesh is incompleted!");
        return;
    }
    mainwidget->SetTriangleMesh(triangleMesh);
    mainwidget->updateGL();
    return;
}

BaseMesh* Window::CreateTriangleMesh(BaseMesh* mesh) {
    if (mesh == NULL) return NULL;
    BaseMesh* triMesh = NULL;
    try {
        triMesh = new BaseMesh();
    } catch (...) {
        QMessageBox::information(this, tr("Error"), "Not enough memory!");
    }

    QVector <Vertex* > vertices = mesh->GetVertices();
    QMap <Vertex*, Vertex*> maps;
    // Add vertices
    for (int i=0; i<vertices.size(); ++i) {
        Vertex* newVertex = triMesh->AddVertex(vertices[i]);
        maps[vertices[i]] = newVertex;
    }

    // Add triangles

    QVector <Face* > faces = mesh->GetFaces();
    for (int i=0; i<faces.size(); ++i) {
        QVector <Vertex* > currentVertices = faces[i]->GetVertices();
        int faceSize = currentVertices.size();
        if (faceSize < 3) {
            QMessageBox::information(this, tr("Error"), "Face with less than 3 vertices!");
            return NULL;
        }

        for (int j=2; j<faceSize; ++j) {
            QVector <Vertex* > triangle;
            triangle.push_back(maps[currentVertices[0]]);
            triangle.push_back(maps[currentVertices[j-1]]);
            triangle.push_back(maps[currentVertices[j]]);
            triMesh->AddFace(triangle);
        }
    }
    triMesh->ComputeEdgeTree();
    return triMesh;
}

void Window::ForExtendedMesh() {
    if (triangleMesh == NULL ||extendedMesh != NULL) return;
    if (!extendedMesh) {
        try {
            delete extendedMesh;
        } catch(...) {
            QMessageBox::information(this, tr("Error"), "Invalid Pointer for onePieceMesh");
        }
        extendedMesh = NULL;
    }
    extendedMesh = CreateExtendedMesh(triangleMesh);

    if (extendedMesh == NULL) {
        QMessageBox::information(this, tr("Error"), "One piece mesh is incompleted!");
        return;
    }
    mainwidget->SetExtendedMesh(extendedMesh);
    std::cout << "Building Folded Mesh..." << std::endl;
    extendedMesh->BuildFoldedMesh();
    std::cout << "Draw the folded mesh..." << std::endl;

    std::cout << "Building UnFolded Mesh..." << std::endl;
    extendedMesh->BuildUnFoldedMesh();
    std::cout << "Draw the Unfolded mesh..." << std::endl;
    mainwidget->updateGL();
    return;
}

ExtendedMesh* Window::CreateExtendedMesh(BaseMesh* triMesh) {
    if (triMesh == NULL) return NULL;
    ExtendedMesh* fMesh = NULL;
//    try {
//        fMesh = new ExtendedMesh();
//    } catch (...) {
//        QMessageBox::information(this, tr("Error"), "Not enough memory!");
//    }

//    QVector <Vertex* > vertices = triMesh->GetVertices();
//    QMap <Vertex*, Vertex*> maps;
//    // Add vertices
//    for (int i=0; i<vertices.size(); ++i) {
//        Vertex* newVertex = fMesh->AddVertex(vertices[i]);
//        maps[vertices[i]] = newVertex;
//    }

//    // Add faces
//    QVector <Face* > faces = triMesh->GetFaces();
//    for (int i=0; i<faces.size(); ++i) {
//        QVector <Vertex* > currentVertices = faces[i]->GetVertices();
//        int faceSize = currentVertices.size();
//        if (faceSize != 3) {
//            QMessageBox::information(this, tr("Error"), "Face are not triangled!");
//            return NULL;
//        }

//        for (int j=2; j<faceSize; ++j) {
//            QVector <Vertex* > triangle;
//            triangle.push_back(maps[currentVertices[0]]);
//            triangle.push_back(maps[currentVertices[j-1]]);
//            triangle.push_back(maps[currentVertices[j]]);
//            fMesh->AddFace(triangle);
//        }
//    }
//    fMesh->ComputeEdgeTree();
    return fMesh;
}

void Window::SetScale(int k ) {
    scaleRatio = (1-k/100.0);
    if (extendedMesh == NULL) return;
    extendedMesh->RebuildMesh(scaleRatio, true);
    mainwidget->updateGL();
    return;
}

void Window::OutputFoldedOBJ() {
    if (extendedMesh == NULL) return;
    BaseMesh* baseFoldedMesh = extendedMesh->GetFoldedMesh();
    QString prefixName;
    for (int i=0; i<fileName.size(); ++i) {
        if (fileName[i] != '.') prefixName += fileName[i];
        else break;
    }
    output->OutputOBJFile(prefixName+"_folded", baseFoldedMesh);
    return;
}

void Window::OutputUnfoldedOBJ() {
    if (extendedMesh == NULL) return;
    BaseMesh* baseUnfoldedMesh = extendedMesh->GetUnFoldedMesh();
    QString prefixName;
    for (int i=0; i<fileName.size(); ++i) {
        if (fileName[i] != '.') prefixName += fileName[i];
        else break;
    }
    output->OutputOBJFile(prefixName+"_unfolded", baseUnfoldedMesh);
    return;
}

void Window::OutputFoldedINF() {
    if (extendedMesh == NULL) return;
    BaseMesh* baseFoldedMesh = extendedMesh->GetFoldedMesh();
    QString prefixName;
    for (int i=0; i<fileName.size(); ++i) {
        if (fileName[i] != '.') prefixName += fileName[i];
        else break;
    }
    output->OutputINFFile(prefixName+"_folded", baseFoldedMesh);
    return;
}

void Window::OutputUnFoldedINF() {
    if (extendedMesh == NULL) return;
    BaseMesh* baseUnfoldedMesh = extendedMesh->GetUnFoldedMesh();
    QString prefixName;
    for (int i=0; i<fileName.size(); ++i) {
        if (fileName[i] != '.') prefixName += fileName[i];
        else break;
    }
    output->OutputINFFile(prefixName+"_unfolded", baseUnfoldedMesh);
    return;
}

void Window::ShowOriginal() {
    bool status = showOriginal->checkState();
    mainwidget->ShowOrignal(status);
    mainwidget->updateGL();
}

void Window::ShowFolded() {
    bool status = showFolded->checkState();
    mainwidget->ShowFolded(status);
    mainwidget->updateGL();
}

void Window::ShowUnfolded() {
    bool status = showUnfolded->checkState();
    mainwidget->ShowUnfolded(status);
    mainwidget->updateGL();
}

void Window::ShowTriangle() {
    bool status = showTriangle->checkState();
    mainwidget->ShowTriangle(status);
    mainwidget->updateGL();
}












