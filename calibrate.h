#ifndef CALIBRATE_H
#define CALIBRATE_H
#include <QImage>
#include <QThread>
#include <QDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <QTime>

using namespace cv;

class Calibrate: public QThread
{
    Q_OBJECT
public:
    Calibrate();
    ~Calibrate();


    int addCirclebordPoints(QStringList filelist, cv::Size &boardSize);

    bool findCircleBoardCorners(cv::Mat image,cv::Size boardSize, std::vector<cv::Point2f> &realCorners);

    void addPoints(const std::vector<cv::Point2f> &imageCorners,const std::vector<cv::Point3f>& objectCorners);

    void getValidScope(cv::Mat imgIn, cv::Size boardSize, Mat &imgOut);

    //开始标定返回重投影误差
    double CalibrateCamera(cv::Size &imageSize);

    void setCalibrateBoardImage(cv::Mat img);

signals:
    void send_found_circle(bool flag);

protected:
    void run();


private:
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints;

    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;

    int flag;

    bool mustInitUndistort;

    cv::Size boardSize;

    cv::Mat calibrateBoardImg;
};

#endif // CALIBRATE_H
