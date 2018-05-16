#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QMap>
#include <QKeyEvent>
#include <QScrollBar>
#include <QVariant>
#include <QDate>
#include <QDateTime>

#include "qtreemethod.h"
#include "qjsonmethod.h"
#include "imagthread.h"

#define CommonDetectPrefix "common_detect/"
#define FilenameParamsCapture CommonDetectPrefix"params_capture.json"
#define FilenameParamsEdgeDect CommonDetectPrefix"params_edge_dect.json"
#define FilenameParamsLineDect CommonDetectPrefix"params_line_dect.json"

struct ScaleFactor {
    int now;
    int max;
    int min;
    double value;

} imageSF = {
    .now = 255,
    .max = 255,
    .min = 75,
    .value = 1.0,
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qLabelImage = new QLabel();
    qLabelImage->setBackgroundRole(QPalette::Base);
    qLabelImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    qLabelImage->setScaledContents(true);

    ui->qScrollArea->setBackgroundRole(QPalette::Base);
    ui->qScrollArea->setWidget(this->qLabelImage);
    ui->qScrollArea->installEventFilter(this);
    ui->qScrollArea->verticalScrollBar()->installEventFilter(this);

    connect(ui->qTreeParamsCapture, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::onTreeDoubleClick);
    connect(ui->qTreeParamsEdgeDect, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::onTreeDoubleClick);
    connect(ui->qTreeParamsLineDect, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::onTreeDoubleClick);

    connect(ImageThread::getInstance(), &ImageThread::FinishOne,
            this, &MainWindow::onImageThreadFinishOne);

    setParamsCapture();
    setSelectEdgeDect();
    setParamsLineDect();
    on_qBtnParamsUpdate_clicked();
}

void MainWindow::onImageThreadFinishOne(const QImage & output,
                                        const QImage & raw,
                                        const QString & log) {
    qDebug() << "finish one";

    ui->qTextEdit->append("--- Finish One Begins ---");
    ui->qTextEdit->append(QDateTime::currentDateTime().toString());
    QScrollBar *mScrollBar = ui->qTextEdit->verticalScrollBar();
    ui->qTextEdit->append(log);
    ui->qTextEdit->append("--- Finish One Ends ---\n");
    mScrollBar->setValue(mScrollBar->maximum());

    qLabelImage->setPixmap(QPixmap::fromImage(output));
    if(qLabelImage->pixmap() != nullptr)
        qLabelImage->resize(imageSF.value * qLabelImage->pixmap()->size());
}

