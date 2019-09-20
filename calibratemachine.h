#ifndef CALIBRATEMACHINE_H
#define CALIBRATEMACHINE_H

#include <QObject>
#include <QImage>
#include <QThread>
#include <QDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "iostream"
#include <QTime>
#include <iostream>
#include <fstream>
#include <QProcess>
#include "phaseshift.h"
#include "yuan_pattern.h"



using namespace cv;

class CalibrateMachine: public QThread
{
    Q_OBJECT
public:
    CalibrateMachine();
    ~CalibrateMachine();


    int addCirclebordPoints(QStringList filelist, cv::Size &boardSize);


    bool findCircleBoardCorners(cv::Mat image,cv::Size boardSize, std::vector<cv::Point2f> &realCorners);

    void addPoints(const std::vector<cv::Point2f> &cameraCorners,const std::vector<cv::Point2f> &dlpCorners,const std::vector<cv::Point3f>& objectCorners);


    void getValidScope(cv::Mat imgIn, cv::Size boardSize, Mat &imgOut);

    //开始标定返回重投影误差
    double CalibrateCamera(cv::Size &imageSize);

    void setCalibrateBoardImage(cv::Mat img);

    void pointsCameraToDlp(cv::Mat phaseX, cv::Mat phaseY, int period, std::vector<cv::Point2f> cameraPoints, std::vector<Point2f> &dlpPoints);

    void saveCalibrateResult(double cameraError,cv::Mat cameraM,cv::Mat cameraKc,double proError,cv::Mat proM,cv::Mat proKc,
                             double stereoError,cv::Mat R,cv::Mat T);

    void decodeFourStepMultiplyHierarchical( std::vector<cv::Mat> mat_list);

    double structuredLightStereoCalibrate();

    void printMat(cv::Mat mat);

    bool decodeThreeFrequencyHeterodyne( cv::Mat mask,std::vector<cv::Mat> &mat_list,std::vector<cv::Mat> &result,Yuan_pattern *yuanClass);
    //重投影评价
    void CalibrationEvaluate(string name,std::vector< std::vector<cv::Point2f> > &points_Sequence,std::vector<cv::Mat> &rvecs,std::vector<cv::Mat> &tvecs,cv::Mat &intrinsic,cv::Mat distortion_coeff);

signals:
    void send_found_circle(bool flag);

protected:
    void run();


public:
    //双目标定
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> cameraPoints;
    std::vector<std::vector<cv::Point2f>> dlpPoints;

    cv::Size boardSize;

private:
    cv::Mat cameraA,dlpA;
    cv::Mat cameraM,dlpM;
    cv::Mat cameraKc,dlpKc;
    cv::Mat M_1;
    cv::Mat M_2;

    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;

//    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints;

//    cv::Mat cameraMatrix;
//    cv::Mat distCoeffs;

    int flag;

    bool mustInitUndistort;



    cv::Mat calibrateBoardImg;
};

#endif // CALIBRATEMACHINE_H
