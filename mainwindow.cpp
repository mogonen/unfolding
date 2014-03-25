#include <QtWidgets>

#include "mainwindow.h"
#include "customdialog.h"

MainWindow::MainWindow()
{
    setWindowTitle(tr("Object Unfolding"));
    //    textEdit = new QPlainTextEdit;
    mainwidget = new MainWidget();
    setCentralWidget(mainwidget);
//    createAllGuiObjects();

    baseMesh = NULL;
    triangleMesh = NULL;
    extendedMesh = NULL;
    createActions();

    createFileMenu();
    createEditMenu();
    createViewMenu();
    createMeshOperationsMenu();
    createSelectEdgesMenu();
    createSelectFacesMenu();
    createSelectVerticesMenu();
    createFlapTypesMenu();
    createUnfoldMenu();
    createHelpMenu();


//    createToolBars();
    createStatusBar();

    // creating a dock
    toolsWidgetDock = new QDockWidget(QString("Options"), this);
    toolsWidgetDock->setVisible(false);
    this->addDockWidget(Qt::LeftDockWidgetArea, toolsWidgetDock);

    readSettings();

    // Alll the connections

    //    connect(textEdit->document(), SIGNAL(contentsChanged()),this, SLOT(documentWasModified()));

    //    setCurrentFile("");
//    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::Clear() {
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

BaseMesh* MainWindow::CreateBaseMesh() {
    BaseMesh* mesh = NULL;
    try {
        mesh = new BaseMesh();
    } catch(...) {
        QMessageBox::information(this, tr("Error"), "No enough memory to create a new mesh.");
        return NULL;
    }
    return mesh;
}

void MainWindow::LoadOBJFile(){
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

    baseMesh->ComputeEdgeTree();

    std::cout << "The number of Edges in the OBJ file is: " << baseMesh->GetEdges().size() << std::endl;
    std::cout << "The number of Faces in the OBJ file is: " << baseMesh->GetFaces().size() << std::endl;
    std::cout << "The number of Vertices in the OBJ file is: " << baseMesh->GetVertices().size() << std::endl;

    mainwidget->Reset();
    mainwidget->SetBaseMesh(baseMesh);
    mainwidget->updateGL();
    return;
}

void MainWindow::TriangulateMesh() {
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

void MainWindow::UseSelectionAsConnectors ()
{
    //// TODO: for using Selection As Connectors
}

void MainWindow::SelectAllEdges(){
    //// TODO: for using Selecting All Edges
}
void MainWindow::ClearSelectionEdges(){
    //// TODO: for using ClearSelectionEdges
}
void MainWindow::SelectInverseEdges(){
    //// TODO: for using SelectInverseEdges
}
void MainWindow::SelectMultipleEdges(){
    //// TODO: for using SelectMultipleEdges
}
void MainWindow::SelectSimilarEdges(){
    //// TODO: for using SelectSimilarEdges
}
void MainWindow::SelectWindowEdges(){
    //// TODO: for using SelectWindowEdges
}
void MainWindow::SelectConnectedComponentEdges(){
    //// TODO: for using SelectConnectedComponentEdges
}
void MainWindow::SelectShortestPathBetween2Vertices(){
    //// TODO: for using SelectShortestPathBetween2Vertices
}
void MainWindow::GrowSelectionEdges(){
    //// TODO: for using GrowSelectionEdges
}
void MainWindow::ShrinkSelectionEdges(){
    //// TODO: for using SShrinkSelectionEdges
}
void MainWindow::SelectEdgeLoop(){
    //// TODO: for using SelectEdgeLoop
}
void MainWindow::SelectEdgeRing(){
    //// TODO: for using SelectEdgeRing
}
void MainWindow::SelectEdgesFromFaces(){
    //// TODO: for using SelectEdgesFromFaces
}
void MainWindow::SelectEdgesFromVertices(){
    //// TODO: for using SelectEdgesFromVertices
}

void MainWindow::UseSelectionAsFlaps()
{
    //// TODO: for using Selection As flaps
}


void MainWindow::SelectAllFaces(){
    //// TODO: for using Selecting All Faces
}
void MainWindow::ClearSelectionFaces(){
    //// TODO: for using ClearSelectionFaces
}
void MainWindow::SelectInverseFaces(){
    //// TODO: for using SelectInverseFaces
}
void MainWindow::SelectMultipleFaces(){
    //// TODO: for using SelectMultipleFaces
}
void MainWindow::SelectSimilarFaces(){
    //// TODO: for using SelectSimilarFaces
}
void MainWindow::SelectWindowFaces(){
    //// TODO: for using SelectWindowFaces
}
void MainWindow::SelectConnectedComponentFaces(){
    //// TODO: for using SelectConnectedComponentFaces
}

void MainWindow::SelectFaceLoop(){
    //// TODO: for using SelectEdgeLoop
}

void MainWindow::SelectFacesFromEdges(){
    //// TODO: for using SelectFacesFromEdges
}
void MainWindow::SelectFacesFromVertices(){
    //// TODO: for using SelectFacesFromVertices
}

BaseMesh* MainWindow::CreateTriangleMesh(BaseMesh* mesh) {
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

bool MainWindow::ForExtendedMesh() {
    if (triangleMesh == NULL ||extendedMesh != NULL) return false;
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
        return false;
    }
    mainwidget->SetExtendedMesh(extendedMesh);
    std::cout << "Building Folded Mesh..." << std::endl;
    extendedMesh->BuildFoldedMesh();
    std::cout << "Draw the folded mesh..." << std::endl;

    std::cout << "Building UnFolded Mesh..." << std::endl;
    extendedMesh->BuildUnFoldedMesh();
    std::cout << "Draw the Unfolded mesh..." << std::endl;
    mainwidget->updateGL();
    return true;
}

ExtendedMesh* MainWindow::CreateExtendedMesh(BaseMesh* triMesh) {
    if (triMesh == NULL) return NULL;
    ExtendedMesh* fMesh = NULL;
    try {
        fMesh = new ExtendedMesh();
    } catch (...) {
        QMessageBox::information(this, tr("Error"), "Not enough memory!");
    }

    QVector <Vertex* > vertices = triMesh->GetVertices();

    QMap <Vertex*, Vertex*> maps;
    // Add vertices
    for (int i=0; i<vertices.size(); ++i) {
        Vertex* newVertex = fMesh->AddVertex(vertices[i]);
        maps[vertices[i]] = newVertex;
    }

    // Add faces
    QVector <Face* > faces = triMesh->GetFaces();
    for (int i=0; i<faces.size(); ++i) {
        QVector <Vertex* > currentVertices = faces[i]->GetVertices();
        int faceSize = currentVertices.size();
        if (faceSize != 3) {
            QMessageBox::information(this, tr("Error"), "Face are not triangled!");
            return NULL;
        }

        for (int j=2; j<faceSize; ++j) {
            QVector <Vertex* > triangle;
            triangle.push_back(maps[currentVertices[0]]);
            triangle.push_back(maps[currentVertices[j-1]]);
            triangle.push_back(maps[currentVertices[j]]);
            //find the flag of the corresponding original edge,
            QVector<int> edge_flags;
            edge_flags.push_back(currentVertices[0]->FindConnectingEdge(currentVertices[j-1])->GetFlag());
            edge_flags.push_back(currentVertices[j-1]->FindConnectingEdge(currentVertices[j])->GetFlag());
            edge_flags.push_back(currentVertices[j]->FindConnectingEdge(currentVertices[0])->GetFlag());
            fMesh->AddFace(triangle,edge_flags);
        }
    }
    return fMesh;
}

void MainWindow::SetScale(int k ) {
    scaleRatio = (1-k/100.0);
    if (extendedMesh == NULL) return;
    extendedMesh->RebuildMesh(scaleRatio, true);
    mainwidget->updateGL();
    return;
}

void MainWindow::OutputFoldedOBJ() {
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

void MainWindow::OutputUnfoldedOBJ() {
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

void MainWindow::OutputFoldedINF() {
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

void MainWindow::OutputUnFoldedINF() {
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

void MainWindow::ShowOriginal() {
    bool status = viewOriginal->isChecked();
    mainwidget->ShowOrignal(status);
    mainwidget->updateGL();
}

void MainWindow::ShowFolded() {
    bool status = viewFolded->isChecked();
    mainwidget->ShowFolded(status);
    mainwidget->updateGL();
}

void MainWindow::ShowUnfolded() {
    bool status = viewUnfolded->isChecked();
    mainwidget->ShowUnfolded(status);
    mainwidget->updateGL();
}

void MainWindow::ShowTriangle() {
    bool status = viewTriangle->isChecked();
    mainwidget->ShowTriangle(status);
    mainwidget->updateGL();
}

void MainWindow::ShowShaded(){
    bool status = viewShaded->isChecked();
    mainwidget->ShowShaded(status);
    mainwidget->updateGL();
}

void MainWindow::ShowVertexData(){
    bool status = viewVertexData->isChecked();
    mainwidget->ShowVertexData(status);
    mainwidget->updateGL();
}


void MainWindow::FlapType1(){
    //// TODO: use flap Type1 as connectors
}
void MainWindow::FlapType2(){
    //// TODO: use flap Type2 as connectors
}
void MainWindow::FlapType3(){
    //// TODO: use flap Type3 as connectors
}
void MainWindow::FlapType4(){
    //// TODO: use flap Type4 as connectors
}


void MainWindow::createAllGuiObjects()
{
//    loadOBJ = new QPushButton("Load OBJ", this);
//    triangulate = new QPushButton("Triangulate", this);
//    onePiece = new QPushButton("Make One Piece", this);

//    showOriginal = new QCheckBox("Show original", this);
//    showTriangle = new QCheckBox("Show trimesh", this);
//    showFolded = new QCheckBox("Show folded", this);
//    showUnfolded = new QCheckBox("Show unfolded", this);
//    showOriginal->setCheckState(Qt::Checked);
//    showTriangle->setCheckState(Qt::Checked);
//    showFolded->setCheckState(Qt::Checked);
//    showUnfolded->setCheckState(Qt::Checked);

//    foldedOBJ = new QPushButton("Output Folded OBJ", this);
//    unfoldedOBJ = new QPushButton("Output Unfolded OBJ", this);
//    foldedINF = new QPushButton("Output Folded INF", this);
//    unfoldedINF = new QPushButton("Output Unfolded INF", this);
//    scaleSetting = new QSlider(Qt::Vertical, this);
    output = new Output();

//    scaleSetting->setRange(20, 70);
//    scaleSetting->setValue(20);
}



void MainWindow::closeEvent(QCloseEvent *event)
{
//    if (maybeSave()) {
//        writeSettings();
        event->accept();
//    } else {
//        event->ignore();
//    }
}

void MainWindow::newFile()
{
    LoadOBJFile();
    //    if (maybeSave()) {
    //        textEdit->clear();
    //        setCurrentFile("");
    //    }
}

void MainWindow::open()
{
    LoadOBJFile();
    //    if (maybeSave()) {
    //        QString fileName = QFileDialog::getOpenFileName(this);
    //        if (!fileName.isEmpty())
    //            loadFile(fileName);
    //    }
}

void MainWindow::save()
{
    //    if (curFile.isEmpty()) {
    //        return saveAs();
    //    } else {
    //        return saveFile(curFile);
    //    }
}

bool MainWindow::saveAs()
{/*
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);*/
    return true;
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Unfolding Project"),
                       tr("The <b>Unfolding Application</b> is a project to demonstrates how "
                          "to unfold objects "));
}

void MainWindow::documentWasModified()
{
    //    setWindowModified(textEdit->document()->isModified());
}

void MainWindow::createActions()
{
    /*
     File MENU
    */

    // New
    newAct = new QAction(QIcon(":/images/document-new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    // Open
    openAct = new QAction(QIcon(":/images/folder-open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    // Save
    saveAct = new QAction(QIcon(":/images/document-save.png"), tr("Save"), this);
    saveAct->setStatusTip(tr("Save file"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(OutputFoldedOBJ()));

    // Save As sub Menu
    saveAsFoldedOBJ = new QAction(tr("Folded OBJ"), this);
    saveAsFoldedOBJ->setStatusTip(tr("Save file as Folded OBJ"));
    connect(saveAsFoldedOBJ, SIGNAL(triggered()), this, SLOT(OutputFoldedOBJ()) );

    saveAsUnfoldedOBJ = new QAction(tr("Unfolded OBJ"), this);
    saveAsUnfoldedOBJ->setStatusTip(tr("Save file as Unfolded OBJ"));
    connect(saveAsUnfoldedOBJ, SIGNAL(triggered()), this, SLOT(OutputUnfoldedOBJ()) );

    saveAsFoldedINF= new QAction(tr("Folded INF"), this);
    saveAsFoldedINF->setStatusTip(tr("Save file as Folded INF"));
    connect(saveAsFoldedINF, SIGNAL(triggered()), this, SLOT(OutputFoldedINF()) );

    saveAsUnfoldedINF = new QAction(tr("Unfolded INF"), this);
    saveAsUnfoldedINF->setStatusTip(tr("Save file as Unfolded INF"));
    connect(saveAsUnfoldedINF, SIGNAL(triggered()), this, SLOT(OutputUnFoldedINF()) );

    // Export As
    exportAct = new QAction(tr("Export"), this);
    exportAct->setStatusTip(tr("Export file as"));
    connect(exportAct, SIGNAL(triggered()), this, SLOT(exportToFile()));

    // Print
    printAct = new QAction(tr("Print"), this);
    printAct->setStatusTip(tr("Print file "));
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    // Quit
    exitAct = new QAction(tr("Exit"), this);
    exitAct->setStatusTip(tr("Quit "));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(quitApp()));

    /*
     Edit MENU
    */

    // Cut, copy and paste
    cutAct = new QAction(tr("Cut"), this);
    cutAct->setStatusTip(tr("Cut text"));
    cutAct->setEnabled(false);
    copyAct = new QAction(tr("Copy"), this);
    copyAct->setStatusTip(tr("Copy text"));
    copyAct->setEnabled(false);
    pasteAct = new QAction(tr("Paste"), this);
    pasteAct->setStatusTip(tr("Paste text"));
    pasteAct->setEnabled(false);
    connect(mainwidget, SIGNAL(copyAvailable(bool)),cutAct, SLOT(setEnabled(bool)));
    connect(mainwidget, SIGNAL(copyAvailable(bool)),copyAct, SLOT(setEnabled(bool)));
    connect(mainwidget, SIGNAL(copyAvailable(bool)),pasteAct, SLOT(setEnabled(bool)));

    /*
     * Mesh Operations Menu
     */
    triangulate = new QAction(tr("Triangulate mesh"), this);;
    triangulate->setStatusTip(tr("Triangulate mesh"));
    connect(triangulate, SIGNAL(triggered()), this, SLOT(TriangulateMesh()));

    /*
     Select Edges
    */
    selectAllEdges = new QAction(tr("Select All"), this);;
    selectAllEdges->setStatusTip(tr("Select All"));
    connect(selectAllEdges, SIGNAL(triggered()), this, SLOT(SelectAllEdges()));

    clearSelectionEdges = new QAction(tr("Clear Selected Edges"), this);;
    clearSelectionEdges->setStatusTip(tr("Clear Selected Edges"));
    connect(clearSelectionEdges, SIGNAL(triggered()), this, SLOT(ClearSelectionEdges()));

    selectInverseEdges = new QAction(tr( "Select Inverse Edges"), this);
    selectInverseEdges->setStatusTip(tr( "Select Inverse Edges"));
    connect(selectInverseEdges, SIGNAL(triggered()), this, SLOT(SelectInverseEdges()));

    selectMultipleEdges = new QAction(tr("Select Multiple Edges"), this);;
    selectMultipleEdges->setStatusTip(tr("Select Multiple Edges"));
    connect(selectMultipleEdges, SIGNAL(triggered()), this, SLOT(SelectMultipleEdges()));

    selectSimilarEdges = new QAction(tr("Select Similar Edges"), this);;
    selectSimilarEdges->setStatusTip(tr("Select Similar Edges"));
    connect(selectSimilarEdges, SIGNAL(triggered()), this, SLOT(SelectSimilarEdges()));

    selectWindowEdges = new QAction(tr("Select Window Edges"), this);;
    selectWindowEdges->setStatusTip(tr("Select Window Edges"));
    connect(selectWindowEdges, SIGNAL(triggered()), this, SLOT(SelectWindowEdges()));

    selectConnectedComponentEdges = new QAction(tr("Select Connected Component Edges"), this);;
    selectConnectedComponentEdges->setStatusTip(tr("Select Connected Component Edges"));
    connect(selectConnectedComponentEdges, SIGNAL(triggered()), this, SLOT(SelectConnectedComponentEdges()));

    selectShortestPathBetween2Vertices = new QAction(tr("Select Shortest Path Between 2 Vertices"), this);;
    selectShortestPathBetween2Vertices->setStatusTip(tr("Select Shortest Path Between 2 Vertices"));
    connect(selectShortestPathBetween2Vertices, SIGNAL(triggered()), this, SLOT(SelectShortestPathBetween2Vertices()));

    growSelectionEdges = new QAction(tr("Grow Selection Edges"), this);;
    growSelectionEdges->setStatusTip(tr("Grow Selection Edges"));
    connect(growSelectionEdges, SIGNAL(triggered()), this, SLOT(GrowSelectionEdges()));

    shrinkSelectionEdges = new QAction(tr("Shrink Selection Edges"), this);;
    shrinkSelectionEdges->setStatusTip(tr("Shrink Selection Edges"));
    connect(shrinkSelectionEdges, SIGNAL(triggered()), this, SLOT(ShrinkSelectionEdges()));

    selectEdgeLoop = new QAction(tr("Select Edge Loop"), this);;
    selectEdgeLoop->setStatusTip(tr("Select Edge Loop"));
    connect(selectEdgeLoop, SIGNAL(triggered()), this, SLOT(SelectEdgeLoop()));


    selectEdgeRing = new QAction(tr("Select Edge Ring"), this);;
    selectEdgeRing->setStatusTip(tr("Select Edge Ring"));
    connect(selectEdgeRing, SIGNAL(triggered()), this, SLOT(SelectEdgeRing()));

    selectEdgesFromFaces = new QAction(tr("Select Edges From Faces"), this);;
    selectEdgesFromFaces->setStatusTip(tr("Select Edges From Faces"));
    connect(selectEdgesFromFaces, SIGNAL(triggered()), this, SLOT(SelectEdgesFromFaces()));

    selectEdgesFromVertices = new QAction(tr("Select Edges From Vertices"), this);;
    selectEdgesFromVertices->setStatusTip(tr("Select Edges From Vertices"));
    connect(selectEdgesFromVertices, SIGNAL(triggered()), this, SLOT(SelectEdgesFromVertices()));


    /*
     Select Faces
    */
    selectAllFaces = new QAction(tr("Select All"), this);;
    selectAllFaces->setStatusTip(tr("Select All"));
    connect(selectAllFaces, SIGNAL(triggered()), this, SLOT(SelectAllFaces()));

    clearSelectionFaces = new QAction(tr("Clear Selected Faces"), this);;
    clearSelectionFaces->setStatusTip(tr("Clear Selected Faces"));
    connect(clearSelectionFaces, SIGNAL(triggered()), this, SLOT(ClearSelectionFaces()));

    selectInverseFaces = new QAction(tr( "Select Inverse Faces"), this);
    selectInverseFaces->setStatusTip(tr( "Select Inverse Faces"));
    connect(selectInverseFaces, SIGNAL(triggered()), this, SLOT(SelectInverseFaces()));

    selectMultipleFaces = new QAction(tr("Select Multiple Faces"), this);;
    selectMultipleFaces->setStatusTip(tr("Select Multiple Faces"));
    connect(selectMultipleFaces, SIGNAL(triggered()), this, SLOT(SelectMultipleFaces()));

    selectSimilarFaces = new QAction(tr("Select Similar Faces"), this);;
    selectSimilarFaces->setStatusTip(tr("Select Similar Faces"));
    connect(selectSimilarFaces, SIGNAL(triggered()), this, SLOT(SelectSimilarFaces()));

    selectWindowFaces = new QAction(tr("Select Window Faces"), this);;
    selectWindowFaces->setStatusTip(tr("Select Window Faces"));
    connect(selectWindowFaces, SIGNAL(triggered()), this, SLOT(SelectWindowFaces()));

    selectConnectedComponentFaces = new QAction(tr("Select Connected Component Faces"), this);;
    selectConnectedComponentFaces->setStatusTip(tr("Select Connected Component Faces"));
    connect(selectConnectedComponentFaces, SIGNAL(triggered()), this, SLOT(SelectConnectedComponentFaces()));

    selectFaceLoop = new QAction(tr("Select Face Loop"), this);;
    selectFaceLoop->setStatusTip(tr("Select Face Loop"));
    connect(selectFaceLoop, SIGNAL(triggered()), this, SLOT(SelectFaceLoop()));

    selectFacesFromVertices = new QAction(tr("Select Faces From Vertices"), this);;
    selectFacesFromVertices->setStatusTip(tr("Select Faces From Vertices"));
    connect(selectFacesFromVertices, SIGNAL(triggered()), this, SLOT(SelectFacesFromVertices()));

    selectFacesFromEdges= new QAction(tr("Select Faces From Edges"), this);;
    selectFacesFromEdges->setStatusTip(tr("Select Faces From Edges"));
    connect(selectFacesFromEdges, SIGNAL(triggered()), this, SLOT(SelectFacesFromEdges()));

    /*
     Select Vertices
    */

    /*
     Unfold MENU
    */

    onePiece = new QAction(tr("One Piece - Random"), this);
    onePiece->setStatusTip(tr("Unfold to one piece"));
    connect(onePiece, SIGNAL(triggered()), this, SLOT(createScaleDialog()));

    usingSelectionAsConnectors = new QAction(tr("Using Selection as connectors"), this);
    usingSelectionAsConnectors ->setStatusTip(tr("Using Selection as connectors"));
    connect(usingSelectionAsConnectors , SIGNAL(triggered()), this, SLOT(UseSelectionAsConnectors ()));
    usingSelectionAsFlaps= new QAction(tr("Using Selection as connectors"), this);
    usingSelectionAsFlaps->setStatusTip(tr("Using Selection as connectors"));
    connect(usingSelectionAsFlaps, SIGNAL(triggered()), this, SLOT(UseSelectionAsFlaps()));

    /*
     Display MENU
    */

    // show Original
    viewOriginal = new QAction(tr("Display Original"), this);
    viewOriginal->setStatusTip(tr("Display Original OBJ file"));
    viewOriginal->setCheckable(true);
    viewOriginal->setChecked(true);
    connect(viewOriginal, SIGNAL(triggered()), this, SLOT(ShowOriginal()));

    //Show Folded
    viewFolded = new QAction(tr("Display Folded"), this);
    viewFolded->setStatusTip(tr("Display Folded OBJ file"));
    viewFolded->setCheckable(true);
    viewFolded->setChecked(true);
    connect(viewFolded, SIGNAL(triggered()), this, SLOT(ShowFolded()));

    //Show unFolded
    viewUnfolded= new QAction(tr("Display UnFolded"), this);
    viewUnfolded->setStatusTip(tr("Display UnFolded OBJ file"));
    viewUnfolded->setCheckable(true);
    viewUnfolded->setChecked(true);
    connect(viewUnfolded, SIGNAL(triggered()), this, SLOT(ShowUnfolded()));

    //Show Triangle
    viewTriangle= new QAction(tr("Display Triangles"), this);
    viewTriangle->setStatusTip(tr("Display Triangles"));
    viewTriangle->setCheckable(true);
    viewTriangle->setChecked(true);
    connect(viewTriangle, SIGNAL(triggered()), this, SLOT(ShowTriangle()));

    //Show Shaded
    viewShaded = new QAction(tr("Display Polygons"), this);
    viewShaded->setStatusTip(tr("Display Polygons"));
    viewShaded->setCheckable(true);
    viewShaded->setChecked(true);
    connect(viewShaded, SIGNAL(triggered()), this, SLOT(ShowShaded()));

    viewVertexData = new QAction(tr("Display Vertex Data"), this);
    viewVertexData->setStatusTip(tr("Display Vertex Data"));
    viewVertexData->setCheckable(true);
    viewVertexData->setChecked(true);
    connect(viewVertexData, SIGNAL(triggered()), this, SLOT(ShowVertexData()));


    /*
     Flap Types MENU
    */

    // Use Flap type 1
    flapType1 = new QAction(QIcon(":/images/flapTypes/flapType1.png"), tr("Flap Type 1"), this);
    flapType1->setStatusTip(tr("Use Flap type 1"));
    flapType1->setCheckable(true);
    flapType1->setChecked(true);
    connect(flapType1, SIGNAL(triggered()), this, SLOT(FlapType1()));

    // Use Flap type 2
    flapType2 = new QAction(QIcon(":/images/flapTypes/flapType2.png"), tr("Flap Type 2"), this);
    flapType2->setStatusTip(tr("Use Flap type 2"));
    flapType2->setCheckable(true);
    flapType2->setChecked(true);
    connect(flapType2, SIGNAL(triggered()), this, SLOT(FlapType2()));

    // Use Flap type 3
    flapType3 = new QAction(QIcon(":/images/flapTypes/flapType3.png"), tr("Flap Type 3"), this);
    flapType3->setStatusTip(tr("Use Flap type 3"));
    flapType3->setCheckable(true);
    flapType3->setChecked(true);
    connect(flapType3, SIGNAL(triggered()), this, SLOT(FlapType3()));

    // Use Flap type 4
    flapType4 = new QAction(QIcon(":/images/flapTypes/flapType4.png"), tr("Flap Type 4"), this);
    flapType4->setStatusTip(tr("Use Flap type 4"));
    flapType4->setCheckable(true);
    flapType4->setChecked(true);
    connect(flapType4, SIGNAL(triggered()), this, SLOT(FlapType4()));

    /*
     Helps MENU
    */
    // About QT
    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(about()));



}
void MainWindow::createEditMenu()
{
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
//    editMenu->addAction(triangulate);

    menuBar()->addSeparator();
}

void MainWindow::createViewMenu()
{
    viewMenu = menuBar()->addMenu(tr("&Display"));
    viewMenu->addAction(viewOriginal);
    viewMenu->addAction(viewFolded);
    viewMenu->addAction(viewTriangle);
    viewMenu->addAction(viewUnfolded);
    viewMenu->addAction(viewShaded);
    viewMenu->addAction(viewVertexData);
}

void MainWindow::createHelpMenu()
{
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutQtAct);
}
void MainWindow::createMeshOperationsMenu()
{
    meshOperationsMenu = menuBar()->addMenu(tr("Mesh Operations"));
    meshOperationsMenu->addAction(triangulate);
}

void MainWindow::createFlapTypesMenu()
{
    flapTypesMenu = menuBar()->addMenu(tr("Flap Types"));
    flapTypesMenu->addAction(flapType1);
    flapTypesMenu->addAction(flapType2);
    flapTypesMenu->addAction(flapType3);
    flapTypesMenu->addAction(flapType4);
}


void MainWindow::createSelectEdgesMenu()
{
    selectEdges = menuBar()->addMenu(tr("Select Edges"));
    selectEdges->addAction(selectAllEdges);
    selectEdges->addAction(clearSelectionEdges);
    selectEdges->addAction(selectInverseEdges);
    selectEdges->addAction(selectMultipleEdges);
    selectEdges->addAction(selectSimilarEdges);
    selectEdges->addAction(selectWindowEdges);
    selectEdges->addAction(selectConnectedComponentEdges);
    selectEdges->addAction(selectShortestPathBetween2Vertices);
    selectEdges->addAction(growSelectionEdges);
    selectEdges->addAction(shrinkSelectionEdges);
    selectEdges->addAction(selectEdgeLoop);
    selectEdges->addAction(selectEdgeRing);
    selectEdges->addAction(selectEdgesFromFaces);
    selectEdges->addAction(selectEdgesFromVertices);
}
void MainWindow::createSelectFacesMenu()
{
    selectFaces = menuBar()->addMenu(tr("Select Faces"));
    selectFaces->addAction(selectAllFaces);
    selectFaces->addAction(clearSelectionFaces);
    selectFaces->addAction(selectInverseFaces);
    selectFaces->addAction(selectMultipleFaces);
    selectFaces->addAction(selectSimilarFaces);
    selectFaces->addAction(selectWindowFaces);
    selectFaces->addAction(selectConnectedComponentFaces);
    selectFaces->addAction(selectFaceLoop);
    selectFaces->addAction(selectFacesFromEdges);
    selectFaces->addAction(selectFacesFromVertices);
}
void MainWindow::createSelectVerticesMenu()
{
//    meshOperationsMenu = menuBar()->addMenu(tr("Mesh Operations"));
//    meshOperationsMenu->addAction(triangulate);
}





void MainWindow::createUnfoldMenu()
{
    unfoldMenu = menuBar()->addMenu(tr("&Unfold"));
    unfoldMenu->addAction(onePiece);
    unfoldMenu->addAction(usingSelectionAsConnectors);
    unfoldMenu->addAction(usingSelectionAsFlaps);

}

void MainWindow::createFileMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);

    saveAsMenu = fileMenu->addMenu(QIcon(":/images/saveas.png"),"&Save As");
    saveAsMenu->addAction(saveAsFoldedOBJ);
    saveAsMenu->addAction(saveAsUnfoldedOBJ);
    saveAsMenu->addAction(saveAsFoldedINF);
    saveAsMenu->addAction(saveAsUnfoldedINF);

    fileMenu->addAction(exportAct);
    fileMenu->addAction(printAct);
    fileMenu->addAction(exitAct);
    fileMenu->addSeparator();
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("QtProject", "Application Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("QtProject", "Application Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

//bool MainWindow::maybeSave()
//{
//    if (textEdit->document()->isModified()) {
//        QMessageBox::StandardButton ret;
//        ret = QMessageBox::warning(this, tr("Application"),
//                     tr("The document has been modified.\n"
//                        "Do you want to save your changes?"),
//                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
//        if (ret == QMessageBox::Save)
//            return save();
//        else if (ret == QMessageBox::Cancel)
//            return false;
//    }
//    return true;
//}

//void MainWindow::loadFile(const QString &fileName)
//{
//    QFile file(fileName);
//    if (!file.open(QFile::ReadOnly | QFile::Text)) {
//        QMessageBox::warning(this, tr("Application"),
//                             tr("Cannot read file %1:\n%2.")
//                             .arg(fileName)
//                             .arg(file.errorString()));
//        return;
//    }

//    QTextStream in(&file);
//#ifndef QT_NO_CURSOR
//    QApplication::setOverrideCursor(Qt::WaitCursor);
//#endif
//    textEdit->setPlainText(in.readAll());
//#ifndef QT_NO_CURSOR
//    QApplication::restoreOverrideCursor();
//#endif

//    setCurrentFile(fileName);
//    statusBar()->showMessage(tr("File loaded"), 2000);
//}

//bool MainWindow::saveFile(const QString &fileName)
//{
//    QFile file(fileName);
//    if (!file.open(QFile::WriteOnly | QFile::Text)) {
//        QMessageBox::warning(this, tr("Application"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(fileName)
//                             .arg(file.errorString()));
//        return false;
//    }

//    QTextStream out(&file);
//#ifndef QT_NO_CURSOR
//    QApplication::setOverrideCursor(Qt::WaitCursor);
//#endif
//    out << textEdit->toPlainText();
//#ifndef QT_NO_CURSOR
//    QApplication::restoreOverrideCursor();
//#endif

//    setCurrentFile(fileName);
//    statusBar()->showMessage(tr("File saved"), 2000);
//    return true;
//}

//void MainWindow::setCurrentFile(const QString &fileName)
//{
//    curFile = fileName;
//    textEdit->document()->setModified(false);
//    setWindowModified(false);

//    QString shownName = curFile;
//    if (curFile.isEmpty())
//        shownName = "untitled.txt";
//    setWindowFilePath(shownName);
//}
//QString MainWindow::strippedName(const QString &fullFileName)
//{
//    return QFileInfo(fullFileName).fileName();
//}


void MainWindow::exportToFile()
{

}

void MainWindow::print(){

}

void MainWindow::quitApp()
{

}




void MainWindow::createScaleDialog()
{
//    if(!ForExtendedMesh())
//        return;

    bool val = ForExtendedMesh();

//    string  Value0 = "Value";            // NOTE: these lines of code (the variables you wish to change)
//    bool    Value1  = true;              //  probably exist in your program already and so it is only
//    int     Value2      = 20;
    int     ScaleValue = 10;

    CustomDialog d(tr("Scale setting"), toolsWidgetDock);                            // Title for custom dialog
    d.addLabel    ("Set the scaling factor");           // The first element is just a text label (non interactive).
//    d.addLineEdit (input1+"  ", &Value0, "Tooltip value");             // line edit.
//    d.addCheckBox (input2+"  ", &Value1, "my tooltip");       // A checkbox with a tooltip (optional last argument).
//    d.addSpinBox  (input3+"  ", 1, 120, &Value2, 1);                   // A number spin box for age.
//    d.addComboBox ("Value: ", "Value1|Value2|Value3", &Value3);   // A combo box with three options (separated by '|').

    scaleSetting = new QSlider(Qt::Horizontal, this);
//            output = new Output();

    scaleSetting->setRange(20, 70);
    scaleSetting->setValue(20);
    connect(scaleSetting, SIGNAL(valueChanged(int)), this, SLOT(SetScale(int)));

    d.addSlider(scaleSetting,tr("Scale Setting"),&ScaleValue,tr("Scale"));

    QMainWindow *window = new QMainWindow(0);
    QToolBar *bar = new QToolBar(window);
    bar->addAction("First");
    window->addToolBar(bar);

    QMenu *menu = new QMenu("file", window);
    menu->addAction("one");
    menu->addAction("one");
    menu->addAction("one");

    window->menuBar()->addMenu(menu);
    window->setParent(toolsWidgetDock);
    toolsWidgetDock->setWidget(window);
    toolsWidgetDock->setVisible(true);

    d.exec();                             // Execution stops here until user closes dialog

    if(d.wasCancelled())                // If the user hit cancel, your values stay the same
        return;                           // and you probably don't need to do anything
//     cout<<ScaleValue << " ... : Scale value" << endl;   // and here it's up to you to do stuff with your new values!
}


void MainWindow::keyPressEvent(QKeyEvent *e)
{
    mainwidget->keyPressEvent(e);
}
