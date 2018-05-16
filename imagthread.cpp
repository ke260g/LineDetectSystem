#include "imagthread.h"
#include <string>
#include <QImage>
#include <QDebug>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "common_detect.h"
#include <vector>
using namespace cv;



ImageThread *ImageThread::instance = new ImageThread;
const char *ImageThread::TAG = "ImageThread";
#define WaitThreadFinishMS (500)

ImageThread::ImageThread() {
    isCapStream = false;
    isCommonDetect = false;
    isExtendDetect = false;
    int retval;

    // 1: init
    retval = mvsdk_init();
    if(retval) {
        printf("%s mvsdk_init() error occurs\n", TAG);
        isInitSuccess = false;
        return;
    }

    /*
     * For unknown reason,
     * sometimes it can not be obtained resolution firstly.
     */
    for(unsigned i = 0; i < 4; ++i) {
        // 2: get resolution
        mvsdk_get_resolution(&imageHeight, &imageWidth);
        printf("%s mvsdk resolution: %d * %d\n", TAG,
               imageHeight, imageWidth);
        if(imageHeight == 0 && imageWidth == 0) {
            isInitSuccess = false;
            continue;
        } else {
            break;
        }
    }

    // 3: get product name
    char product_name[12];
    retval = mvsdk_get_product_name(product_name, 12);
    if(retval < 0)
        printf("%s mvsdk_get_product_name() failed\n", TAG);
    else
        printf("%s mvsdk product_name: %s\n", TAG, product_name);

    // 4: set default expouse time
    ExposureTimeMS = 100;
    double ExpouseTime = ExposureTimeMS * 1000;
    mvsdk_set_exposure_time(ExpouseTime);

    // 5: alloc memory space for image
    mQImageBuffer = new uchar[imageHeight * imageWidth * 3];
    isInitSuccess = true;
}

ImageThread * ImageThread::getInstance() {
    return instance;
}

// setter
void ImageThread::setParams(const QMap<QString, double> & params) {
    qDebug() << params;

    ExposureTimeMS = params["ExposureTime"];
    double ExpouseTime = ExposureTimeMS * 1000;
    mvsdk_set_exposure_time(ExpouseTime);
}

// method
void ImageThread::capOneFrame() {
    isCapStream = false;
    while(isRunning()) {
        msleep(WaitThreadFinishMS);
    }
    start();
}

void ImageThread::capStreamStart() {
    isCapStream = true;
    if(isRunning())
        return;
    start();
}

void ImageThread::capStreamStop() {
    isCapStream = false;
    while(isRunning())
        msleep(WaitThreadFinishMS);
}


void ImageThread::run() {
    int retval = 0;
    void *imgbuf;

    QString log, linesCSV;

    if(isInitSuccess == false)
        return;
    qDebug() << TAG << " " << "run begins";
    Mat imgRawMat(imageHeight, imageWidth, CV_8UC1);
    Mat dst;

    do {
        // get image
        retval = mvsdk_get_image(&imgbuf);
        if(retval) {
            qDebug() <<  TAG << " " << " capture error";
            continue;
        }
        memcpy(mQImageBuffer, imgbuf, imageHeight * imageWidth);
        mQImageRaw = QImage(mQImageBuffer,
                               imageWidth, imageHeight,
                               imageWidth,
                               QImage::Format_Indexed8);

        std::vector<cv::Vec4i> lines;
        if(isCommonDetect) {
            memcpy(imgRawMat.data, imgbuf, imageHeight * imageWidth);
            CommonDetect::doCore(imgRawMat, dst,
                                 lines, log, linesCSV,
                                 methodEdgeDect,
                                 paramsEdgeDect,
                                 paramsLineDect);
            memcpy(mQImageBuffer, dst.data, dst.rows * dst.step);
            qDebug() << dst.rows << " " << dst.step;
            mQImageOutput = QImage(mQImageBuffer,
                               imageWidth, imageHeight,
                               dst.step,
                               QImage::Format_RGB888);
        } else {
            mQImageOutput = mQImageRaw;
        }
        emit instance->FinishOne(mQImageOutput, mQImageRaw, log);
    } while(isCapStream);
    qDebug() << TAG << " " << "run ends";
}

void ImageThread::setParamsCapture(const QMap<QString, double> & params) {
    ExposureTimeMS = params["ExposureTime"];
    double ExpouseTime = ExposureTimeMS * 1000;
    mvsdk_set_exposure_time(ExpouseTime);
}

void ImageThread::setParamsEdgeDect(const QMap<QString, double> & params,
                                    const QString & method) {
    paramsEdgeDect = params;
    methodEdgeDect = method;
}

void ImageThread::setParamsLineDect(const QMap<QString, double> & params) {
    paramsLineDect = params;
}


void ImageThread::setCommonDetect(const bool isDo) {
    isCommonDetect = isDo;
}
