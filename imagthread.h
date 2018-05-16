#ifndef IMAGETHREAD_H
#define IMAGETHREAD_H

#include <QObject>
#include <QMap>
#include <QThread>
#include <QImage>
#include <QTextStream>

#include <mvsdk_iface.h>

class ImageThread  : public QThread
{
    Q_OBJECT
public:
    static ImageThread * getInstance();
    // setter
    void setParams(const QMap<QString, double> & params);

    void setParamsCapture(const QMap<QString, double> & params);
    void setParamsEdgeDect(const QMap<QString, double> & params,
                           const QString & method);
    void setParamsLineDect(const QMap<QString, double> & params);
    /**
     * @brief setCommonDetect
     * @param isDo
     * @warning must set all-params before set*Detect,
     * 		otherwise, behavior will be undefined and dangerous
     * 		all program crashes
     */
    void setCommonDetect(const bool isDo);
    void setExtendDetect(const bool isDo);

    // method
    void capOneFrame();
    void capStreamStart();
    void capStreamStop();

    void run() Q_DECL_OVERRIDE;

signals:
    void FinishOne(const QImage & output,
                   const QImage & raw,
                   const QString & log);

private:
    ImageThread();
    static const char * TAG;
    static ImageThread * instance;



    bool isCapStream;
    bool isCommonDetect;
    bool isExtendDetect;
    bool isInitSuccess;

    int imageWidth;
    int imageHeight;
    double ExposureTimeMS;
    uchar * mQImageBuffer;
    QImage mQImageOutput;
    QImage mQImageRaw;

    QString methodEdgeDect;
    QMap<QString, double> paramsEdgeDect;
    QMap<QString, double> paramsLineDect;

};

#endif // IMAGETHREAD_H
