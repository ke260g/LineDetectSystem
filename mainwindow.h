#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QJsonObject>
#include <QMap>
#include <QLabel>
#include <QResizeEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_qBtnCaptureStream_clicked(bool checked);
    void on_qBtnDoDetect_clicked(bool checked);
    void on_qBtnCaptureOneFrame_clicked();
    void on_qBtnParamsUpdate_clicked();

    void on_qComboBoxLineDect_currentTextChanged(const QString &arg1);

    void on_qBtnParamsSave_clicked();

private:
    Ui::MainWindow *ui;

    QLabel *qLabelImage;

    float imageScrollFactor;
    bool qLabelImageResize(QObject *obj, QEvent *ev);
    bool qScrollAreaVBar(QObject *obj, QEvent *ev);

    // to make 'value'-column of qTreeParams and qTreeResults editable
    void onTreeDoubleClick(QTreeWidgetItem * item, const int column);

    QJsonObject JObjcParamsEdgeDect;
    void setSelectEdgeDect();
    void setParamsCapture();
    void setParamsEdgeDect(const QString & method);
    void setParamsLineDect();

    void onImageThreadFinishOne(const QImage & output,
                                const QImage & raw,
                                const QString & log);
protected:
    bool eventFilter(QObject *obj, QEvent *ev);
};

#endif // MAINWINDOW_H
