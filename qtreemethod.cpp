#include "qtreemethod.h"
#include <QDebug>

#include <QFile>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QDebug>


void qTreeMethod::setParams(QTreeWidget * tree,
                            QVector<QVector<QString>> & params) {
    if(tree == nullptr)
        return;
    tree->clear();

    const int FrameWidth = tree->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    int treeHeightPixels = tree->header()->height() + (2*FrameWidth);

    for(const QVector<QString> & eachRow : params) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        for(int i = 0; i < eachRow.length(); ++i) {
            item->setText(i, eachRow[i]);
            item->setFirstColumnSpanned(false);
            tree->addTopLevelItem(item);
            tree->resizeColumnToContents(i);
        }
        treeHeightPixels += tree->visualItemRect(item).height();
    }
    tree->setMinimumHeight(treeHeightPixels);
}


void qTreeMethod::getParams(QTreeWidget *tree,
                            QMap<QString, double> &params) {
    QTreeWidgetItemIterator it(tree);
    params.clear();
    while (*it) {
        params[(*it)->text(0)] = (*it)->text(1).toDouble();
        ++it;
    }
}


void qTreeMethod::getParams(QTreeWidget * tree,
                            QVector<QVector<QString>> & params) {
    QTreeWidgetItemIterator it(tree);
    params.clear();
    while (*it) {
        QVector<QString> eachRow;
        for(int i = 0; i < (*it)->columnCount(); ++i)
            eachRow.append((*it)->text(i));
        params.append(eachRow);
        ++it;
    }
}
