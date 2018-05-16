#include "qjsonmethod.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QIODevice>
#include <QJsonObject>



void qJsonMethod::FileToJDoc(const QString & iFilename,
                             QJsonDocument & oJdoc) {
    QFile file;
    QString configString;
    file.setFileName(iFilename);

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    configString = file.readAll();
    file.close();

    oJdoc = QJsonDocument::fromJson(configString.toUtf8());
}

void qJsonMethod::JDocToFile(const QJsonDocument & iJDoc,
                             const QString & oFilename) {
    QFile jsonfile(oFilename);
    jsonfile.open(QFile::WriteOnly);
    jsonfile.write(iJDoc.toJson());
    jsonfile.close();
}

void qJsonMethod::JArrayToVector(const QJsonArray & iJArray,
                                 QVector<QVector<QString>> & oVector) {
    oVector.clear();
    for(int i = 0; i < iJArray.size(); ++i) {
        QVector<QString> eachRow;
        QJsonObject rowJson = iJArray[i].toObject();
        eachRow.append(rowJson["name"].toString());
        eachRow.append(rowJson["value"].toString());
        eachRow.append(rowJson["comment"].toString());
        oVector.append(eachRow);
    }
}


void qJsonMethod::VectorToJArray(const QVector<QVector<QString>> & iVector,
                                 QJsonArray & oJArray) {
    QJsonArray mJArray;

    // clear oJArray;
    int len = oJArray.size();
    for(int i = 0; i < len; ++i)
        oJArray.removeFirst();

    for(int i = 0; i < iVector.size(); ++i) {
        QJsonObject eachRow;
        QString key;

        key = "name";
        QJsonValue nameValue(iVector[i][0]);
        eachRow.insert(key, nameValue);

        key = "value";
        QJsonValue valueValue(iVector[i][1]);
        eachRow.insert(key, valueValue);

        key = "comment";
        QJsonValue commentValue(iVector[i][2]);
        eachRow.insert(key, commentValue);

        QJsonValue itemValue(eachRow);
        oJArray.append(itemValue);
    }
}
