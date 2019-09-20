#ifndef YUAN_PATTERN_H
#define YUAN_PATTERN_H

#include <QObject>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

#include "hash_table.h"

enum  Direction{Vertical,Horizontal};
enum  Color{Red,Green,Blue,White,Black,Magenta,Cyan,Yellow,halfRed,halfGreen,halfBlue,halfWhite,halfMagenta,halfCyan,halfYellow};
enum  kernel{Circle,Gauss, Butterworth};
double const myPI = 3.1415926535;


struct pointXY{
    double x;
    double y;
};
struct channels{
    uchar blue;
    uchar green;
    uchar red;
};

#include <vector>
#include <iostream>

class Yuan_pattern : public QObject
{
    Q_OBJECT
public:
    explicit Yuan_pattern(QObject *parent = 0);
    ~Yuan_pattern();
signals:

public slots:

private:

    unsigned int columns=1280;
    unsigned int rows=800;

    float wavelength_P1=40;
    float wavelength_P2=46;
    float wavelength_P3=350;

    hash_table phaseError_table;
    hash_table table_p2;
    hash_table table_p3;

    cv::Mat _R,_T;
    cv::Mat left_intrinsic,right_intrinsic;
    cv::Mat left_distCoeffs,right_distCoeffs;

    cv::Mat E_1,E_2,M_1,M_2;

    double** atan2Table_threeStep;
    double** atan2Table_fourStep;

    cv::Mat tempUnwrap1,tempUnwrap2;

public:
    std::vector<cv::Mat> image_paint;
    std::vector<std::vector<double>> value_paint;
    std::vector<pointXY> point_paint;

    double* gammaTable;

public:
    void parameter_setting(int row,int col){
        this->rows=row;
        this->columns=col;
        std::cout<<"parameter_setting  rows : "<<rows<<std::endl;
        std::cout<<"parameter_setting  columns : "<<columns<<std::endl;
    }

    void parameter_threeFrequency_set(float p1,float p2,float p3){
        this->wavelength_P1=p1;
        this->wavelength_P1=p2;
        this->wavelength_P1=p3;
    }

    void reconstruction_parameters_set();

    //灰度图的正弦条纹
    void generate_gray_sindata(Direction direction,int stepNum,int piexls_per_period,std::vector<cv::Mat> &patternSequence);
    //三频正弦
    void generate_gray_three_Freq_fout_shift_sin_pattern(std::vector<cv::Mat> &patternSequence,int P1=52,int P2=58,int P3=620);
    //彩色三频正弦
    void generate_color_three_sinusoidal_pattern(std::vector<cv::Mat> &colorPattern,int stepNum,Direction direction);

    void generate_color_three_step_sin_pattern(std::vector<cv::Mat> &colorPattern, int pixelperperiod_1, int pixelperperiod_2,int stepNum,Direction direction,double phaseShift);

    void generate_color_sin_phaseCode_phaseshift_pattern(std::vector<cv::Mat> &colorPattern,int pixelperperiod,Direction direction);

    void generate_color_composite_code_pattern(std::vector<cv::Mat> &colorPattern, int pixelperperiod_1,int pixelperperiod_2,int stepNum, Direction direction);

    void generate_pure_color_pattern(cv::Mat &colorPattern,Color color_selection );

    void generate_pure_sinusoidal_pattern(std::vector<cv::Mat> &colorPattern,int pixels_per_period,int stepNum,Color color_selection ,Direction direction,double gamma=1);

    void generate_pure_phaseCode_pattern(std::vector<cv::Mat> &colorPattern,int pixels_per_period,int stepNum,Direction direction,double gamma=1);        

    void generate_phaseCode_sin_pattern(std::vector<cv::Mat> &srcImage,int pixels_per_period,Direction direction,int stepNum,double gamma=1);

    void generate_pure_phaseCode2016_pattern(std::vector<cv::Mat> &colorPattern, int pixels_per_period, int stepNum, Direction direction);

    void generate_phaseCode_grayCode_sin_pattern(std::vector<cv::Mat> &srcImage,int pixels_per_period,Direction direction,int stepNum,double gamma=1);

    void generate_binary_grayCode_sin_pattern(std::vector<cv::Mat> &srcImage,int pixels_per_period,Direction direction,int stepNum,double gamma=1);

    void generate_composed_sin_pattern(std::vector<cv::Mat> &srcImage,int pixels_per_period,int stepNum,double gamma=1);

    void generate_gradual_change_gray_pattern(std::vector<cv::Mat> &srcImage,Color color_select);

    void generate_gamma_pure_sinusoidal_pattern(std::vector<cv::Mat> &colorPattern,int pixels_per_period,int stepNum,Color color_selection ,Direction direction,double &gammaA,std::vector<double> &gammaB);

    void generate_gamma_phaseCode_grayCode_sin_pattern(std::vector<cv::Mat> &srcImage,int pixels_per_period,Direction direction,int stepNum,
                                                       double &blueGammaA,std::vector<double> &blueGammaB);

    void transform_to_binary_pattern(cv::Mat &srcImage,Direction _direction);

    /**********************************************/

    void decode_color_sin_phaseCode_phaseshift_pattern(std::vector<cv::Mat> &srcImage,cv::Mat &dstImage,cv::Mat &mask,Direction _direction);

    void decode_phaseCode_sin_pattern(std::vector<cv::Mat> &srcImage,cv::Mat &dstImage,cv::Mat &mask,Direction _direction);

    void decode_pure_phaseCode2016_pattern(std::vector<cv::Mat> &srcImage, cv::Mat &dstImage, cv::Mat &mask, Direction _direction);

