#include "common_detect.h"
#include <QDebug>
#include <QFile>
#include <QTime>
#include <QDir>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#include <string>
#include<cmath>
using namespace std;

#include "dbscan.h"

#define DEBUG 0

// [1] self data structure
class dot {
    public:
        double m;
        double b;
        dot(double in_m, double in_b) {
            m = in_m;
            b = in_b;
        }
};
// ![1]

// [2] self DBSCAN-child-class
class dotDBSCAN : public DBSCAN {
    public:
        dotDBSCAN(const double minPts = 1, const double minEps = 0) {
            set_minPts(minPts);
            set_minEps(minEps);
        }
    protected:
        bool compare_distance(const void * l, const void * r) {
            double deltaX = abs(((dot *)l)->m - ((dot *)r)->m);
            double deltaY = abs(((dot *)l)->b - ((dot *)r)->b);
            if(deltaX > 0.5)
                return false;
            if(deltaY > 50)
                return false;
            return true;
        }
        void rawdata_string(const void * rawdata, std::stringstream & ss) {
            dot * elem = (dot *)rawdata;
            ss << elem->m << " " << elem->b << '\n';
        }
        double calculate_distance(const void * l, const void * r) {
            Q_UNUSED(l);
            Q_UNUSED(r);
            return 0.0;
        }
};
// ![2]

const QString CommonDetect::TAG = "ImageThread";
void CommonDetect::doCore(const Mat &src, Mat &dst,
                          std::vector<cv::Vec4i> lines,
                          QString & log, QString & linesCSV,
                          const QString & methodEdgeDect,
                          const QMap<QString, double> & paramsEdgeDect,
                          const QMap<QString, double> & paramsLineDect) {
    log.clear();
    QDebug mDebug(&log);

    qDebug() << TAG << " methodEdgeDect = " << methodEdgeDect;
    qDebug() << TAG << " paramsEdgeDect = " << paramsEdgeDect;
    qDebug() << TAG << " paramsLineDect = " << paramsLineDect;

    Mat tmp;
    if(methodEdgeDect == "Canny") {
        Canny(src, tmp, paramsEdgeDect["threshold1"],
                        paramsEdgeDect["threshold2"],
                        (int)(paramsEdgeDect["apertureSize"]));
    } else if(methodEdgeDect == "Sobel") {
        Sobel(src, tmp, paramsEdgeDect["ddepth"],
                        (int)paramsEdgeDect["dx"],
                        (int)paramsEdgeDect["dy"],
                        (int)paramsEdgeDect["ksize"]);
    } else if(methodEdgeDect == "Laplacian") {
        Laplacian(src, tmp, (int)paramsEdgeDect["ddepth"],
                        (int)paramsEdgeDect["ksize"]);
    } else {
        qDebug() << TAG << " methodEdgeDect: " << methodEdgeDect << " is not supported";
        mDebug << TAG << " methodEdgeDect: " << methodEdgeDect << " is not supported";
    }

    HoughLinesP(tmp, lines, paramsLineDect["rho"],
                    paramsLineDect["thetaAngle"] * CV_PI / 180.0,
                    (int)paramsLineDect["threshold"],
                    paramsLineDect["minLineLength"],
                    paramsLineDect["minLineGap"]);

    qDebug() << TAG << " HoughLinesP get number of lines = " << lines.size();
    mDebug << TAG << " HoughLinesP get number of lines = " << lines.size();
    cvtColor(tmp, dst, CV_GRAY2BGR);

    vector<dot> dotVector;
    vector<void *> dataset;

    QTextStream fDataStream(&linesCSV);
    fDataStream << "P0x,P0y,P1x,P1y,m,b,Length\n";
    for(unsigned int i = 0; i < lines.size(); ++i) {
        // draw line
        line(dst, Point(lines[i][0], lines[i][1]),
                        Point(lines[i][2], lines[i][3]),
                        Scalar(0,255, 0),
                        1);
        // draw points at begin and end of a single line
        circle(dst, Point(lines[i][0], lines[i][1]), 1, Scalar(0, 0, 255));
        circle(dst, Point(lines[i][2], lines[i][3]), 1, Scalar(0, 0, 255));

        int j = 0;

        for(; j < 4; ++j) {
            fDataStream << (quint32)lines[i][j] << ',';
        }
        double deltaY = (lines[i][1] - lines[i][3]);
        double deltaX = (lines[i][0] - lines[i][2]);
        double m = deltaY / deltaX;
        double b = lines[i][1] - m*lines[i][0];

        fDataStream << m << ',';
        fDataStream << b << ',';
        fDataStream << abs(deltaX) + abs(deltaY) << '\n';
        dotVector.push_back(dot(m, b));
        dataset.push_back((void *)&dotVector[dotVector.size() - 1]);
    }


    // for(unsigned i = 0; i < vxVector.size(); ++i)
    //    dats.push_back((void *)&vxVector[i]);
    // [4] perform cluster
    vector<int> result;
    int numCluster = -1;
    string logDBSCAN;
    dotDBSCAN mDBSCAN;
    mDBSCAN.set_log(true);
    // enable log, so that will be obtained
    mDBSCAN.cluster(dataset, DBSCAN::distance_type_bool,
            result, numCluster, logDBSCAN);
    // ![4]

    qDebug() << "After using DBSCAN";
    // group lines for every cluster
    vector<vector<int>> final_lines(numCluster);
    for(unsigned i = 0; i < dotVector.size(); ++i) {
        if(result[i] != -1)
            final_lines[result[i]].push_back(i);
        qDebug() << dotVector[i].m << " " << dotVector[i].b << " " << result[i];
    }

    // average m and b for each group lines
    for(unsigned i = 0; i < final_lines.size(); ++i) {
        unsigned len = final_lines[i].size();
        double m_sum = 0;
        double b_sum = 0;
        for(unsigned j = 0; j < len; ++j) {
            m_sum += dotVector[final_lines[i][j]].m;
            b_sum += dotVector[final_lines[i][j]].b;
            qDebug() << i << " : " << final_lines[i][j];
        }
        m_sum /= len;
        b_sum /= len;
        qDebug() << "new line[ " << i << " ] m = " << m_sum << " b = " << b_sum;
        mDebug << "new line[ " << i << " ] m = " << m_sum << " b = " << b_sum << "\n";
    }
    qDebug() << "< -- >";

#if DEBUG
    imshow("No.1 src", src);
    imshow("No.2 Canny", tmp);
    imshow("No.3 HoughP", dst);
#endif
}
