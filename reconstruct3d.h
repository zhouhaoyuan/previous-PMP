#ifndef RECONSTRUCT3D_H
#define RECONSTRUCT3D_H

#include <QObject>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QThread>
#include <QDebug>
#include <QSplitter>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QPushButton>
#include <QGridLayout>
#include <QDebug>
#include <QFileDialog>
#include <QTime>
#include <QMetaType>
#include <math.h>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include "calibratemachine.h"
#include <iostream>
#include <fstream>
#include <QProcess>



class Point_3D
{
public:
    typedef double PointType;
    Point_3D();
//    Point_3D(const PointType &x_in, const PointType &y_in, const PointType &z_in);

    PointType x;
    PointType y;
    PointType z;
};

class Reconstruct3D : public QThread
{
    Q_OBJECT
public:
    explicit Reconstruct3D();

    void cvShowMat(cv::Mat img, std::string name);

    void cvShowMatFloat(cv::Mat img, std::string name,int period);

    void saveMask(std::vector<cv::Mat> m_list);

    void computeMaskImage(std::vector<cv::Mat> m_list, Mat &mask_mat);

    void loadImages(std::vector<cv::Mat> mats,cv::Mat mask, QString c_path, QString out_path);

    void decodeHierarchyPatterns(std::vector<cv::Mat> in_mat,std::vector<cv::Mat> &x_unwrap_list,std::vector<cv::Mat> &y_unwrap_list);

    void generatePointCloud(std::vector<cv::Mat> in_mat);

    void generateHieraryPointCloud(std::vector<cv::Mat> in_mat);

    void generateMultiplyWavelengthPointCloud(std::vector<cv::Mat> in_mat);

    void decodeThreeHoterodyUnwrap(std::vector<cv::Mat> in_mat,cv::Mat &unwrap_x,cv::Mat &unwrap_y,cv::Mat mask);

    void createCorrectPoints(cv::Mat phaseX, cv::Mat phaseY,std::vector<cv::Point2f> &l_points,std::vector<cv::Point2f> &r_points);

    void createCorrectPoints(cv::Mat phaseX, cv::Mat phaseY, std::vector<cv::Point2f> &l_points, std::vector<cv::Point2f> &r_points,cv::Mat mask);

    void phaseToPixelFloat(cv::Mat phaseX, cv::Mat phaseY, int period, QString name);

    void restruct3D(std::vector<cv::Point2f> leftPoints, std::vector<cv::Point2f> rightPoints, std::vector<cv::Point3f> &pCloud);

    void readCalibPara();

    void savePointCloud(std::vector<Point3f> pCloud, QString path);

    inline void GeneratePoint(const cv::Mat &K1, const cv::Mat &kc1, const cv::Mat &K2, const cv::Mat &kc2,
                                              const cv::Mat &Rt, const cv::Mat &T, const unsigned p1_x, const unsigned p1_y,
                                              const float p2_x, const float p2_y, Point3f *point, double *distance);

    inline void ApproximateRayIntersection(const cv::Point3d &v1, const cv::Point3d &q1, const cv::Point3d &v2, const cv::Point3d &q2,
                                                           Point3f *point, double *distance, double *out_lambda1, double *out_lambda2);


    void IntersectionRestruct3D(std::vector<cv::Point2f> leftPoints, std::vector<cv::Point2f> rightPoints, std::vector<Point3f> &pCloud);

signals:

public slots:

private:
    cv::Mat M_1;
    cv::Mat M_2;

    cv::Mat cameraA,dlpA;
    cv::Mat cameraM,dlpM;
    cv::Mat cameraKc,dlpKc;

    cv::Mat R_Mat,T_Mat;

    std::vector<cv::Mat> ps_mat_list;

    cv::Mat in_unwrap_x;
    cv::Mat in_unwrap_y;

    QString calib_path;
    QString output_path;

    cv::Mat m_mask;

};

#endif // RECONSTRUCT3D_H
