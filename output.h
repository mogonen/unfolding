#ifndef OUTPUT_H
#define OUTPUT_H

#include "basemesh.h"
#include <QString>
#include <QIODevice>
#include <QTextStream>
#include <QMap>
#include <QFile>
#include <iostream>

class Output {
public:
    Output() { }
    ~Output() { }
    bool OutputOBJFile(QString name, BaseMesh* mesh);
    bool OutputINFFile(QString name, BaseMesh* mesh);
};

#endif // OUTPUT_H
