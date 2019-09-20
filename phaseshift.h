#ifndef PHASESHIFT_H
#define PHASESHIFT_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTime>
#include <QDebug>

using namespace cv;

double const PI= 3.1415926535;

class PhaseShift
{
public:
    PhaseShift();


    void fourStepPhaseShift(std::vector<cv::Mat> inMats,cv::Mat& outMat);

    void threeStepPhaseShift(std::vector<cv::Mat> inMats,cv::Mat& outMat);

    void unwarpHierarchical(float rate, const Mat l_wrap, cv::Mat h_wrap, cv::Mat &h_unwrap, cv::Mat &k_Mat);

    void unwarpDualWavelength(cv::Mat l_unwrap, cv::Mat h_wrap, cv::Mat &h_unwrap, cv::Mat &k_Mat);

    void unwrapMultiplyWavelength(std::vector<cv::Mat> wrap_list, cv::Mat& unwrap_phase,
                                  std::vector<cv::Mat> &unwrap_phase_list);

    void unwarpMultiplyHierarchical(float rate, std::vector<cv::Mat> wrap_list, cv::Mat& unwrap_phase);

    void unwarpMultiplyHierarchical(float rate, std::vector<cv::Mat> wrap_list, cv::Mat& unwrap_phase,
                                    std::vector<cv::Mat> &unwrap_phase_list);

    void unwrap_MP_MW(std::vector<cv::Mat> wrap_list, cv::Mat& unwrap_phase);

    void phaseNormalization(float rate,cv::Mat unwrap_phase,cv::Mat &norm_phase);

    void unwrapThreeFrequencyHeterodyne(std::vector<cv::Mat> wrap_list,cv::Mat& unwrap_phase);

    /*****************************************************************************************************/

    void multipleWavelengthUnwrapFloat(std::vector<cv::Mat> setImage,cv::Mat& resultMat);


    void setThreeFrequenceWavelength(float p1,float p2,float p3);


private:
    float wavelength_P1;
    float wavelength_P2;
    float wavelength_P3;


};

#endif // PHASESHIFT_H