    void decode_phaseCode_grayCode_sin_pattern(std::vector<cv::Mat> &srcImage,cv::Mat &dstImage,cv::Mat &mask,Direction _direction);

    void decode_grayCode_sin_pattern(std::vector<cv::Mat> &srcImage,cv::Mat &dstImage,cv::Mat &mask,Direction _direction);

    void decode_sinusoidal(cv::Mat mask,std::vector<cv::Mat> &srcImageSequence,cv::Mat &dstImage,int stepNum);

    void decode_sinusoidal_LookUpTable(cv::Mat &mask,std::vector<cv::Mat> &srcImageSequence,cv::Mat &dstImage,int stepNum);

    void decode_threefrequencyHeterodyne(cv::Mat mask,std::vector<cv::Mat> &srcImageSequence,int stepNum,std::vector<cv::Mat> &dstImageSequence,cv::Mat &unwrap_x,cv::Mat &unwrap_y);

    void decode_threefrequency(cv::Mat mask,std::vector<cv::Mat> &srcImageSequence,cv::Mat &unwrap,int stepNum=4);

    void decode_doublefrequencyHeterodyne(cv::Mat &mask,std::vector<cv::Mat> &srcImageSequence, int T1, int T2, cv::Mat &unwrap,Direction _direction);

    bool wrap_fourier_transform(const cv::Mat &src,cv::Mat &dst,kernel _kernel,int center_x,int radius);

    void decode_multi_step_phaseShift(std::vector<cv::Mat> &src,cv::Mat &dst,int stepNum,int phaseShitf,const cv::Mat &mask);

    void decode_grayCode(std::vector<cv::Mat> &src,cv::Mat &dst,const cv::Mat &mask);

    void decode_multi_grayCode(std::vector<cv::Mat> &src,cv::Mat &dst,const cv::Mat &mask);

    /******************************Gamma************************/

    double gamma_active_calibration(std::vector<cv::Mat> &phaseSequence,std::vector<double> &diff,int rowNum);

    double* quadratic_equation_of_one_variable(double a,double b,double c);

    void correct_phase(cv::Mat &src,cv::Mat &dst,const cv::Mat &mask,int Num);

    bool multipleLinearRegression(const std::vector<double> &x,const std::vector<double> &y,int numRegression,double &A,std::vector<double> &B);

    double fitting_polynomial(const double &A,const std::vector<double> &B,const double &x);

    void difference_phase(const cv::Mat &original,const cv::Mat &real,int rowNum,std::vector<double> &diff,const cv::Mat &mask);

    void difference_phase_pointXY(cv::Mat &original,cv::Mat &real,int rowNum,std::vector<pointXY> &diff,cv::Mat &mask);

    void meanValue_gradual_gray(const std::vector<cv::Mat> &srcImages,std::vector<double> &result);

    void build_gamma_Table(const double &gammaA,const std::vector<double> &gammaB,int numRegression);

    //**********************create the look up table******************/

    hash_table create_LookUpTable(std::vector<cv::Mat> &phase,cv::Mat &mask);

    bool hash_table_set(hash_table table,int Num);

    hash_table get_LookUpTable(int Num);


    /******************************crosstalk********************/

    void meanValue(cv::Mat mask, cv::Mat &pattern,double &R_mean,double &G_mean,double &B_mean);

    void color_correction(cv::Mat mask, cv::Mat &srcImage,cv::Mat &dst,const cv::Mat &crosstalk,const cv::Mat &reflect);

    void HSI_color_space_transform(cv::Mat &srcImage,cv::Mat &dstImage);

    bool build_crosstalk_look_up_table(channels*** crossResult,const cv::Mat &crosstalk,const cv::Mat &reflect);

    bool correct_crosstalk_by_LookUpTable(channels*** crossTable, cv::Mat &srcImage,cv::Mat &dstImage,const cv::Mat &mask);

    /*******************3D reconstruct************************/

    void createCorrectPoints(cv::Mat unwrapX,cv::Mat unwrapY,int pixels,std::vector<cv::Point2f> &l_points, std::vector<cv::Point2f> &r_points,cv::Mat mask);

    void reconstruct3D(std::vector<cv::Point2f> leftPoints, std::vector<cv::Point2f> rightPoints, std::vector<cv::Point3f> &pCloud);

    void savePointCloud(std::vector<cv::Point3f> pCloud,QString path);

    void matrix_calculate(double* a,double *b,double *c,int cols);

};

inline bool Yuan_pattern::hash_table_set(hash_table table,int Num)
{
    if(table.isNull())
    {
        std::cout<<"Error: the hash_table is empty ... \n";
        return false;
    }
    if(Num==1)phaseError_table=table;
    else if(Num==2)table_p2=table;
    else table_p3=table;
    return true;
}
inline hash_table Yuan_pattern::get_LookUpTable(int Num)
{
    if(Num==1)return phaseError_table;
    if(Num==2)return table_p2;
    if(Num==3)return table_p3;
}
inline void Yuan_pattern::reconstruction_parameters_set()
{
    cv::FileStorage fs;
    fs.open("../ScanData/calibResult.yml",cv::FileStorage::READ);
    if(!fs.isOpened())
    {
        std::cout<<"Error: the yml file can not be opened!......"<<std::endl;
        return;
    }
    fs["R"]>>_R;
    fs["T"]>>_T;
    fs["cameraM"]>>left_intrinsic;
    fs["proM"]>>right_intrinsic;
    fs["cameraKc"]>>left_distCoeffs;
    fs["proKc"]>>right_distCoeffs;
    fs.release();
}

#endif // YUAN_PATTERN_H
