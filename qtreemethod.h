#ifndef QTREEMETHOD_H
#define QTREEMETHOD_H

#include <QTreeWidget>
#include <QVector>
#include <QMap>

/**
 * @brief The qTreeMethod class
 * @warning never check the dimension of between tree and params
 */
class qTreeMethod: public QObject
{
    Q_OBJECT
public:
    static void setParams(QTreeWidget * tree,
                          QVector<QVector<QString>> & params);
    static void getParams(QTreeWidget * tree,
                          QMap<QString, double> & params);
    static void getParams(QTreeWidget * tree,
                          QVector<QVector<QString>> & params);
private:
    qTreeMethod();
};

#endif // QTREEMETHOD_H
