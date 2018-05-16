#ifndef QJSONMETHOD_H
#define QJSONMETHOD_H

#include <QObject>
#include <QJsonDocument>
#include <QVector>
#include <QJsonArray>
#include <QJsonObject>

class qJsonMethod : public QObject
{
    Q_OBJECT
public:
    static void FileToJDoc(const QString & iFilename,
                           QJsonDocument & oJDoc);
    static void JDocToFile(const QJsonDocument & iJDoc,
                           const QString & oFilename);

    static void JArrayToVector(const QJsonArray & iJArray,
                               QVector<QVector<QString>> & oVector);
    static void VectorToJArray(const QVector<QVector<QString>> & iVector,
                               QJsonArray & oJArray);
private:
    qJsonMethod();

};

#endif // QJSONMETHOD_H
