#include <QMainWindow>
#include <QPlainTextEdit>
#include "window.h"
#include "mainwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow(){ Clear();}


protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void newFile();
    void open();
    void save();
    bool saveAs();
    void exportToFile();
    void print();
    void quitApp();



    void SelectAllEdges();
    void ClearSelectionEdges();
    void SelectInverseEdges();
    void SelectMultipleEdges();
    void SelectSimilarEdges();
    void SelectWindowEdges();
    void SelectConnectedComponentEdges();
    void SelectShortestPathBetween2Vertices();
    void GrowSelectionEdges();
    void ShrinkSelectionEdges();
    void SelectEdgeLoop();
    void SelectEdgeRing();
    void SelectEdgesFromFaces();
    void SelectEdgesFromVertices();

    void SelectAllFaces();
    void ClearSelectionFaces();
    void SelectInverseFaces();
    void SelectMultipleFaces();
    void SelectSimilarFaces();
    void SelectWindowFaces();
    void SelectConnectedComponentFaces();
    void SelectFaceLoop();
    void SelectFacesFromEdges();
    void SelectFacesFromVertices();

    void about();
    void ShowOriginal();
    void ShowFolded();
    void ShowUnfolded();
    void ShowTriangle();

    void ShowShaded();
    void ShowVertexData();

    void FlapType1();
    void FlapType2();
    void FlapType3();
    void FlapType4();


    void documentWasModified();
    void LoadOBJFile();
    void TriangulateMesh();
    bool ForExtendedMesh();
    void createScaleDialog();

    void UseSelectionAsConnectors();
    void UseSelectionAsFlaps();
    void SetScale(int k);

    void OutputFoldedOBJ();
    void OutputUnfoldedOBJ();
    void OutputFoldedINF();
    void OutputUnFoldedINF();


private:

    BaseMesh* CreateBaseMesh();
    BaseMesh* CreateTriangleMesh(BaseMesh* mesh);
    ExtendedMesh* CreateExtendedMesh(BaseMesh* triMesh);
    QSlider* scaleSetting;

    void Clear();

    void createAllGuiObjects();
    void createActions();

    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createHelpMenu();
    void createUnfoldMenu();
    void createMeshOperationsMenu();
    void createFlapTypesMenu();

    void createSelectEdgesMenu();
    void createSelectFacesMenu();
    void createSelectVerticesMenu();

//    void createScaleDialog(QString title, QSlider* scaleSetting);

    void createToolBars();

    void createStatusBar();

    void readSettings();

    void writeSettings();

    bool maybeSave();

    void loadFile(const QString &fileName);

    bool saveFile(const QString &fileName);

    void setCurrentFile(const QString &fileName);

    QString strippedName(const QString &fullFileName);

    QPlainTextEdit *textEdit;
    QString curFile;

    QPushButton* loadOBJ;


    QMenu *fileMenu;
    QToolBar *fileToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QMenu *saveAsMenu;
    QAction* saveAsFoldedOBJ;
    QAction* saveAsUnfoldedOBJ;
    QAction* saveAsFoldedINF;
    QAction* saveAsUnfoldedINF;
    QAction *exportAct;
    QAction *printAct;
    QAction *exitAct;

    QMenu *editMenu;
    QToolBar *editToolBar;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    QMenu *meshOperationsMenu;
    QAction* triangulate;

    QMenu *unfoldMenu;
    QAction* onePiece;
    QAction* usingSelectionAsConnectors;
    QAction* usingSelectionAsFlaps;

    QMenu *flapTypesMenu;
    QAction* flapType1;
    QAction* flapType2;
    QAction* flapType3;
    QAction* flapType4;


    QMenu *selectEdges;
    QAction *selectAllEdges;
    QAction *clearSelectionEdges;
    QAction *selectInverseEdges;
    QAction *selectMultipleEdges;
    QAction *selectSimilarEdges;
    QAction *selectWindowEdges;
    QAction *selectConnectedComponentEdges;
    QAction *selectShortestPathBetween2Vertices;
    QAction *growSelectionEdges;
    QAction *shrinkSelectionEdges;
    QAction *selectEdgeLoop;
    QAction *selectEdgeRing;
    QAction *selectEdgesFromFaces;
    QAction *selectEdgesFromVertices;


    QMenu *selectFaces;
    QAction *selectAllFaces;
    QAction *clearSelectionFaces;
    QAction *selectInverseFaces;
    QAction *selectMultipleFaces;
    QAction *selectSimilarFaces;
    QAction *selectWindowFaces;
    QAction *selectConnectedComponentFaces;
    QAction *selectFaceLoop;
    QAction *selectFacesFromEdges;
    QAction *selectFacesFromVertices;

    QMenu *selectVertices;
    QAction *selectVerticesFromFaces;
    QAction *selectVerticesFromEdges;


    QMenu *helpMenu;
    QAction *aboutQtAct;

    QMenu *viewMenu;
    QAction *viewOriginal;
    QAction *viewFolded;
    QAction *viewTriangle;
    QAction *viewUnfolded;

    QAction *viewShaded;
    QAction *viewVertexData;

    QDockWidget *toolsWidgetDock;

    MainWidget *mainwidget;    // OWNED
    BaseMesh* baseMesh;  // OWNED
    BaseMesh* triangleMesh; // OWNED
    ExtendedMesh* extendedMesh; // OWNED
    Output* output; // OWNED
    QString fileName;

    qreal scaleRatio;
};
