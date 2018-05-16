#ifndef COMMON_DETECT_H
#define COMMON_DETECT_H

#include <opencv2/core/core.hpp>
#include <vector>

#include <QObject>
#include <QString>
#include <QMap>

class CommonDetect : public QObject {
    Q_OBJECT
public:
    /**
     * @brief doCommonDetect
     * @param dst result image
     * @param src source input image
     * @param lines output result of HoughLines()
     * @param log output log
     * @param methodEdgeDect
     * @param paramsEdgeDect input params for edgeDect()
     * @param paramsLineDect input params for HoughP()
     * @warning never check the matching
     *          between `paramsEdgeDect' and `methodEdgeDetect'\
     */
    static void doCore(const cv::Mat & src,
                       cv::Mat & dst,
                       std::vector<cv::Vec4i> lines,
                       QString & log, QString & linesCSV,
                       const QString & methodEdgeDect,
                       const QMap<QString, double> & paramsEdgeDect,
                       const QMap<QString, double> & paramsLineDect);
private:
    CommonDetect();
    static const QString TAG;
};

#endif // COMMON_DETECT_H