void MainWindow::onTreeDoubleClick(QTreeWidgetItem * item, const int column) {
    Qt::ItemFlags prev_flags = item->flags();
    if (column == 1) {
        item->setFlags(prev_flags | Qt::ItemIsEditable);
    } else if (prev_flags & Qt::ItemIsEditable) {
        item->setFlags(prev_flags ^ Qt::ItemIsEditable);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev) {
    if (obj == qLabelImage || obj == ui->qScrollArea)
        return qLabelImageResize(obj, ev);
    else if(obj == ui->qScrollArea->verticalScrollBar())
        return qScrollAreaVBar(obj, ev);
    else
        return QMainWindow::eventFilter(obj, ev);
    return true;

}

bool MainWindow::qScrollAreaVBar(QObject *obj, QEvent *ev) {
    Q_UNUSED(obj);
    if(ev->type() == QEvent::Wheel)
        return true;
    else
        return false;
}

// to resize showing image with ctrl-mouseWheel
bool MainWindow::qLabelImageResize(QObject *obj, QEvent *ev) {
    Q_UNUSED(obj);
    QWheelEvent *wheel_event = static_cast<QWheelEvent *>(ev);
    QKeyEvent *key_event = static_cast<QKeyEvent *>(ev);
    bool isCtrl = key_event->modifiers() & Qt::ControlModifier;

    do {
        if (ev->type() == QEvent::Wheel && isCtrl) {
            int direct = wheel_event->angleDelta().y();

            if(qLabelImage->pixmap() == NULL)
                break;

            // int barval = ui->qScrollArea->verticalScrollBar()->value();
            // ui->qScrollArea->verticalScrollBar()->setValue(barval);
            if(direct > 0 && imageSF.now < imageSF.max) {
                ++imageSF.now;
            } else if(direct < 0 && imageSF.now > imageSF.min) {
                --imageSF.now;
            }

            imageSF.value = (float)imageSF.now/(float)imageSF.max;
            qLabelImage->resize(imageSF.value * qLabelImage->pixmap()->size());
        }
    } while(0);

    return false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_qBtnCaptureStream_clicked(bool checked)
{
    if(checked) {
        ui->qBtnCaptureOneFrame->setEnabled(false);
        ImageThread::getInstance()->capStreamStart();
        qDebug() << "do CaptureStream";

    } else {
        ui->qBtnCaptureOneFrame->setEnabled(true);
        ImageThread::getInstance()->capStreamStop();
        qDebug() << "stop CaptureStream";
    }
}

void MainWindow::on_qBtnCaptureOneFrame_clicked()
{
    ImageThread::getInstance()->capOneFrame();
}

void MainWindow::on_qBtnDoDetect_clicked(bool checked)
{
    ImageThread::getInstance()->setCommonDetect(checked);
}

void MainWindow::on_qBtnParamsUpdate_clicked()
{
    // obtain params
    QMap<QString, double> paramsCapture;
    QMap<QString, double> paramsEdgeDect;
    QMap<QString, double> paramsLineDect;

    qTreeMethod::getParams(ui->qTreeParamsCapture, paramsCapture);
    qTreeMethod::getParams(ui->qTreeParamsEdgeDect, paramsEdgeDect);
    qTreeMethod::getParams(ui->qTreeParamsLineDect, paramsLineDect);

    QString SelectMethod = ui->qComboBoxLineDect->currentText();

    // set to ImageThread
    ImageThread::getInstance()->setParamsCapture(paramsCapture);
    ImageThread::getInstance()->setParamsEdgeDect(paramsEdgeDect,
                                                  SelectMethod);
    ImageThread::getInstance()->setParamsLineDect(paramsLineDect);

    // save new params to JObjcParamsEdgeDect
    QVector<QVector<QString>> params;
    qTreeMethod::getParams(ui->qTreeParamsEdgeDect, params);
    QJsonArray JArray;
    qJsonMethod::VectorToJArray(params, JArray);
    JObjcParamsEdgeDect[SelectMethod] = JArray;

}

// set combobox select of EdgeDect
void MainWindow::setSelectEdgeDect() {
    QJsonDocument JDoc;
    QString filename(FilenameParamsEdgeDect);
    qJsonMethod::FileToJDoc(filename,
                            JDoc);
    JObjcParamsEdgeDect = JDoc.object();
    ui->qComboBoxLineDect->addItems(JObjcParamsEdgeDect.keys());
}

// set params on the front-end
void MainWindow::setParamsCapture() {
    QJsonDocument JDoc;
    QString filename(FilenameParamsCapture);
    qJsonMethod::FileToJDoc(filename,
                            JDoc);
    QJsonArray JArray = JDoc.array();
    QVector<QVector<QString>> parmasVector;
    qJsonMethod::JArrayToVector(JArray,
                                parmasVector);
    qTreeMethod::setParams(ui->qTreeParamsCapture,
                           parmasVector);
}

void MainWindow::setParamsEdgeDect(const QString & method) {
    if(JObjcParamsEdgeDect.contains(method) == false)
        return;

    QJsonArray JArray = JObjcParamsEdgeDect[method].toArray();
    QVector<QVector<QString>> parmasVector;
    qJsonMethod::JArrayToVector(JArray, parmasVector);
    qTreeMethod::setParams(ui->qTreeParamsEdgeDect,
                           parmasVector);
}

void MainWindow::setParamsLineDect() {
    QJsonDocument JDoc;
    QString filename(FilenameParamsLineDect);
    qJsonMethod::FileToJDoc(filename,
                            JDoc);
    QJsonArray JArray = JDoc.array();

    QVector<QVector<QString>> parmasVector;
    qJsonMethod::JArrayToVector(JArray,
                                parmasVector);
    qTreeMethod::setParams(ui->qTreeParamsLineDect,
                           parmasVector);
}

void MainWindow::on_qComboBoxLineDect_currentTextChanged(const QString & method) {
    setParamsEdgeDect(method);
}

void MainWindow::on_qBtnParamsSave_clicked() {
     // obtain params
    QVector<QVector<QString>> paramsCapture;
    QVector<QVector<QString>> paramsEdgeDect;
    QVector<QVector<QString>> paramsLineDect;

    qTreeMethod::getParams(ui->qTreeParamsCapture, paramsCapture);
    qTreeMethod::getParams(ui->qTreeParamsEdgeDect, paramsEdgeDect);
    qTreeMethod::getParams(ui->qTreeParamsLineDect, paramsLineDect);

    QString SelectMethod = ui->qComboBoxLineDect->currentText();

    QJsonArray paramsJArray;
    QJsonDocument JDoc;

    // save to file
    qJsonMethod::VectorToJArray(paramsCapture, paramsJArray);
    JDoc = QJsonDocument(paramsJArray);
    qJsonMethod::JDocToFile(JDoc, FilenameParamsCapture);

    // save new params to JObjcParamsEdgeDect
    QVector<QVector<QString>> params;
    qTreeMethod::getParams(ui->qTreeParamsEdgeDect, params);
    QJsonArray JArray;
    qJsonMethod::VectorToJArray(params, JArray);
    JObjcParamsEdgeDect[SelectMethod] = JArray;
    // save to file
    JDoc = QJsonDocument(JObjcParamsEdgeDect);
    qJsonMethod::JDocToFile(JDoc, FilenameParamsEdgeDect);

    // save to file
    qJsonMethod::VectorToJArray(paramsLineDect, paramsJArray);
    JDoc = QJsonDocument(paramsJArray);
    qJsonMethod::JDocToFile(JDoc, FilenameParamsLineDect);
}
