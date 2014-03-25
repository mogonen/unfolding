#include "output.h"


bool Output::OutputOBJFile(QString name, BaseMesh *mesh) {
    if (!mesh) return false;
    std::cout << "Outputing OBJ file..." << std::endl;
    QString fileName = name + ".obj";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        std::cout <<"Open file sucessfully!" << std::endl;
        QTextStream stream(&file);
        QVector <Vertex*> vts = mesh->GetVertices();
        QMap <Vertex*, int> vTable;
        int num = 1;
        for (int i=0; i<vts.size(); ++i) {
            stream << "v " << vts[i]->x() << " " << vts[i]->y() << " " << vts[i]->z() << endl;
            vTable[vts[i]] = num;
            num++;
        }
        stream << "# " << vts.size() << " vertices" << endl;
        stream << endl;

        QVector <Face* > faces = mesh->GetFaces();
        for (int i=0; i<faces.size(); ++i) {
            QVector <Vertex* > vFace = faces[i]->GetVertices();
            stream << "f";
            for (int j=0; j<vFace.size(); ++j)
                stream << " " << vTable[vFace[j]];
            stream << endl;
        }
        stream << "# " << faces.size() << " faces" << endl;
    }
    file.close();
    return true;
}


bool Output::OutputINFFile(QString name, BaseMesh *mesh) {
    if (!mesh) return false;
    std::cout << "Outputing INF file..." << std::endl;
    QString fileName = name + ".inf";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        std::cout <<"Open file sucessfully!" << std::endl;
        QTextStream stream(&file);
        QVector <Vertex*> vts = mesh->GetVertices();
        QMap <Vertex*, int> vTable;
        int num = 1;
        for (int i=0; i<vts.size(); ++i) {
            stream << i+1 << ", " << vts[i]->x() << ", " << vts[i]->y() << ", " << vts[i]->z() << endl;
            vTable[vts[i]] = num;
            num++;
        }

        stream << endl;

        QVector <Face* > faces = mesh->GetFaces();
        for (int i=0; i<faces.size(); ++i) {
            QVector <Vertex* > vFace = faces[i]->GetVertices();
            stream << i+1;
            for (int j=0; j<vFace.size(); ++j)
                stream << ", " << vTable[vFace[j]];
            stream << endl;
        }
    }
    file.close();
    return true;
}
