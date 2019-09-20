#include "yuan_pattern.h"
#include <fstream>
#include "math.h"
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QTime>
#include <omp.h>

#include "MultipleLinearRegression.h"

Yuan_pattern::Yuan_pattern(QObject *parent) : QObject(parent)
{
    //读参
    reconstruction_parameters_set();

    E_1= cv::Mat::eye(3,4,CV_64FC1);
    E_2= cv::Mat::zeros(3,4,CV_64FC1);

    M_1= cv::Mat::zeros(3,4,CV_64FC1);
    M_2= cv::Mat::zeros(3,4,CV_64FC1);

    for(int i=0;i< _R.rows;i++)
    {
        for(int j= 0;j<_R.cols;j++)
        {
            E_2.at<double>(i,j)= _R.at<double>(i,j);
        }
    }

    for(int i= 0;i<3;i++)
    {
        E_2.at<double>(i,3)= _T.at<double>(i);
    }

    M_1= left_intrinsic*E_1;
    M_2= right_intrinsic*E_2;


    //_threeStep
    atan2Table_threeStep = new double*[511];

    for(unsigned int i = 0; i < 511; ++i){
        atan2Table_threeStep[i] = new double[1021];
    }
    for(int i = 0; i < 511; ++i){
        for(int j = 0; j < 1021; ++j){

           atan2Table_threeStep[i][j] = atan2((double)(std::sqrt(3)*( i - 255 )),
                                                       (double)( j - 510 ))+CV_PI;
        }
    }

    std::cout<<"atan2Table_threeStep initialized successfully ... \n";

    //_fourStep

    atan2Table_fourStep = new double*[511];

    for(unsigned int i = 0; i < 511; ++i){
        atan2Table_fourStep[i] = new double[511];
    }
    for(int i = 0; i < 511; ++i){
        for(int j = 0; j < 511; ++j){

           atan2Table_fourStep[i][j] = atan2( i-255, j-255)+CV_PI;
        }
    }
    std::cout<<"atan2Table_fourStep initialized successfully ... \n";

}
Yuan_pattern::~Yuan_pattern()
{
    for(unsigned int i = 0; i < 511; ++i){
        delete [] atan2Table_threeStep[i];
    }
    delete [] atan2Table_threeStep;

    for(unsigned int i = 0; i < 511; ++i){
        delete [] atan2Table_fourStep[i];
    }
    delete [] atan2Table_fourStep;

}


void Yuan_pattern::generate_gray_sindata(Direction direction,int stepNum,int piexls_per_period,std::vector<cv::Mat> &patternSequence)
{
    unsigned int rowNumber=this->rows;
    unsigned int colNumber=this->columns;
    unsigned int Pixels_per_period=piexls_per_period;
    switch(direction)
    {
        case Vertical:
        {
            for(int n=0;n<stepNum;n++)
            {
                cv::Mat sinusoidal_pattern(rowNumber, colNumber, CV_8UC1);
                for(unsigned int i=0;i < rowNumber;i++)
                {
                    for(unsigned int j=0;j < colNumber;j++)
                    {
                        sinusoidal_pattern.at<uchar>(i,j)=128+127*cos(2*CV_PI*(j+1)/Pixels_per_period+(n)*2*CV_PI/stepNum);
                    }
                }
                patternSequence.push_back(sinusoidal_pattern);
            }

        }
            break;
        case Horizontal:
        {
            for(int n=0;n<4;n++)
            {
               cv::Mat sinusoidal_pattern(rowNumber, colNumber, CV_8UC1);
               for(unsigned int i=0;i<rowNumber;i++)
               {
                   for(unsigned int j=0;j<colNumber;j++)
                   {
                       sinusoidal_pattern.at<uchar>(i,j)=128+127*cos((i+1)*2*CV_PI/Pixels_per_period+(n)*2*CV_PI/stepNum);
                   }
               }
               patternSequence.push_back(sinusoidal_pattern);
            }

        }
            break;
    }

}

void Yuan_pattern::generate_gray_three_Freq_fout_shift_sin_pattern(std::vector<cv::Mat> &patternSequence,int P1,int P2,int P3)
{
    unsigned P_1=P1;
    unsigned P_2=P2;
    unsigned P_3=P3;

    generate_gray_sindata(Vertical,4,P_1,patternSequence);
    generate_gray_sindata(Vertical,4,P_2,patternSequence);
    generate_gray_sindata(Vertical,4,P_3,patternSequence);

    generate_gray_sindata(Horizontal,4,P_1,patternSequence);
    generate_gray_sindata(Horizontal,4,P_2,patternSequence);
    generate_gray_sindata(Horizontal,4,P_3,patternSequence);

}

void Yuan_pattern::generate_color_three_sinusoidal_pattern(std::vector<cv::Mat> &colorPattern,int stepNum,Direction direction)
{
   QTime t1;
   t1.start();

   int P1 = wavelength_P1;
   int P2 = wavelength_P2;
   int P3 = wavelength_P3;

   double CV2PI = CV_PI*2;
   double CV2PI_f1 = CV2PI/P1;
   double CV2PI_f2 = CV2PI/P2;
   double CV2PI_f3 = CV2PI/P3;

    switch (direction)
    {
        case Vertical:
        {
                for(int n=0;n<stepNum;n++)
                {
                    int m= 0;
                    if(stepNum==4)
                    {
                        m=n-2;
                    }else{
                        m=n;
                    }                 

                    cv::Mat pattern(rows,columns,CV_8UC3);

                    uchar *ptr = pattern.ptr<uchar>(0);

                    for(unsigned int j=0;j < columns*3; j+=3)
                    {
                        ptr[j]  = 128+127*cos(CV2PI_f1*(j/3)+m*CV2PI/stepNum);
                        ptr[j+1]= 128+127*cos(CV2PI_f2*(j/3)+m*CV2PI/stepNum);
                        ptr[j+2]= 128+127*cos(CV2PI_f3*(j/3)+m*CV2PI/stepNum);
                    }
                    for(unsigned int i = 1; i < rows; ++i){

                        pattern.row(0).copyTo(pattern.row(i));
                    }

                    colorPattern.push_back(pattern);
                }
        }
        break;
    case Horizontal:
            {
                for(int n=0;n<stepNum;n++)
                {
                    int m= 0;
                    if(stepNum==4)
                    {
                        m=n-2;
                    }else{
                        m=n;
                    }

                    cv::Mat pattern(rows,columns,CV_8UC3);

                    for(unsigned int i = 0; i < rows; ++i)
                    {
                        uchar *ptr = pattern.ptr<uchar>(i);

                        ptr[0] = 128+127*cos(CV2PI_f1*(i)+m*CV2PI/stepNum);
                        ptr[1] = 128+127*cos(CV2PI_f2*(i)+m*CV2PI/stepNum);
                        ptr[2] = 128+127*cos(CV2PI_f3*(i)+m*CV2PI/stepNum);
                    }
                    for(unsigned int j = 1; j < columns; ++j){

                        pattern.colRange(0,1).copyTo(pattern.colRange(j,j+1));
                    }

                    colorPattern.push_back(pattern);
                }
            }
         break;
    }
    std::cout<<"generate_color_three_sinusoidal_pattern consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::generate_color_three_step_sin_pattern(std::vector<cv::Mat> &colorPattern,int pixelperperiod_1,int pixelperperiod_2, int stepNum, Direction direction,double phaseShift)
{
    QTime t1;
    t1.start();

    unsigned int n,i,j;
    if(direction==Vertical)
    {
        double my2PIperStep = 2*CV_PI/stepNum;
        double my4PIperStep = 4*CV_PI/stepNum;
        double my2PI_f1 = 2*CV_PI/pixelperperiod_1;
        double my2PI_f2 = 2*CV_PI/pixelperperiod_2;
        double my2PI = 2*CV_PI;

        double phaseCode = 0;
        unsigned int ladderNum = 15;

        for( n = 0; n < stepNum ; n++){

            cv::Mat pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            uchar* patternPtr = pattern.ptr<uchar>(0);
            double n_my2PIperStep = n*my2PIperStep;
            double n_my4PIperStep = n*my4PIperStep;
            for( j = 0; j < columns; ++j){

//                phaseCode = my2PI*(floor((double)(j-1)/pixelperperiod)/(ladderNum));
//                patternPtr[j*3] = 128 + 64*cos( my2PI_f*j + 3*CV_PI/6 + n_my2PIperStep ) + 63*cos( phaseCode + CV_PI/15 + CV_PI/6 + n_my4PIperStep ) ;
//                patternPtr[j*3+2] = 128 + 64*cos( my2PI_f*j + 3*CV_PI/6 + n_my2PIperStep + CV_PIperStep ) + 63*cos( phaseCode + CV_PI/15 + CV_PI/6 + n_my4PIperStep + CV_PIperStep );

                patternPtr[j*3] = 128 + 127*cos( my2PI_f1*j  + n_my4PIperStep + phaseShift) ;
                patternPtr[j*3+2] = 128 + 127*cos( my2PI_f2*j  + n_my4PIperStep + phaseShift );
            }
            for( i = 1; i < rows; ++i){
                pattern.row(0).copyTo( pattern.row(i) );
            }

//            std::vector<cv::Mat> channels;
//            cv::split(pattern,channels);
//            transform_to_binary_pattern(channels[0],Vertical);
//            //transform_to_binary_pattern(channels[1],Vertical);
//            transform_to_binary_pattern(channels[2],Vertical);
//            cv::merge(channels,pattern);

           cv::imwrite("../ScanData/compositeV" + QString::number(n).toStdString() + ".bmp",pattern);
            colorPattern.push_back(pattern);
        }
    }else{

        double my2PIperStep = 2*CV_PI/stepNum;
        double my4PIperStep = 4*CV_PI/stepNum;
        double my2PI_f1 = 2*CV_PI/pixelperperiod_1;
        double my2PI_f2 = 2*CV_PI/pixelperperiod_2;
        double my2PI = 2*CV_PI;

        double phaseCode = 0;
        unsigned int ladderNum = 10;

        for( n = 0; n < stepNum ; n++){

            cv::Mat pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            double n_my2PIperStep = n*my2PIperStep;
            double n_my4PIperStep = n*my4PIperStep;

            for( i = 0; i < rows; ++i){

                uchar* patternPtr = pattern.ptr<uchar>(i);
//                phaseCode = my2PI*(floor((double)(i-1)/pixelperperiod)/(ladderNum));
//                patternPtr[0] = 128 + 64*cos( my2PI_f*i + 3*CV_PI/5 + n_my2PIperStep ) + 63*cos( phaseCode + CV_PI/10 + CV_PI/6 + n_my4PIperStep ) ;
//                patternPtr[2] = 128 + 64*cos( my2PI_f*i + 3*CV_PI/5 + n_my2PIperStep + CV_PIperStep ) + 63*cos( phaseCode + CV_PI/10 + CV_PI/6 + n_my4PIperStep + CV_PIperStep ) ;

                patternPtr[0] = 128 + 127*cos( my2PI_f1*i + n_my4PIperStep +phaseShift);
                patternPtr[2] = 128 + 127*cos( my2PI_f2*i + n_my4PIperStep +phaseShift );
            }
            for( j = 1; j < columns; ++j){
                pattern.col(0).copyTo( pattern.col(j) );
            }

//            std::vector<cv::Mat> channels;
//            cv::split(pattern,channels);
//            transform_to_binary_pattern(channels[0],Vertical);
//            //transform_to_binary_pattern(channels[1],Vertical);
//            transform_to_binary_pattern(channels[2],Vertical);
//            cv::merge(channels,pattern);

            cv::imwrite("../ScanData/compositeH" + QString::number(n).toStdString() + ".bmp",pattern);
            colorPattern.push_back(pattern);
        }
    }
    std::cout<<"the generate_color_composite_code_pattern consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::generate_color_sin_phaseCode_phaseshift_pattern(std::vector<cv::Mat> &colorPattern,int pixelperperiod, Direction direction)
{
    QTime t1;
    t1.start();
    switch (direction)
    {
        case Vertical:
        {
            cv::Mat sin_pattern(rows,columns,CV_8UC3);
            uchar *sinptr=sin_pattern.ptr<uchar>(0);
            for(unsigned int j=0;j<columns;++j)
            {
                sinptr[j*3]  = 128+127*cos(2*CV_PI*(j)/pixelperperiod+(-2)*2*CV_PI/3 -CV_PI/3 );
                sinptr[j*3+1]= 128+127*cos(2*CV_PI*(j)/pixelperperiod+(-1)*2*CV_PI/3 -CV_PI/3 );
                sinptr[j*3+2]= 128+127*cos(2*CV_PI*(j)/pixelperperiod+(0)*2*CV_PI/3 -CV_PI/3 );
            }
            for(unsigned int i=1;i<rows;++i)
            {
                sin_pattern.row(0).copyTo(sin_pattern.row(i));
            }
            colorPattern.push_back(sin_pattern);

            cv::Mat phaseCode_pattern(rows,columns,CV_8UC3);
            uchar *phaseptr=phaseCode_pattern.ptr<uchar>(0);
            for(unsigned int j=0;j<columns;++j)
            {
                unsigned int ladderNum = 32;
                double phaseCode =  2*CV_PI*(floor((double)(j-1)/pixelperperiod)/(ladderNum));
                phaseptr[j*3]  = 128+127*cos(phaseCode-(-2)*2*CV_PI/3 +2*CV_PI/4-CV_PI/8);
                phaseptr[j*3+1]= 128+127*cos(phaseCode-(-1)*2*CV_PI/3 +2*CV_PI/4-CV_PI/8 );
                phaseptr[j*3+2]= 128+127*cos(phaseCode-(0)*2*CV_PI/3 +2*CV_PI/4-CV_PI/8 );
            }
            for(unsigned int i=1;i<rows;++i)
            {
               phaseCode_pattern.row(0).copyTo(phaseCode_pattern.row(i));
            }
            colorPattern.push_back(phaseCode_pattern);

        }
        break;
    case Horizontal:
            {
                cv::Mat sin_pattern(rows,columns,CV_8UC3);
                for(unsigned int i=0;i<rows;++i)
                {
                    uchar *ptr=sin_pattern.ptr<uchar>(i);

                    ptr[0]  = 128+127*cos(2*CV_PI*(i)/pixelperperiod+(-2)*2*CV_PI/3 -CV_PI/3);
                    ptr[1] = 128+127*cos(2*CV_PI*(i)/pixelperperiod+(-1)*2*CV_PI/3 -CV_PI/3);
                    ptr[2] = 128+127*cos(2*CV_PI*(i)/pixelperperiod+(0)*2*CV_PI/3 -CV_PI/3);

                }
                for(unsigned int j=1;j<columns;++j)
                {
                    sin_pattern.col(0).copyTo(sin_pattern.col(j));
                }
                colorPattern.push_back(sin_pattern);

                cv::Mat phaseCode_pattern(rows,columns,CV_8UC3);
                for(unsigned int i=0;i<rows;++i)
                {
                    uchar *ptr=phaseCode_pattern.ptr<uchar>(i);
                    unsigned int ladderNum = 20;
                    double phaseCode =  2*CV_PI*(floor((double)(i-1)/pixelperperiod)/(ladderNum));
                    ptr[0]  = 128+127*cos(phaseCode-(-2)*2*CV_PI/3 +2*CV_PI/4-CV_PI/8);
                    ptr[1]= 128+127*cos(phaseCode-(-1)*2*CV_PI/3 +2*CV_PI/4-CV_PI/8 );
                    ptr[2]= 128+127*cos(phaseCode-(0)*2*CV_PI/3 +2*CV_PI/4-CV_PI/8 );

                }
                for(unsigned int j=1;j<columns;++j)
                {
                    phaseCode_pattern.col(0).copyTo(phaseCode_pattern.col(j));
                }
                colorPattern.push_back(phaseCode_pattern);
            }
         break;
    }
    std::cout<<"generate_color_sin_phaseCode_phaseshift_pattern consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::generate_color_composite_code_pattern(std::vector<cv::Mat> &colorPattern, int pixelperperiod_1, int pixelperperiod_2, int stepNum, Direction direction)
{
       QTime t1;
       t1.start();

       unsigned int n,i,j;

       double CV_PIperStep = CV_PI/stepNum;
       double my2PIperStep = 2*CV_PI/stepNum;
       double my4PIperStep = 4*CV_PI/stepNum;
       double my2PI_f1 = 2*CV_PI/pixelperperiod_1;
       double my2PI_f2 = 2*CV_PI/pixelperperiod_2;
       double my2PI = 2*CV_PI;
       double phaseCode = 0;

       if(direction==Vertical)
       {
           unsigned int ladderNum = 15;

           for( n = 1; n < stepNum+1 ; n++){

               cv::Mat pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
               uchar* patternPtr = pattern.ptr<uchar>(0);
               double n_my2PIperStep = n*my2PIperStep;
               double n_my4PIperStep = n*my4PIperStep;

               for( j = 0; j < columns; ++j){

   //                phaseCode = my2PI*(floor((double)(j-1)/pixelperperiod)/(ladderNum));
   //                patternPtr[j*3] = 128 + 64*cos( my2PI_f*j + 3*CV_PI/6 + n_my2PIperStep ) + 63*cos( phaseCode + CV_PI/15 + CV_PI/6 + n_my4PIperStep ) ;
   //                patternPtr[j*3+2] = 128 + 64*cos( my2PI_f*j + 3*CV_PI/6 + n_my2PIperStep + CV_PIperStep ) + 63*cos( phaseCode + CV_PI/15 + CV_PI/6 + n_my4PIperStep + CV_PIperStep );

                   patternPtr[j*3] = 128 + 64*cos( my2PI_f1*j + n_my2PIperStep ) + 63*cos( my2PI_f2*j -CV_PI  + n_my4PIperStep  ) ;
                   patternPtr[j*3+2] = 128 + 64*cos( my2PI_f1*j + n_my2PIperStep + CV_PIperStep ) + 63*cos( my2PI_f2*j -CV_PI + n_my4PIperStep - CV_PIperStep*3 ) ;
               }
               for( i = 1; i < rows; ++i){
                   pattern.row(0).copyTo( pattern.row(i) );
               }

               std::vector<cv::Mat> channels;
               cv::split(pattern,channels);
               transform_to_binary_pattern(channels[0],Vertical);
               //transform_to_binary_pattern(channels[1],Vertical);
               transform_to_binary_pattern(channels[2],Vertical);
               cv::merge(channels,pattern);

               cv::imwrite("../ScanData/compositeV" + QString::number(n).toStdString() + ".bmp",pattern);
               colorPattern.push_back(pattern);
           }
       }else{

           unsigned int ladderNum = 10;

           for( n = 1; n < stepNum+1 ; n++){

               cv::Mat pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
               double n_my2PIperStep = n*my2PIperStep;
               double n_my4PIperStep = n*my4PIperStep;

               for( i = 0; i < rows; ++i){

                   uchar* patternPtr = pattern.ptr<uchar>(i);

   //                phaseCode = my2PI*(floor((double)(i-1)/pixelperperiod)/(ladderNum));
   //                patternPtr[0] = 128 + 64*cos( my2PI_f*i + 3*CV_PI/5 + n_my2PIperStep ) + 63*cos( phaseCode + CV_PI/10 + CV_PI/6 + n_my4PIperStep ) ;
   //                patternPtr[2] = 128 + 64*cos( my2PI_f*i + 3*CV_PI/5 + n_my2PIperStep + CV_PIperStep ) + 63*cos( phaseCode + CV_PI/10 + CV_PI/6 + n_my4PIperStep + CV_PIperStep ) ;

                   patternPtr[0] = 128 + 64*cos( my2PI_f1*i + n_my2PIperStep ) + 63*cos( my2PI_f2*i -CV_PI + n_my4PIperStep ) ;
                   patternPtr[2] = 128 + 64*cos( my2PI_f1*i + n_my2PIperStep + CV_PIperStep ) + 63*cos( my2PI_f2*i -CV_PI + n_my4PIperStep - CV_PIperStep*3 ) ;
               }
               for( j = 1; j < columns; ++j){
                   pattern.col(0).copyTo( pattern.col(j) );
               }

               std::vector<cv::Mat> channels;
               cv::split(pattern,channels);
               transform_to_binary_pattern(channels[0],Vertical);
              // transform_to_binary_pattern(channels[1],Vertical);
               transform_to_binary_pattern(channels[2],Vertical);
               cv::merge(channels,pattern);

               cv::imwrite("../ScanData/compositeH" + QString::number(n).toStdString() + ".bmp",pattern);
               colorPattern.push_back(pattern);
           }
       }
       std::cout<<"the generate_color_composite_code_pattern consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::generate_pure_color_pattern(cv::Mat &colorPattern,Color color_selection )
{
    switch (color_selection)
    {
    case White:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(255,255,255));
        colorPattern=srcImage.clone();
    }
        break;
    case Black:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(0));
        colorPattern=srcImage.clone();
    }
        break;
    case Red:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(0,0,255));
        colorPattern=srcImage.clone();
    }
        break;
    case Green:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(0,255,0));
        colorPattern=srcImage.clone();
    }
        break;
    case Blue:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(255,0,0));
        colorPattern=srcImage.clone();
    }
        break;
    case Magenta:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(255,0,255));
        colorPattern=srcImage.clone();
    }
        break;
    case Cyan:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(255,255,0));
        colorPattern=srcImage.clone();
    }
        break;
    case Yellow:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(0,255,255));
        colorPattern=srcImage.clone();
    }
        break;
    case halfRed:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(0,0,128));
        colorPattern=srcImage.clone();
    }
        break;
    case halfGreen:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(0,128,0));
        colorPattern=srcImage.clone();
    }
        break;
    case halfBlue:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(128,0,0));
        colorPattern=srcImage.clone();
    }
        break;
    case halfMagenta:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(128,0,128));
        colorPattern=srcImage.clone();
    }
        break;
    case halfCyan:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(128,128,0));
        colorPattern=srcImage.clone();
    }
        break;
    case halfYellow:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(0,128,128));
        colorPattern=srcImage.clone();
    }
        break;
    case halfWhite:
    {
        cv::Mat srcImage=cv::Mat(rows,columns,CV_8UC3,cv::Scalar(128,128,128));
        colorPattern=srcImage.clone();
    }
        break;

        default:
            break;
    }


}

void Yuan_pattern::generate_pure_sinusoidal_pattern(std::vector<cv::Mat> &colorPattern,int pixels_per_period,int stepNum,Color color_selection,Direction direction ,double gamma)
{
    QTime t1;
    t1.start();

    double _gamma = 1/gamma;

    switch (color_selection)
    {
        case Red:
        {
            if(direction==Vertical)
            {
                int m= 0;
                for(int n=0;n<stepNum;n++)
                {
                    if(stepNum==4)
                    {
                        m=n-2;
                    }else{
                        m=n;
                    }
                    cv::Mat pattern(rows,columns,CV_8UC3);
                    for(unsigned int i=0;i<rows;i++)
                    {
                        uchar *ptr=pattern.ptr<uchar>(i);
                        for(unsigned int j=0;j<columns*3;j+=3)
                        {
                            ptr[j]=0;
                            ptr[j+1]=0;
                            ptr[j+2]=pow(128+127*cos(2*CV_PI*(j/3+1)/pixels_per_period+m*2*CV_PI/stepNum),_gamma);
                        }
                    }
                    colorPattern.push_back(pattern);
                }
            }else
            {
                for(int n=0;n<stepNum;n++)
                {
                    int m= 0;
                    if(stepNum==4)
                    {
                        m=n-2;
                    }else{
                        m=n;
                    }
                    cv::Mat pattern(rows,columns,CV_8UC3);
                    for(unsigned int i=0;i<rows;i++)
                    {
                        uchar *ptr=pattern.ptr<uchar>(i);
                        for(unsigned int j=0;j<columns*3;j+=3)
                        {
                            ptr[j]=0;
                            ptr[j+1]=0;
                            ptr[j+2]=pow(128+127*cos(2*CV_PI*(i+1)/pixels_per_period+m*2*CV_PI/stepNum),_gamma);
                        }
                    }
                    colorPattern.push_back(pattern);
                }
            }

        }
            break;
        case Green:
        {
            if(direction==Vertical)
            {
                for(int n=0;n<stepNum;n++)
                {
                    int m= 0;
                    if(stepNum==4)
                    {
                        m=n-2;
                    }else{
                        m=n;
                    }
                    cv::Mat pattern(rows,columns,CV_8UC3);
                    for(unsigned int i=0;i<rows;i++)
                    {
                        uchar *ptr=pattern.ptr<uchar>(i);
                        for(unsigned int j=0;j<columns*3;j+=3)
                        {
                            ptr[j]=0;
                            ptr[j+1]=pow(128+127*cos(2*CV_PI*(j/3+1)/pixels_per_period+m*2*CV_PI/stepNum),_gamma);
                            ptr[j+2]=0;
                        }
                    }
                    colorPattern.push_back(pattern);
                }
            }else
            {
                for(int n=0;n<stepNum;n++)
                {
                    int m= 0;
                    if(stepNum==4)
                    {
                        m=n-2;
                    }else{
                        m=n;
                    }
                     cv::Mat pattern(rows,columns,CV_8UC3);
                    for(unsigned int i=0;i<rows;i++)
                    {
                        uchar *ptr=pattern.ptr<uchar>(i);
                        for(unsigned int j=0;j<columns*3;j+=3)
                        {
                            ptr[j]=0;
                            ptr[j+1]=pow(128+127*cos(2*CV_PI*(i+1)/pixels_per_period+m*2*CV_PI/stepNum),_gamma);
                            ptr[j+2]=0;
                        }
                    }
                    colorPattern.push_back(pattern);
                }
            }
        }
            break;
        case Blue:
        {
            if(direction!=Horizontal)
            {
                for(int n=0;n<stepNum;n++)
                {
                    int m= n-2;
                    if(stepNum==4)
                    {
                        m=n-2;
                    }else if(stepNum==15){
                        m=n+5;   //对应4步为16步n-8，对应3步为15步n+5
                    }else
                    {
                        m=n;
                    }

                    cv::Mat pattern(rows,columns,CV_8UC3);

                    uchar *ptr=pattern.ptr<uchar>(0);

                    for(int j=0;j<columns;++j)
                    {
                        ptr[j*3]=128+127*cos(2*CV_PI*(j)/pixels_per_period+m*2*CV_PI/stepNum);//
                        //ptr[j*3]=pow(0.5+0.5*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/3 -CV_PI/3),_gamma);
                        ptr[j*3+1]=0;
                        ptr[j*3+2]=0;
                    }
                    for(int i = 1; i <rows; ++i){
                        pattern.row(0).copyTo(pattern.row(i));
                    }

                    colorPattern.push_back(pattern);
                }
            }else
            {
                for(int n=0;n<stepNum;n++)
                {
                    int m= n-2;
                    if(stepNum==4)
                    {
                        m=n-2;
                    }else if(stepNum==16){
                        m=n-8;//对应4步为16步n-8，对应3步为15步n+5
                    }else
                    {
                        m=n;
                    }

                    cv::Mat pattern(rows,columns,CV_8UC3);
                    for(int i=0;i<rows;++i)
                    {
                        uchar *ptr=pattern.ptr<uchar>(i);

                        ptr[0]=128+127*cos(2*CV_PI*(i)/pixels_per_period+m*2*CV_PI/stepNum);
                        //ptr[j*3]=pow(0.5+0.5*cos(2*CV_PI*(i)/pixels_per_period+(n-2)*2*CV_PI/3 -CV_PI/3),_gamma);
                        ptr[1]=0;
                        ptr[2]=0;

                    }
                    for(int j = 1; j < columns;++j){
                        pattern.col(0).copyTo(pattern.col(j));
                    }

                    colorPattern.push_back(pattern);
                }
            }
        }
            break;

            default:
                break;
    }


    std::cout<<"generate_pure_sinusoidal_pattern consumed "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::generate_pure_phaseCode_pattern(std::vector<cv::Mat> &colorPattern, int pixels_per_period, int stepNum, Direction direction, double gamma)
{
    QTime t1;
    t1.start();

    bool overOneStair = true;
    bool OneStair = false;

    std::vector<cv::Mat> sequencePattern;

    if(direction==Vertical)
    {
//            int m= n-2;
//            if(stepNum==4)
//            {
//                m=n-2;
//            }else if(stepNum==16){
//                m=n-10;
//            }else
//            {
//                m=n-5;
//            }

        for(int n = 0; n < stepNum; ++n){

            cv::Mat sin_pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            cv::Mat phase_pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            cv::Mat assit_pattern = cv::Mat::zeros(rows,columns,CV_8UC3);

            uchar *sinPtr   = sin_pattern.ptr<uchar>(0);
            uchar *phasePtr = phase_pattern.ptr<uchar>(0);
            uchar *assitPtr = assit_pattern.ptr<uchar>(0);

            for(int j=0;j<columns;++j)
            {
                ////  over one stair
 if(overOneStair){
                unsigned int ladderNum = 8;
                double phaseCode =  2*CV_PI*(floor((double)((j-1)%(ladderNum*pixels_per_period))/pixels_per_period))/(ladderNum);
                unsigned int subladderNum = 4;
                double phaseCode1 = 2*CV_PI*(floor((double)(j-1)/(ladderNum*pixels_per_period))/(subladderNum));

                sinPtr[j*3]   = 128+127*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/stepNum );
                phasePtr[j*3]   = 128+127*cos( phaseCode - (n-2)*2*CV_PI/stepNum );
                assitPtr[j*3]   = 128+127*cos( phaseCode1 - (n-2)*2*CV_PI/stepNum );
}

                ////  one stair
if(OneStair){
                unsigned int ladderNum = 32;
                double phaseCode =  2*CV_PI*(floor((double)(j-1)/pixels_per_period)/(ladderNum));

                sinPtr[j*3]   = 128+127*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/stepNum );
                phasePtr[j*3]   = 128+127*cos( phaseCode - (n-2)*2*CV_PI/stepNum );
}
            }
            for(int i = 1; i <rows;++i)
            {
                (sin_pattern.row(0)).copyTo(sin_pattern.row(i));
                (phase_pattern.row(0)).copyTo(phase_pattern.row(i));
                (assit_pattern.row(0)).copyTo(assit_pattern.row(i));
            }
            sequencePattern.push_back(sin_pattern);
            sequencePattern.push_back(phase_pattern);
            sequencePattern.push_back(assit_pattern);
        }
        int num = 0;
        if(overOneStair) num= 3;
        else  num= 2;

        for(int i = 0; i < num; i++ ){
            colorPattern.push_back( sequencePattern[i] );
            colorPattern.push_back( sequencePattern[i+3] );
            colorPattern.push_back( sequencePattern[i+6] );
            colorPattern.push_back( sequencePattern[i+9] );
        }

    }else{

        for(int n = 0; n < stepNum; ++n){

            cv::Mat sin_pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            cv::Mat phase_pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            cv::Mat assit_pattern = cv::Mat::zeros(rows,columns,CV_8UC3);

            for(int j=0;j<rows;++j)
            {
                ////  over one stair
if(overOneStair){
                unsigned int ladderNum = 5;
                double phaseCode =  2*CV_PI*(floor((double)((j-1)%(ladderNum*pixels_per_period))/pixels_per_period))/(ladderNum);
                unsigned int subladderNum = 4;
                double phaseCode1 = 2*CV_PI*(floor((double)(j-1)/(ladderNum*pixels_per_period))/(subladderNum));

               sin_pattern.at<uchar>(j,0) = 128+127*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/stepNum );
               phase_pattern.at<uchar>(j,0) = 128+127*cos(phaseCode-(n-2)*2*CV_PI/stepNum );
               assit_pattern.at<uchar>(j,0) = 128+127*cos(phaseCode1-(n-2)*2*CV_PI/stepNum );
}
                ////  one stair
if(OneStair){
                unsigned int ladderNum = 20;
                double phaseCode =  2*CV_PI*(floor((double)(j-1)/pixels_per_period)/(ladderNum));

                sin_pattern.rowRange(j,j+1)   = 128+127*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/stepNum );
                phase_pattern.rowRange(j,j+1)   = 128+127*cos( phaseCode - (n-2)*2*CV_PI/stepNum );
}
            }

            for(int i = 1; i <columns;++i)
            {
                sin_pattern.col(0).copyTo(sin_pattern.col(i));
                phase_pattern.col(0).copyTo(phase_pattern.col(i));
                assit_pattern.col(0).copyTo(assit_pattern.col(i));
            }
            sequencePattern.push_back(sin_pattern);
            sequencePattern.push_back(phase_pattern);
            sequencePattern.push_back(assit_pattern);
        }

        int num = 0;
        if(overOneStair) num= 3;
        else  num= 2;

        for(int i = 0; i < num; i++ ){
            colorPattern.push_back( sequencePattern[i] );
            colorPattern.push_back( sequencePattern[i+3] );
            colorPattern.push_back( sequencePattern[i+6] );
            colorPattern.push_back( sequencePattern[i+9] );
        }
    }
    std::cout<<"generate_pure_phaseCode_pattern consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::generate_phaseCode_sin_pattern(std::vector<cv::Mat> &srcImage,int pixels_per_period,Direction direction,int stepNum,double gamma)
{

    cv::Mat pattern=cv::Mat::zeros(rows,columns,CV_64FC3);

    double _gamma=1/gamma;

    ////stepNum=4, codePhase = -(n-2) ,sin = (n-2),  j-1
    ////stepNum=3,codePhase = -(n-3) 后加-CV_PI ,sin =+(n-3) 后加+CV_PI ,j-2

    if(direction==Vertical)
    {      
        for(unsigned int n=0;n<stepNum;++n)
        {
            for(unsigned int i=0;i<rows;++i)
            {
                double *ptr=pattern.ptr<double>(i);

                for(unsigned int j=0;j<columns;++j)
                {
                    if(stepNum==4 )
                    {
                        ////  one stair
#if 0
                        unsigned int ladderNum = 32;
                        double phaseCode =  2*CV_PI*(floor((double)(j-1)/pixels_per_period)/(ladderNum));
                        //double assistPhaseCode =  2*CV_PI*(floor((double)(j-1+pixels_per_period/2)/pixels_per_period)/(ladderNum));
                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+1]=0;
                        ////辅助阶梯
                        //ptr[j*3+1]=pow(0.5+0.5*cos(assistPhaseCode -(n-2)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+2]=pow(0.5+0.5*cos(phaseCode-(n-2)*2*CV_PI/stepNum ),_gamma);
#endif

                        ////  over one stair
#if 1
                        unsigned int ladderNum = 8;
                        double phaseCode =  2*CV_PI*(floor((double)((j-1)%(ladderNum*pixels_per_period))/pixels_per_period))/(ladderNum);
                        unsigned int subladderNum = 4;
                        double phaseCode1 = 2*CV_PI*(floor((double)(j-1)/(ladderNum*pixels_per_period))/(subladderNum));
                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+1]=pow(0.5 +0.5*cos(phaseCode1-(n-2)*2*CV_PI/stepNum ),_gamma);
                       // ptr[j*3+1]=0;
                        ptr[j*3+2]=pow(0.5 +0.5*cos(phaseCode-(n-2)*2*CV_PI/stepNum ),_gamma);
#endif

                    }else{
                        ////  one stair
#if 0
                        unsigned int ladderNum = 32;
                        double phaseCode =  2*CV_PI*(floor((double)(j-1)/pixels_per_period)/(ladderNum));
                        //double assistPhaseCode =  2*CV_PI*(floor((double)(j-2+pixels_per_period/2)/pixels_per_period)/(ladderNum));
                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(j)/pixels_per_period+(n-4)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+1]=0;
                        ////辅助阶梯
                        //ptr[j*3+1]=pow(0.5+0.5*cos(assistPhaseCode -(n-3)*2*CV_PI/stepNum -CV_PI ),_gamma);
                        ptr[j*3+2]=pow(0.5+0.5*cos(phaseCode-(n-4)*2*CV_PI/stepNum ),_gamma);
#endif
                        ////  over one stair
#if 1
                        unsigned int ladderNum = 8;
                        double phaseCode =  2*CV_PI*(floor((double)((j-1)%(ladderNum*pixels_per_period))/pixels_per_period))/(ladderNum);
                        unsigned int subladderNum = 4;
                        double phaseCode1 = 2*CV_PI*(floor((double)(j-1)/(ladderNum*pixels_per_period))/(subladderNum));
                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(j)/pixels_per_period+(n-4)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+1]=pow(0.5+0.5*cos(phaseCode1 -(n-4)*2*CV_PI/stepNum ) ,_gamma);
                        //ptr[j*3+1]= 0;
                        ptr[j*3+2]=pow(0.5+0.5*cos(phaseCode -(n-4)*2*CV_PI/stepNum ) ,_gamma);
#endif
                    }

                }
            }
            cv::Mat _pattern;
            pattern.convertTo(_pattern,CV_8UC3,255,0);
            srcImage.push_back(_pattern);
        }

    }else{

        for(unsigned int n=0;n<stepNum;++n)
        {
            for(unsigned int i=0;i<rows;++i)
            {
                double *ptr=pattern.ptr<double>(i);

                for(unsigned int j=0;j<columns;++j)
                {
                    if(stepNum==4 )
                    {
                        ////  one stair
#if 0
                        unsigned int ladderNum = 20;
                        double phaseCode =  2*CV_PI*(floor((double)(i-1)/pixels_per_period)/(ladderNum));
                        //double assistPhaseCode =  2*CV_PI*(floor((double)(i-1+pixels_per_period/2)/pixels_per_period)/(ladderNum));
                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(i)/pixels_per_period+(n-2)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+1]=0;
                        ////辅助阶梯
                        //ptr[j*3+1]=pow(0.5+0.5*cos(assistPhaseCode -(n-2)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+2]=pow(0.5+0.5*cos(phaseCode-(n-2)*2*CV_PI/stepNum ),_gamma);
#endif
                        ////  over one stair
#if 1
                        unsigned int ladderNum = 5;
                        double phaseCode =  2*CV_PI*(floor((double)((i-1)%(ladderNum*pixels_per_period))/pixels_per_period))/(ladderNum);
                        unsigned int subladderNum = 4;
                        double phaseCode1 = 2*CV_PI*(floor((double)(i-1)/(ladderNum*pixels_per_period))/(subladderNum));

                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(i)/pixels_per_period+(n-2)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+1]=pow(0.5 +0.5*cos(phaseCode1-(n-2)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+2]=pow(0.5 +0.5*cos(phaseCode-(n-2)*2*CV_PI/stepNum ),_gamma);
#endif

                    }else{
                        ////  one stair
#if 0
                        unsigned int ladderNum = 20;
                        double phaseCode =  2*CV_PI*(floor((double)(i-1)/pixels_per_period)/(ladderNum));
                        //double assistPhaseCode =  2*CV_PI*(floor((double)(i-2+pixels_per_period/2)/pixels_per_period)/(ladderNum));
                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(i)/pixels_per_period+(n-4)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+1]=0;
                        ////辅助阶梯
                        //ptr[j*3+1]=pow(0.5+0.5*cos(assistPhaseCode -(n-3)*2*CV_PI/stepNum -CV_PI ),_gamma);
                        ptr[j*3+2]=pow(0.5+0.5*cos(phaseCode-(n-4)*2*CV_PI/stepNum ),_gamma);
#endif

                        ////  over one stair
#if 1
                        unsigned int ladderNum = 5;
                        double phaseCode =  2*CV_PI*(floor((double)((i-1)%(ladderNum*pixels_per_period))/pixels_per_period))/(ladderNum);
                        unsigned int subladderNum = 4;
                        double phaseCode1 = 2*CV_PI*(floor((double)(i-1)/(ladderNum*pixels_per_period))/(subladderNum));

                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(i)/pixels_per_period+(n-4)*2*CV_PI/stepNum ),_gamma);
                        ptr[j*3+1]=pow(0.5+0.5*cos(phaseCode1 -(n-4)*2*CV_PI/stepNum ) ,_gamma);
                        ptr[j*3+2]=pow(0.5+0.5*cos(phaseCode -(n-4)*2*CV_PI/stepNum ) ,_gamma);
#endif
                    }

                }
            }
            cv::Mat _pattern;
            pattern.convertTo(_pattern,CV_8UC3,255,0);
            srcImage.push_back(_pattern);
        }
    }
}

void Yuan_pattern::generate_pure_phaseCode2016_pattern(std::vector<cv::Mat> &colorPattern, int pixels_per_period, int stepNum, Direction direction)
{
    QTime t1;
    t1.start();

    std::vector<cv::Mat> sequencePattern;

    if(direction==Vertical)
    {
        for(int n = 0; n < stepNum; ++n){

            cv::Mat sin_pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            cv::Mat phase_pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            uchar *sinPtr   = sin_pattern.ptr<uchar>(0);
            uchar *phasePtr = phase_pattern.ptr<uchar>(0);

            for(int j=0;j<columns;++j)
            {
                unsigned int ladderNum = 4;
                double phaseCode =  2*CV_PI*(floor((double)((j-1)%(ladderNum*2*pixels_per_period))/(2*pixels_per_period)))/(ladderNum);

                sinPtr[j*3]   = 128+127*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/stepNum );
                phasePtr[j*3]   = 128+127*cos( phaseCode - (n-2)*2*CV_PI/stepNum );

            }
            for(int i = 1; i <rows;++i)
            {
                (sin_pattern.row(0)).copyTo(sin_pattern.row(i));
                (phase_pattern.row(0)).copyTo(phase_pattern.row(i));
            }
            sequencePattern.push_back(sin_pattern);
            sequencePattern.push_back(phase_pattern);
        }

    }else{

        for(int n = 0; n < stepNum; ++n){

            cv::Mat sin_pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            cv::Mat phase_pattern = cv::Mat::zeros(rows,columns,CV_8UC3);

            for(int j=0;j<rows;++j)
            {
                unsigned int ladderNum = 4;
                double phaseCode =  2*CV_PI*(floor((double)((j-1)%(ladderNum*2*pixels_per_period))/(2*pixels_per_period)))/(ladderNum);

               sin_pattern.at<uchar>(j,0) = 128+127*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/stepNum );
               phase_pattern.at<uchar>(j,0) = 128+127*cos(phaseCode-(n-2)*2*CV_PI/stepNum );

            }

            for(int i = 1; i <columns;++i)
            {
                sin_pattern.col(0).copyTo(sin_pattern.col(i));
                phase_pattern.col(0).copyTo(phase_pattern.col(i));
            }
            sequencePattern.push_back(sin_pattern);
            sequencePattern.push_back(phase_pattern);
        }
    }
    for(int i = 0; i < 2; i++ ){
        colorPattern.push_back( sequencePattern[i] );
        colorPattern.push_back( sequencePattern[i+2] );
        colorPattern.push_back( sequencePattern[i+4] );
        colorPattern.push_back( sequencePattern[i+6] );
    }
    std::cout<<"generate_pure_phaseCode_pattern consumed : "<<t1.elapsed()<<" ms\n";

}

void Yuan_pattern::generate_phaseCode_grayCode_sin_pattern(std::vector<cv::Mat> &srcImage, int pixels_per_period, Direction direction, int stepNum, double gamma)
{
    QTime t1;
    t1.start();

    cv::Mat pattern=cv::Mat::zeros(rows,columns,CV_64FC3);

    double _gamma=1/gamma;

    ////stepNum=4, codePhase = -(n-2) ,sin = (n-2),  j-1
    ////stepNum=3,codePhase = -(n-3) 后加-CV_PI ,sin =+(n-3) 后加+CV_PI ,j-2

    if(direction==Vertical)
    {
        double phaseCode = 0 ;
        for(unsigned int n=0;n<stepNum;++n)
        {
            for(unsigned int i=0;i<rows;++i)
            {
                double *ptr=pattern.ptr<double>(i);

                for(unsigned int j=0;j<columns;++j)
                {
//                    if(stepNum==4 || stepNum==10 )
//                    {
                        int m = n-2;
                        if(stepNum==8) m = n-4;
                        pixels_per_period= 40 ;
                        unsigned int ladderNum = 8;
                        if( (j>pixels_per_period*ladderNum && j<= pixels_per_period*ladderNum*2)||
                             ( j> pixels_per_period*ladderNum*3 ) )
                             phaseCode = 2*CV_PI*((floor((double)((j-1)%(ladderNum*pixels_per_period))/pixels_per_period))/(ladderNum));
                        else
                             phaseCode = 2*CV_PI*(ladderNum-1-(floor((double)((j-1)%(ladderNum*pixels_per_period))/pixels_per_period)))/(ladderNum);

                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(j)/pixels_per_period+ m*2*CV_PI/stepNum ),_gamma);

                        if(n==0 && j> pixels_per_period*ladderNum*2 ) ptr[j*3+1] = 1;
                        else if(n==1 && j>pixels_per_period*ladderNum && j<= pixels_per_period*ladderNum*3 ) ptr[j*3+1] = 1;
                        else if(n==2 && j<= pixels_per_period*ladderNum*2 ) ptr[j*3+1] = 1;
                        else if(n==3 && (j<=pixels_per_period*ladderNum || j> pixels_per_period*ladderNum*3)) ptr[j*3+1] = 1;
                        else ptr[j*3+1] = 0;

                        ptr[j*3+2]=pow(0.5 +0.5*cos(phaseCode - m*2*CV_PI/stepNum ),_gamma);


//                    }else{


//                    }

                }
            }
            cv::Mat _pattern;
            pattern.convertTo(_pattern,CV_8UC3,255,0);
            srcImage.push_back(_pattern);
        }

    }else{

        double phaseCode = 0 ;
        for(unsigned int n=0;n<stepNum;++n)
        {
            for(unsigned int i=0;i<rows;++i)
            {
                double *ptr=pattern.ptr<double>(i);

                for(unsigned int j=0;j<columns;++j)
                {
//                    if(stepNum==4 || stepNum==10 )
//                    {
                        int m = n-2;
                        if(stepNum==8) m = n-4;
                        pixels_per_period= 40 ;
                        unsigned int ladderNum = 5;

                        if( (i>pixels_per_period*ladderNum && i<= pixels_per_period*ladderNum*2) ||
                              ( i> pixels_per_period*ladderNum*3 )  )
                            phaseCode = 2*CV_PI*((floor((double)((i-1)%(ladderNum*pixels_per_period))/pixels_per_period))/(ladderNum));
                        else
                            phaseCode = 2*CV_PI*(ladderNum-1-(floor((double)((i-1)%(ladderNum*pixels_per_period))/pixels_per_period)))/(ladderNum);

                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(i)/pixels_per_period+ m*2*CV_PI/stepNum ),_gamma);

                        if(n==0 && i> pixels_per_period*ladderNum*2 ) ptr[j*3+1] = 1;
                        else if(n==1 && i>pixels_per_period*ladderNum && i<= pixels_per_period*ladderNum*3 ) ptr[j*3+1] = 1;
                        else if(n==2 && i<= pixels_per_period*ladderNum*2 ) ptr[j*3+1] = 1;
                        else if(n==3 && (i<=pixels_per_period*ladderNum || i> pixels_per_period*ladderNum*3)) ptr[j*3+1] = 1;
                        else ptr[j*3+1] = 0;

                        ptr[j*3+2]=pow(0.5 +0.5*cos(phaseCode - m*2*CV_PI/stepNum ),_gamma);


//                    }else{


//                    }

                }
            }
            cv::Mat _pattern;
            pattern.convertTo(_pattern,CV_8UC3,255,0);
            srcImage.push_back(_pattern);
        }
    }

    std::cout<<"the generate_phaseCode_grayCode_sin_pattern consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::generate_binary_grayCode_sin_pattern(std::vector<cv::Mat> &srcImage, int pixels_per_period, Direction direction, int stepNum, double gamma)
{
    QTime t1;
    t1.start();

    cv::Mat pattern=cv::Mat::zeros(rows,columns,CV_64FC3);

    double _gamma=1/gamma;

    ////stepNum=4, codePhase = -(n-2) ,sin = (n-2),  j-1
    ////stepNum=3,codePhase = -(n-3) 后加-CV_PI ,sin =+(n-3) 后加+CV_PI ,j-2

    if(direction==Vertical)
    {
        for(unsigned int n=0;n<stepNum;++n)
        {
            for(unsigned int i=0;i<rows;++i)
            {
                double *ptr=pattern.ptr<double>(i);

                for(unsigned int j=0;j<columns;++j)
                {
                    if(stepNum==4)
                    {
                        pixels_per_period= columns/16 ;

                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/stepNum ),_gamma);

                        if(n==0 && j> columns/2 ) ptr[j*3+1] = 255;
                        else if(n==1 && ((j>columns/4) && (j<= columns*3/4)) ) ptr[j*3+1] = 255;
                        else if(n==2 && ((j>columns*2/16 && j<=columns*6/16) || (j>columns*10/16 && j<=columns*14/16) )) ptr[j*3+1] = 255;
                        else if(n==3 && ( (j>columns*1/16 && j<=columns*3/16) ||
                                          (j>columns*5/16 && j<=columns*7/16) ||
                                          (j>columns*9/16 && j<=columns*11/16)||
                                          (j>columns*13/16 && j<=columns*15/16) )) ptr[j*3+1] = 255;
                        else
                            ptr[j*3+1] = 0;

                        ptr[j*3+2]= 1 - ptr[j*3+1];


                    }else{


                    }

                }
            }
            cv::Mat _pattern;
            pattern.convertTo(_pattern,CV_8UC3,255,0);
            srcImage.push_back(_pattern);
        }

    }else{

        for(unsigned int n=0;n<stepNum;++n)
        {
            for(unsigned int i=0;i<rows;++i)
            {
                double *ptr=pattern.ptr<double>(i);

                for(unsigned int j=0;j<columns;++j)
                {
                    if(stepNum==4)
                    {
                        pixels_per_period= rows/16 ;

                        ptr[j*3]  =pow(0.5+0.5*cos(2*CV_PI*(i)/pixels_per_period+(n-2)*2*CV_PI/stepNum ),_gamma);

                        if(n==0 && i> rows/2 ) ptr[j*3+1] = 255;
                        else if(n==1 && i>rows/4 && i<= rows*3/4 ) ptr[j*3+1] = 255;
                        else if(n==2 && ((i>rows*2/16 && i<=rows*6/16) || (i>rows*10/16 && i<=rows*14/16) ) ) ptr[j*3+1] = 255;
                        else if(n==3 && ( (i>rows*1/16 && i<=rows*3/16) ||
                                          (i>rows*5/16 && i<=rows*7/16) ||
                                          (i>rows*9/16 && i<=rows*11/16)||
                                          (i>rows*13/16 && i<=rows*15/16) )) ptr[j*3+1] = 255;
                        else
                            ptr[j*3+1] = 0;

                        ptr[j*3+2]= 1 - ptr[j*3+1];


                    }else{


                    }

                }
            }
            cv::Mat _pattern;
            pattern.convertTo(_pattern,CV_8UC3,255,0);
            srcImage.push_back(_pattern);
        }
    }

    std::cout<<"the generate_binary_grayCode_sin_pattern consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::generate_composed_sin_pattern(std::vector<cv::Mat> &srcImage, int pixels_per_period, int stepNum, double gamma)
{
    cv::Mat pattern=cv::Mat::zeros(rows,columns,CV_64FC3);

    double _gamma=1/gamma;

    //stepNum=4, codePhase = -(n-2) ,sin = (n-2),  j-1
    //stepNum=3,codePhase = -(n-3) 后加-CV_PI ,sin =+(n-3) 后加+CV_PI ,j-2

        for(unsigned int n=0;n<stepNum;++n)
        {
            for(unsigned int i=0;i<rows;++i)
            {
                double *ptr=pattern.ptr<double>(i);

                for(unsigned int j=0;j<columns;++j)
                {

                        unsigned int ladderNumX= 12 ,ladderNumY= 7 ;

                        double codePhaseX = 2*CV_PI*(floor((double)(j-1)/pixels_per_period)/(ladderNumX));//%(ladderNum*pixels_per_period)
                        double codePhaseY = 2*CV_PI*(floor((double)(i-1)/pixels_per_period)/(ladderNumY));

                        ptr[j*3]  =pow(0.5+0.4*cos(2*CV_PI*(j)/pixels_per_period-n*2*CV_PI/stepNum
                                                   + 0.1*cos(codePhaseX-n*4*CV_PI/stepNum)),_gamma);

                        ptr[j*3+1]=0;

                        ptr[j*3+2]=pow(0.5+0.4*cos(2*CV_PI*(i)/pixels_per_period-n*2*CV_PI/stepNum
                                                   + 0.1*cos(codePhaseY-n*4*CV_PI/stepNum)),_gamma);

                }
            }
            cv::Mat _pattern;
            pattern.convertTo(_pattern,CV_8UC3,255,0);
            srcImage.push_back(_pattern);
        }
}

void Yuan_pattern::generate_gradual_change_gray_pattern(std::vector<cv::Mat> &srcImage,Color color_select)
{
    for(unsigned int gray=0;gray<256;gray += 10)
    {
        cv::Mat image = cv::Mat::zeros(rows,columns,CV_8UC3);
        for(int i=0;i<rows;++i)
        {
            uchar *ptr = image.ptr<uchar>(i);
            for(int j=0;j<columns;++j)
            {
                switch(color_select)
                {
                    case Blue:
                    {
                        ptr[j*3] = gray;
                        ptr[j*3+1] = 0;
                        ptr[j*3+2] =0 ;
                    }
                        break;
                    case Green:
                    {
                        ptr[j*3] = 0;
                        ptr[j*3+1] = gray;
                        ptr[j*3+2] =0 ;
                    }
                        break;
                    case Red:
                    {
                        ptr[j*3] = 0;
                        ptr[j*3+1] = 0;
                        ptr[j*3+2] = gray ;
                    }
                        break;
                }

            }
        }
        srcImage.push_back(image);
    }
}

void Yuan_pattern::generate_gamma_pure_sinusoidal_pattern(std::vector<cv::Mat> &colorPattern, int pixels_per_period, int stepNum, Color color_selection, Direction direction, double &gammaA, std::vector<double> &gammaB)
{
    QTime t1;
    t1.start();
    if(direction!=Horizontal)
    {
        for(int n=0;n<stepNum;n++)
        {
            int m= n-2;
            if(stepNum==4)
            {
                m=n-2;
            }else if(stepNum==16){
                m=n-8;
            }else
            {
                m=n;
            }
            cv::Mat pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            double grayValue = 0;
            double realValue = 0;
            for(int i=0;i<rows;++i)
            {
                uchar *ptr=pattern.ptr<uchar>(i);
                for(int j=0;j<columns;++j)
                {
//                    if(i==0 && j==0)
//                    {
//                        std::cout<<gammaA<<" , "<<gammaB[0]<<" , "<<gammaB[1]<<" , "<<gammaB[2]<<" , "
//                            <<gammaB[3]<<" , "<<gammaB[4]<<" , "<<gammaB[5]<<" , "<<gammaB[6]<<std::endl;
//                    }
                    realValue = 128+127*cos(2*CV_PI*(j)/pixels_per_period+m*2*CV_PI/stepNum);
                    //realValue = 0.5+0.5*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/3 -CV_PI/3;
//                    grayValue = gammaA +gammaB[0]*realValue +gammaB[1]*pow(realValue,2) +gammaB[2]*pow(realValue,3) +gammaB[3]*pow(realValue,4)
//                             +gammaB[4]*pow(realValue,5) +gammaB[5]*pow(realValue,6) +gammaB[6]*pow(realValue,7);
                    grayValue = gammaTable[(int)realValue];

//                    if(i==0 &&j%10==0 && n==0)
//                    {
//                        std::cout<<(int)grayValue<<" ";
//                        if(j%100==0)std::cout<<std::endl;
//                    }
//                    std::cout<<std::endl;
                    ptr[j*3] = (uchar)grayValue;
                    ptr[j*3+1]=0;
                    ptr[j*3+2]=0;
                }
            }
            colorPattern.push_back(pattern);
        }
    }else
    {
        for(int n=0;n<stepNum;n++)
        {
            int m= n-2;
            if(stepNum==4)
            {
                m=n-2;
            }else if(stepNum==16){
                m=n-8;
            }else
            {
                m=n;
            }
            cv::Mat pattern = cv::Mat::zeros(rows,columns,CV_8UC3);
            double grayValue = 0;
            double realValue = 0;
            for(int i=0;i<rows;++i)
            {
                uchar *ptr=pattern.ptr<uchar>(i);
                for(int j=0;j<columns;++j)
                {
                    realValue = 128+127*cos(2*CV_PI*(i)/pixels_per_period+m*2*CV_PI/stepNum);
                    //realValue = 0.5+0.5*cos(2*CV_PI*(j)/pixels_per_period+(n-2)*2*CV_PI/3 -CV_PI/3;
//                    grayValue = gammaA +gammaB[0]*realValue +gammaB[1]*pow(realValue,2) +gammaB[2]*pow(realValue,3) +gammaB[3]*pow(realValue,4)
//                             +gammaB[4]*pow(realValue,5) +gammaB[5]*pow(realValue,6) +gammaB[6]*pow(realValue,7);
                    grayValue = gammaTable[(int)realValue];

                    ptr[j*3] = (uchar)grayValue;
                    ptr[j*3+1]=0;
                    ptr[j*3+2]=0;
                }
            }
            colorPattern.push_back(pattern);
        }
    }

    std::cout<<"generate_gamma_pure_sinusoidal_pattern consumed "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::generate_gamma_phaseCode_grayCode_sin_pattern(std::vector<cv::Mat> &srcImage, int pixels_per_period, Direction direction, int stepNum,
                                                                 double &blueGammaA,std::vector<double> &blueGammaB)
{
    QTime t1;
    t1.start();
    ////stepNum=4, codePhase = -(n-2) ,sin = (n-2),  j-1
    ////stepNum=3,codePhase = -(n-3) 后加-CV_PI ,sin =+(n-3) 后加+CV_PI ,j-2

    if(direction==Vertical)
    {
        double phaseCode = 0,sinRealValue = 0,sinGrayValue = 0 ,
                            phaseRealValue = 0,phaseGrayValue =0;

        for(unsigned int n=0;n<stepNum;++n)
        {
            int m = n - 2 ;
            if(stepNum==8) m = n - 4;
            cv::Mat pattern=cv::Mat::zeros(rows,columns,CV_8UC3);
            for(unsigned int i=0;i<rows;++i)
            {
                uchar *ptr=pattern.ptr<uchar>(i);

                for(unsigned int j=0;j<columns;++j)
                {
                    if(stepNum==4 || stepNum==8 )
                    {
                        pixels_per_period= 40 ;
                        unsigned int ladderNum = 8;
                        if( (j>pixels_per_period*ladderNum && j<= pixels_per_period*ladderNum*2)||
                             ( j> pixels_per_period*ladderNum*3 ) )
                             phaseCode = 2*CV_PI*((floor((double)((j-1)%(ladderNum*pixels_per_period))/pixels_per_period))/(ladderNum));
                        else
                             phaseCode = 2*CV_PI*(ladderNum-1-(floor((double)((j-1)%(ladderNum*pixels_per_period))/pixels_per_period)))/(ladderNum);

                        sinRealValue  = 128+127*cos(2*CV_PI*(j)/pixels_per_period+m*2*CV_PI/stepNum );
//                        sinGrayValue = blueGammaA + blueGammaB[0]*sinRealValue + blueGammaB[1]*pow(sinRealValue,2) + blueGammaB[2]*pow(sinRealValue,3)
//                                            + blueGammaB[3]*pow(sinRealValue,4) + blueGammaB[4]*pow(sinRealValue,5) + blueGammaB[5]*pow(sinRealValue,6)
//                                           + blueGammaB[6]*pow(sinRealValue,7);
//                        if(sinGrayValue < 0)ptr[j*3]  = 0;
//                        else if(sinGrayValue > 255)ptr[j*3]  = 255;
//                        else ptr[j*3]  = (int)sinGrayValue;
                        sinGrayValue = gammaTable[(int)sinRealValue];
//                        if(sinRealValue < 140) ptr[j*3]  = (uchar)gammaTable[(int)sinRealValue];
//                        else ptr[j*3]  = (uchar)sinRealValue;
                        ptr[j*3]  = (uchar)sinGrayValue;

                        if(n==0 && j> pixels_per_period*ladderNum*2 ) ptr[j*3+1] = 255;
                        else if(n==1 && j>pixels_per_period*ladderNum && j<= pixels_per_period*ladderNum*3 ) ptr[j*3+1] = 255;
                        else if(n==2 && j<= pixels_per_period*ladderNum*2 ) ptr[j*3+1] = 255;
                        else if(n==3 && (j<=pixels_per_period*ladderNum || j> pixels_per_period*ladderNum*3)) ptr[j*3+1] = 255;
                        else ptr[j*3+1] = 0;

                        phaseRealValue = 128 +127*cos(phaseCode-m*2*CV_PI/stepNum);
//                        phaseGrayValue = blueGammaA + blueGammaB[0]*phaseRealValue + blueGammaB[1]*pow(phaseRealValue,2) + blueGammaB[2]*pow(phaseRealValue,3)
//                                + blueGammaB[3]*pow(phaseRealValue,4) + blueGammaB[4]*pow(phaseRealValue,5) + blueGammaB[5]*pow(phaseRealValue,6)
//                               + blueGammaB[6]*pow(phaseRealValue,7);
//                        if(phaseGrayValue < 0)ptr[j*3+2]  = 0;
//                        else if(phaseGrayValue > 255)ptr[j*3+2]  = 255;
//                        else ptr[j*3+2]  = (int)phaseGrayValue;
                        phaseGrayValue = gammaTable[(int)phaseRealValue];
//                        if(phaseRealValue < 140) ptr[j*3+2]  = (uchar)phaseGrayValue;
//                        else ptr[j*3+2]  = (uchar)phaseRealValue;
                        ptr[j*3+2]  = (uchar)phaseGrayValue;

                    }else{


                    }

                }
            }
            srcImage.push_back(pattern);
        }

    }else{

        double phaseCode = 0,sinRealValue = 0,sinGrayValue = 0 ,
                            phaseRealValue = 0,phaseGrayValue =0;

        for(unsigned int n=0;n<stepNum;++n)
        {
            int m = n - 2 ;
            if(stepNum==8) m = n - 4;
            cv::Mat pattern=cv::Mat::zeros(rows,columns,CV_8UC3);
            for(unsigned int i=0;i<rows;++i)
            {
                uchar *ptr=pattern.ptr<uchar>(i);

                for(unsigned int j=0;j<columns;++j)
                {
                    if(stepNum==4 || stepNum==8 )
                    {
                        pixels_per_period= 40 ;
                        unsigned int ladderNum = 5;

                        if( (i>pixels_per_period*ladderNum && i<= pixels_per_period*ladderNum*2) ||
                              ( i> pixels_per_period*ladderNum*3 )  )
                            phaseCode = 2*CV_PI*((floor((double)((i-1)%(ladderNum*pixels_per_period))/pixels_per_period))/(ladderNum));
                        else
                            phaseCode = 2*CV_PI*(ladderNum-1-(floor((double)((i-1)%(ladderNum*pixels_per_period))/pixels_per_period)))/(ladderNum);

                        sinRealValue  = 128+127*cos(2*CV_PI*(i)/pixels_per_period + m*2*CV_PI/stepNum );
//                        sinGrayValue = blueGammaA + blueGammaB[0]*sinRealValue + blueGammaB[1]*pow(sinRealValue,2) + blueGammaB[2]*pow(sinRealValue,3)
//                                 + blueGammaB[3]*pow(sinRealValue,4) + blueGammaB[4]*pow(sinRealValue,5) + blueGammaB[5]*pow(sinRealValue,6)
//                                + blueGammaB[6]*pow(sinRealValue,7);
                        sinGrayValue = gammaTable[(int)sinRealValue];

//                        if(sinRealValue < 140) ptr[j*3]  = (uchar)sinGrayValue;
//                        else ptr[j*3]  = (uchar)sinRealValue;
                        ptr[j*3]  = (uchar)sinGrayValue;

                        if(n==0 && i> pixels_per_period*ladderNum*2 ) ptr[j*3+1] = 255;
                        else if(n==1 && i>pixels_per_period*ladderNum && i<= pixels_per_period*ladderNum*3 ) ptr[j*3+1] = 255;
                        else if(n==2 && i<= pixels_per_period*ladderNum*2 ) ptr[j*3+1] = 255;
                        else if(n==3 && (i<=pixels_per_period*ladderNum || i> pixels_per_period*ladderNum*3)) ptr[j*3+1] = 255;
                        else ptr[j*3+1] = 0;

                        phaseRealValue = 128 +127*cos(phaseCode - m*2*CV_PI/stepNum);
//                        phaseGrayValue = blueGammaA + blueGammaB[0]*phaseRealValue + blueGammaB[1]*pow(phaseRealValue,2) + blueGammaB[2]*pow(phaseRealValue,3)
//                                + blueGammaB[3]*pow(phaseRealValue,4) + blueGammaB[4]*pow(phaseRealValue,5) + blueGammaB[5]*pow(phaseRealValue,6)
//                               + blueGammaB[6]*pow(phaseRealValue,7);
                        phaseGrayValue = gammaTable[(int)phaseRealValue];

//                        if(phaseRealValue < 140) ptr[j*3+2]  = (uchar)phaseGrayValue;
//                        else ptr[j*3+2]  = (uchar)phaseRealValue;
                        ptr[j*3+2]  = (uchar)phaseGrayValue;

                    }else{


                    }

                }
            }
            srcImage.push_back(pattern);
        }
    }

    std::cout<<"the generate_gamma_phaseCode_grayCode_sin_pattern consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::transform_to_binary_pattern(cv::Mat &srcImage, Direction _direction)
{
    QTime t1;
        t1.start();

        unsigned int jj = 0;
        double error_j = 0,error_jj = 0;
        for(unsigned int i = 0; i < rows; ++i){
            for(unsigned int j = 0; j < columns; ++j){

                jj = columns - 1 - j;
                error_j = (srcImage.at<uchar>(i,j)>=128) ? (srcImage.at<uchar>(i,j) - 255) : srcImage.at<uchar>(i,j);
                error_jj = (srcImage.at<uchar>(i,jj)>=128) ? (srcImage.at<uchar>(i,jj) - 255) : srcImage.at<uchar>(i,jj);

                if( i != rows - 1){

                    //odd
                    if(i%2==0){
                        if(j == 0){
                            srcImage.at<uchar>(i,j+1) += error_j*0.5;
                            srcImage.at<uchar>(i+1,j) += error_j*0.25;
                        }
                        else if(j ==  columns - 1){
                            srcImage.at<uchar>(i+1,j-1) += error_j*0.25;
                            srcImage.at<uchar>(i+1,j) += error_j*0.25;
                        }
                        else{
                            srcImage.at<uchar>(i+1,j-1) += error_j*0.25;
                            srcImage.at<uchar>(i+1,j) += error_j*0.25;
                            srcImage.at<uchar>(i,j+1) += error_j*0.5;
                        }
                    }else{
                        if(jj==0){
                            srcImage.at<uchar>(i+1,jj) += error_jj*0.25;
                            srcImage.at<uchar>(i+1,jj+1) += error_jj*0.25;
                        }
                        else if(jj ==  columns - 1){
                            srcImage.at<uchar>(i,jj-1) += error_jj*0.5;
                            srcImage.at<uchar>(i+1,jj) += error_jj*0.25;
                        }
                        else{
                            srcImage.at<uchar>(i,jj-1) += error_jj*0.5;
                            srcImage.at<uchar>(i+1,jj) += error_jj*0.25;
                            srcImage.at<uchar>(i+1,jj+1) += error_jj*0.25;
                        }
                    }
                }else{
                    if(jj > 0 && i%2 != 0) srcImage.at<uchar>(i,jj-1) += error_jj*0.5;
                    if(jj > 0 && i%2 == 0) srcImage.at<uchar>(i,j+1) += error_jj*0.5;
                }
            }
        }
        for(int i = 0; i < rows; ++i){
            uchar* patternPtr = srcImage.ptr<uchar>(i);
            for(int j = 0; j < columns; ++j){
                patternPtr[j] = patternPtr[j]>=128 ? 255:0;
            }
        }

//        int height = srcImage.rows;
//        int weight = srcImage.cols;
//        cv::Mat value = cv::Mat::zeros(height,weight,CV_8UC1);
//        cv::Mat error = cv::Mat::zeros(height,weight,CV_8UC1);
//        int T = 128;
//        for (int i = 0; i < height; i++)
//                {
//                    for (int j = 0; j < weight; j++)
//                    {
//                        if (i == (height - 1))
//                        {
//                            if (srcImage.at<uchar>(i, j) >= T)
//                            {
//                                value.at<uchar>(i, j) = 255;
//                            }
//                            else
//                            {
//                                value.at<uchar>(i, j) = 0;
//                            }
//                        }
//                        else
//                        {
//                            if (0 == i % 2)//求余数 从左到右--奇数行
//                            {
//                                if (j == 0 || j == (weight - 1))
//                                {
//                                    if (srcImage.at<uchar>(i, j) >= T)
//                                    {
//                                        value.at<uchar>(i, j) = 255;
//                                    }
//                                    else
//                                    {
//                                        value.at<uchar>(i, j) = 0;
//                                    }
//                                }
//                                else
//                                {
//                                    if (srcImage.at<uchar>(i, j) >= T)
//                                    {
//                                        value.at<uchar>(i, j) = 255;
//                                    }
//                                    else
//                                    {
//                                        value.at<uchar>(i, j) = 0;
//                                    }
//                                    error.at<uchar>(i, j) = srcImage.at<uchar>(i, j) - value.at<uchar>(i, j);//求取误差值

//                                    srcImage.at<uchar>(i, j + 1) += (uchar)(0.5 * error.at<uchar>(i, j));
//                                    srcImage.at<uchar>(i + 1, j) += (uchar)(0.25 * error.at<uchar>(i, j));
//                                    srcImage.at<uchar>(i + 1, j - 1) += (uchar)(0.25 * error.at<uchar>(i, j));

//                                }
//                            }
//                            else //从右到左--偶数行
//                            {
//                                int jj;
//                                jj = weight - 1 - j;
//                                if (jj == 0 || jj == (weight - 1))
//                                {
//                                    if (srcImage.at<uchar>(i, jj) >= T)
//                                    {
//                                        value.at<uchar>(i, jj) = 255;
//                                    }
//                                    else
//                                    {
//                                        value.at<uchar>(i, jj) = 0;
//                                    }
//                                }
//                                else
//                                {
//                                    if (srcImage.at<uchar>(i, jj) >= T)
//                                    {
//                                        value.at<uchar>(i, jj) = 255;
//                                    }
//                                    else
//                                    {
//                                        value.at<uchar>(i, jj) = 0;
//                                    }
//                                    error.at<uchar>(i, jj) = srcImage.at<uchar>(i, jj) - value.at<uchar>(i, jj);//求取误差值

//                                    srcImage.at<uchar>(i, jj - 1) += (uchar)(0.5 * error.at<uchar>(i, jj));
//                                    srcImage.at<uchar>(i + 1, jj) += (uchar)(0.25 * error.at<uchar>(i, jj));
//                                    srcImage.at<uchar>(i + 1, jj + 1) += (uchar)(0.25 * error.at<uchar>(i, jj));

//                                }
//                            }
//                        }
//                    }
//                }

//                for(int i = 0; i < rows; ++i){
//                    uchar* patternPtr = srcImage.ptr<uchar>(i);
//                    for(int j = 0; j < columns; ++j){
//                        patternPtr[j] = patternPtr[j]>=128 ? 255:0;
//                    }
//                }
        std::cout<<"transform_to_binary_pattern consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::decode_color_sin_phaseCode_phaseshift_pattern(std::vector<cv::Mat> &srcImage, cv::Mat &dstImage, cv::Mat &mask, Direction _direction)
{
    QTime t1;
    t1.start();

      unsigned int imageRow=srcImage[0].rows;
      unsigned int imageCol=srcImage[0].cols;

      cv::Mat sinusoidal=srcImage[0].clone();
      cv::Mat phaseCode =srcImage[1].clone();

      cv::Mat unwrapPhase=cv::Mat::zeros(sinusoidal.rows,sinusoidal.cols,CV_64FC1);

      unsigned int i=0,j=0;

      for(i=0;i<imageRow;++i)
      {
          double *phaseCodePtr=phaseCode.ptr<double>(i);
          double *sinusoidalPtr=sinusoidal.ptr<double>(i);
          uchar *maskPtr=mask.ptr<uchar>(i);
          double *unwrapPtr=unwrapPhase.ptr<double>(i);

          for(j=0;j<imageCol;++j)
          {
              if(maskPtr[j]!=0)
              {
                  if(_direction == Vertical)
                  {

                    phaseCodePtr[j]= 31 - floor(phaseCodePtr[j]*32/(2*CV_PI));
                    if(phaseCodePtr[j]==-1)phaseCodePtr[j]=0;
                    unwrapPtr[j]=(phaseCodePtr[j])*2*CV_PI+sinusoidalPtr[j];

                  }else{

                       phaseCodePtr[j]= 19 - floor(phaseCodePtr[j]*20/(2*CV_PI));
                      if(phaseCodePtr[j]==-1)phaseCodePtr[j]=0;
                      unwrapPtr[j]=(phaseCodePtr[j])*2*CV_PI+sinusoidalPtr[j];

                  }

              }
          }
      }



      dstImage=unwrapPhase.clone();

      image_paint.push_back(sinusoidal);
      image_paint.push_back(phaseCode);
      image_paint.push_back(unwrapPhase);

      std::cout<<"the time of decode_phaseCode_sin_pattern is "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::meanValue(cv::Mat mask, cv::Mat &pattern,double &R_mean,double &G_mean,double &B_mean)
{
    if(pattern.channels()!=3)
    {
        return;
        std::cout<<"the number of color_pattern's channel is not correct ... "<<std::endl;
    }

    /********************找最小值*******************/
//    double R_min=255,G_min=255,B_min=255;

//    for(int i=0;i < pattern.rows;++i)
//    {
//        uchar* ptr=pattern.ptr<uchar>(i);
//        uchar* mask_ptr=mask.ptr<uchar>(i);

//        for(int j=0;j < pattern.cols*3;j+=3)
//        {
//            if(mask_ptr[j/3]!=0)
//            {
//                if(R_min > ptr[j+2])
//                    R_min=ptr[j+2];

//                if(G_min > ptr[j+1])
//                    G_min=ptr[j+1];

//                if(B_min > ptr[j])
//                    B_min=ptr[j];
//            }
//        }
//    }
//    std::cout<<"R_min : "<<R_min<<" , "<<"G_min : "<<G_min<<" , "<<"B_min : "<<B_min<<std::endl;

//    for(int i=0;i < pattern.rows;++i)
//    {
//        uchar* ptr=pattern.ptr<uchar>(i);
//        for(int j=0;j < pattern.cols*3;j+=3)
//        {
//            ptr[j]  -= B_min;
//            ptr[j+1]-= G_min;
//            ptr[j+2]-= R_min;
//        }
//    }

    double total_R=0.0,total_G=0.0,total_B=0.0;
    double numbers=0;

    for(int i=250;i<pattern.rows-250;++i)
    {
        uchar* ptr=pattern.ptr<uchar>(i);
        uchar* mask_ptr=mask.ptr<uchar>(i);
        for(int j=300;j<pattern.cols-300;++j)
        {
            if(mask_ptr[j]!=0)
            {
                total_R+=ptr[j*3+2];
                total_G+=ptr[j*3+1];
                total_B+=ptr[j*3];
                numbers++;
            }
        }
    }
    if(numbers==0)
    {
        R_mean=0;
        G_mean=0;
        B_mean=0;
    }
    else
    {
        R_mean=(double)total_R/numbers;
        G_mean=(double)total_G/numbers;
        B_mean=(double)total_B/numbers;
    }
}

void Yuan_pattern::HSI_color_space_transform(cv::Mat &srcImage,cv::Mat &dstImage)
{
    QTime time;
    time.start();
    if(srcImage.channels()!=3)
    {
        std::cout<<"the number of pattern's channels is not correct ... "<<std::endl;
        return;
    }
    cv::Mat HSIimage=cv::Mat::zeros(cv::Size(srcImage.size()),CV_8UC3);
    std::vector <cv::Mat> HSIchannels;
    cv::split(HSIimage, HSIchannels);

    std::vector < cv::Mat > RGBchannels;
    cv::split(srcImage,RGBchannels);

    for (int i = 0; i < srcImage.rows; ++i)
    {
        uchar *Rptr=RGBchannels[2].ptr<uchar>(i);
        uchar *Gptr=RGBchannels[1].ptr<uchar>(i);
        uchar *Bptr=RGBchannels[0].ptr<uchar>(i);

        uchar *Hptr=HSIchannels[0].ptr<uchar>(i);
        uchar *Sptr=HSIchannels[1].ptr<uchar>(i);
        uchar *Iptr=HSIchannels[2].ptr<uchar>(i);

        for (int j = 0; j < srcImage.cols; ++j)
        {
            double H, S, I;
            int Bvalue = Bptr[j];
            int Gvalue = Gptr[j];
            int Rvalue = Rptr[j];

            //求Theta =acos((((Rvalue - Gvalue) + (Rvalue - Bvalue)) / 2) / sqrt(pow((Rvalue - Gvalue), 2) + (Rvalue - Bvalue)*(Gvalue - Bvalue)));
            double numerator =(double) ((Rvalue - Gvalue) + (Rvalue - Bvalue)) / 2;
            double denominator = sqrt(pow((Rvalue - Gvalue), 2) + (Rvalue - Bvalue)*(Gvalue - Bvalue));
            if (denominator == 0) H = 0;
            else{
                double Theta = acos(numerator / denominator) ;
                if (Bvalue <= Gvalue)
                     H = Theta;
                else  H = 2*CV_PI - Theta;
            }
            Hptr[j] = (int)(H);

            //求S = 1-3*min(Bvalue,Gvalue,Rvalue)/(Rvalue+Gvalue+Bvalue);
            int minvalue = Bvalue;
            if (minvalue > Gvalue) minvalue = Gvalue;
            if (minvalue > Rvalue) minvalue = Rvalue;
            numerator = 3 * minvalue;
            denominator = Rvalue + Gvalue + Bvalue;
            if (denominator == 0)  S = 0;
            else{
                S = 1 - numerator / denominator;
            }
            Sptr[j] = (int)(S);

            I= (Rvalue + Gvalue + Bvalue) / 3;
            Iptr[j] = (int)(I);
        }
    }

    cv::merge(HSIchannels, HSIimage);
    dstImage=HSIimage.clone();
    std::cout<<time.elapsed()<<" ms"<<std::endl;
}

bool Yuan_pattern::build_crosstalk_look_up_table(channels ***crossResult, const cv::Mat &crosstalk, const cv::Mat &reflect)
{
    QTime t1;
    t1.start();

    cv::Mat temp = cv::Mat::zeros(3,1,CV_64FC1);
    cv::Mat result = cv::Mat::zeros(3,1,CV_64FC1);
    cv::Mat cross_reflect=cv::Mat(3,3,CV_64FC1);
    cross_reflect = crosstalk * reflect;

    for(int b = 0; b < 256; ++b){
        for(int g = 0; g < 256; ++g){
            for(int r = 0; r < 256; ++r){

                temp.at<double>(0,0) = (double)b ;
                temp.at<double>(1,0) = (double)g ;
                temp.at<double>(2,0) = (double)r ;

                cv::solve(cross_reflect,temp,result);

                if( result.at<double>(0,0) < 0 ){
                    crossResult[b][g][r].blue = 0;
                }else if(result.at<double>(0,0) > 255 ){
                    crossResult[b][g][r].blue = 255;
                }else{
                    crossResult[b][g][r].blue = (uchar)result.at<double>(0,0);
                }

                if( result.at<double>(1,0) < 0 ){
                    crossResult[b][g][r].green = 0;
                }else if(result.at<double>(1,0) > 255 ){
                    crossResult[b][g][r].green = 255;
                }else{
                    crossResult[b][g][r].green = (uchar)result.at<double>(1,0);
                }

                if( result.at<double>(2,0) < 0 ){
                    crossResult[b][g][r].red = 0;
                }else if(result.at<double>(2,0) > 255 ){
                    crossResult[b][g][r].red = 255;
                }else{
                    crossResult[b][g][r].red = (uchar)result.at<double>(2,0);
                }

            }
        }
    }

    std::cout<<"build_crosstalk_look_up_table consumed : "<<t1.elapsed()<<" ms\n";

    return true;

}

bool Yuan_pattern::correct_crosstalk_by_LookUpTable(channels ***crossTable,  cv::Mat &srcImage, cv::Mat &dstImage, const cv::Mat &mask)
{
    QTime t1;
    t1.start();

    if(srcImage.channels()!=3)
    {
        std::cout<<"Error: the channel of srcImage is not correct ... \n";
        return false;
    }

    unsigned int imageHeight = srcImage.rows;
    unsigned int imageWidth = srcImage.cols;

    for(unsigned int i = 0; i < imageHeight; ++i){

        uchar* srcPtr = srcImage.ptr<uchar>(i);
        uchar* dstPtr = dstImage.ptr<uchar>(i);
        const uchar* maskPtr = mask.ptr<uchar>(i);

        for(unsigned int j = 0; j < imageWidth; ++j){

            if(maskPtr[j] != 0){
                dstPtr[j*3]   = crossTable[ srcPtr[j*3] ][ srcPtr[j*3+1] ][ srcPtr[j*3+2] ].blue;
                dstPtr[j*3+1] = crossTable[ srcPtr[j*3] ][ srcPtr[j*3+1] ][ srcPtr[j*3+2] ].green;
                dstPtr[j*3+2] = crossTable[ srcPtr[j*3] ][ srcPtr[j*3+1] ][ srcPtr[j*3+2] ].red;
            }
        }
    }

    std::cout<<"correct_crosstalk_by_LookUpTable consumed : "<<t1.elapsed()<<" ms\n";
    return true;

}

void Yuan_pattern::color_correction(cv::Mat mask, cv::Mat &srcImage,cv::Mat &dst,const cv::Mat &crosstalk,const cv::Mat &reflect)
{
    QTime t1;
    t1.start();    

    cv::Mat dstImage=srcImage.clone();

    if(srcImage.channels()!=3)
    {
        std::cout<<"Error: the channel of srcImage is not correct ... \n";
        return;
    }

//    std::ofstream fout;
//    fout.open("../ScanData/crossiamge/result.txt");

    unsigned int imageHeight = srcImage.rows;
    unsigned int imageWidth = srcImage.cols;

//    if(srcImage.isContinuous())
//    {
//        imageWidth = imageHeight * imageWidth;
//        imageHeight = 1;
//    }

    cv::Mat temp=cv::Mat(3,1,CV_64FC1);
    cv::Mat corss_reflect=cv::Mat(3,3,CV_64FC1);
    corss_reflect = crosstalk * reflect;
    cv::Mat correction_result=cv::Mat(3,1,CV_64FC1);

//    for (char i = 'a'; i <= 'z'; i++)
//       std::cout << "Hello World! " << i << std::endl;
//    #pragma omp parallel for //num_threads(4)
    for(int i=0;i<imageHeight;++i)
    {

        uchar *src_ptr=srcImage.ptr(i);
        uchar *dst_ptr=dstImage.ptr(i);
        uchar *mask_ptr=mask.ptr(i);

        for(int j=0;j<imageWidth;++j)
        {
            if(mask_ptr[j]!=0)
            {
                temp.at<double>(0,0)=(double)src_ptr[j*3];
                temp.at<double>(1,0)=(double)src_ptr[j*3+1];
                temp.at<double>(2,0)=(double)src_ptr[j*3+2];

                cv::solve(corss_reflect,temp,correction_result);

                //std::cout<<"correction_result : "<<std::endl<<correction_result<<std::endl;

                if(correction_result.at<double>(0,0)<0)
                {
                    dst_ptr[j*3]=0;
                }else if(correction_result.at<double>(0,0)>255)
                {
                    dst_ptr[j*3]=255;
                }else
                {
                    dst_ptr[j*3]=correction_result.at<double>(0,0);
                }

                if(correction_result.at<double>(1,0)<0)
                {
                    dst_ptr[j*3+1]=0;
                }else if(correction_result.at<double>(1,0)>255)
                {
                    dst_ptr[j*3+1]=255;
                }else
                {
                    dst_ptr[j*3+1]=correction_result.at<double>(1,0);
                }

                if(correction_result.at<double>(2,0)<0)
                {
                    dst_ptr[j*3+2]=0;
                }else if(correction_result.at<double>(2,0)>255)
                {
                    dst_ptr[j*3+2]=255;
                }else
                {
                    dst_ptr[j*3+2]=correction_result.at<double>(2,0);
                }


//                if(i==700)
//                {
//                    fout<<"blue: "<<blue<<" , green: "<<green<<" , red: "<<red<<"  ";
//                    fout<<"blue: "<<dstImage.at<cv::Vec3b>(i,j)[0]<<" , green: "<<dstImage.at<cv::Vec3b>(i,j)[1]<<" , red: "<<dstImage.at<cv::Vec3b>(i,j)[2]<<std::endl;
//                }

                //std::cout<<(int)srcImage.at<cv::Vec3b>(i,j)[0]<<" , "<<(int)srcImage.at<cv::Vec3b>(i,j)[1]<<" , "<<(int)srcImage.at<cv::Vec3b>(i,j)[2]<<std::endl;

            }
            else
            {
                dst_ptr[j*3]  =0;
                dst_ptr[j*3+1]=0;
                dst_ptr[j*3+2]=0;
            }
        }
    }


    dst=dstImage.clone();
   // fout.close();

   std::cout<<"the color_correction consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::decode_sinusoidal(cv::Mat mask,std::vector<cv::Mat> &srcImageSequence,cv::Mat &dstImage,int stepNum)
{
    int ImageHeight=srcImageSequence[0].rows;
    int ImageWidth =srcImageSequence[0].cols;

    QTime t1;
    t1.start();

    switch (stepNum) {
    case 3:
    {
        if(srcImageSequence.size()< 3)
        {
            std::cout<<"the number of sinusoidal pattern is inadequate ... "<<std::endl;
            return;
        }

        cv::Mat img1= srcImageSequence.at(0);
        cv::Mat img2= srcImageSequence.at(1);
        cv::Mat img3= srcImageSequence.at(2);

        cv::Mat sinusoidal_phase=cv::Mat::zeros(ImageHeight,ImageWidth,CV_64FC1);
        for(int i=0;i<ImageHeight;++i)
        {

            uchar* ptr1= img1.ptr<uchar>(i);
            uchar* ptr2= img2.ptr<uchar>(i);
            uchar* ptr3= img3.ptr<uchar>(i);
            uchar* mask_ptr= mask.ptr<uchar>(i);
            double* optr= sinusoidal_phase.ptr<double>(i);

            for(int j=0;j<ImageWidth;++j)
            {
                if(mask_ptr[j]!=0)
                {
                    optr[j]=(double)atan2((double)(std::sqrt(3)*(ptr1[j]-ptr3[j])),
                                         (double)(2*ptr2[j]-ptr1[j]-ptr3[j]))+CV_PI;
                }else
                {
                    optr[j]=0;
                }
            }
        }

        //cv::normalize(sinusoidal_phase,sinusoidal_phase,1.0,0.0,cv::NORM_MINMAX);

        dstImage=sinusoidal_phase.clone();
    }
        break;
    case 4:
    {
        if(srcImageSequence.size()< 4)
        {
            std::cout<<"the number of sinusoidal pattern is inadequate ... "<<std::endl;
            return;
        }

        cv::Mat img1= srcImageSequence.at(0);
        cv::Mat img2= srcImageSequence.at(1);
        cv::Mat img3= srcImageSequence.at(2);
        cv::Mat img4= srcImageSequence.at(3);

        cv::Mat sinusoidal_phase=cv::Mat::zeros(ImageHeight,ImageWidth,CV_64FC1);

        for(int i=0;i<ImageHeight;++i)
        {

            uchar* ptr1= img1.ptr<uchar>(i);
            uchar* ptr2= img2.ptr<uchar>(i);
            uchar* ptr3= img3.ptr<uchar>(i);
            uchar* ptr4= img4.ptr<uchar>(i);
            uchar* mask_ptr= mask.ptr<uchar>(i);
            double* optr= sinusoidal_phase.ptr<double>(i);

            for(int j=0;j<ImageWidth;++j)
            {
                if(mask_ptr[j]!=0)
                {
                    optr[j]=(double)atan2((ptr4[j]-ptr2[j]),
                        (ptr1[j]-ptr3[j]))+CV_PI;

                }else
                {
                    optr[j]=0;
                }
            }
        }

        //cv::normalize(sinusoidal_phase,sinusoidal_phase,1.0,0.0,cv::NORM_MINMAX);

        dstImage=sinusoidal_phase.clone();
    }
        break;
    default:
        break;
    }
    std::cout<<"the time of decode_sinusoidal is "<<t1.elapsed()<<" ms"<<std::endl;
}

void Yuan_pattern::decode_sinusoidal_LookUpTable(cv::Mat &mask, std::vector<cv::Mat> &srcImageSequence, cv::Mat &dstImage, int stepNum)
{
    int ImageHeight=srcImageSequence[0].rows;
    int ImageWidth =srcImageSequence[0].cols;

    QTime t1;
    t1.start();

    switch (stepNum) {
    case 3:
    {
        if(srcImageSequence.size()< 3)
        {
            std::cout<<"the number of sinusoidal pattern is inadequate ... "<<std::endl;
            return;
        }

        cv::Mat img1= srcImageSequence.at(0);
        cv::Mat img2= srcImageSequence.at(1);
        cv::Mat img3= srcImageSequence.at(2);

        for(int i=0;i<ImageHeight;++i)
        {

            uchar* ptr1= img1.ptr<uchar>(i);
            uchar* ptr2= img2.ptr<uchar>(i);
            uchar* ptr3= img3.ptr<uchar>(i);
            uchar* mask_ptr= mask.ptr<uchar>(i);
            double* optr= dstImage.ptr<double>(i);

            for(int j=0;j<ImageWidth;++j)
            {
                if(mask_ptr[j]!=0)
                {
                    optr[j] = (double)atan2Table_threeStep[ ptr1[j] - ptr3[j] +255 ][ 2*ptr2[j] - ptr1[j] - ptr3[j] +510 ];
                }else
                {
                    optr[j]=0;
                }
            }
        }

        //cv::normalize(sinusoidal_phase,sinusoidal_phase,1.0,0.0,cv::NORM_MINMAX);
    }
        break;
    case 4:
    {
        if(srcImageSequence.size()< 4)
        {
            std::cout<<"the number of sinusoidal pattern is inadequate ... "<<std::endl;
            return;
        }

        cv::Mat img1= srcImageSequence.at(0);
        cv::Mat img2= srcImageSequence.at(1);
        cv::Mat img3= srcImageSequence.at(2);
        cv::Mat img4= srcImageSequence.at(3);

        for(int i=0;i<ImageHeight;++i)
        {

            uchar* ptr1= img1.ptr<uchar>(i);
            uchar* ptr2= img2.ptr<uchar>(i);
            uchar* ptr3= img3.ptr<uchar>(i);
            uchar* ptr4= img4.ptr<uchar>(i);
            uchar* mask_ptr= mask.ptr<uchar>(i);
            double* optr= dstImage.ptr<double>(i);

            for(int j=0;j<ImageWidth;++j)
            {
                if(mask_ptr[j]!=0)
                {
                    optr[j] = (double)atan2Table_fourStep[ ptr4[j]-ptr2[j]+255 ][ ptr1[j]-ptr3[j]+255 ];

                }else
                {
                    optr[j]=0;
                }
            }
        }

        //cv::normalize(sinusoidal_phase,sinusoidal_phase,1.0,0.0,cv::NORM_MINMAX);

    }
        break;
    default:
        break;
    }
    std::cout<<"decode_sinusoidal_LookUpTable consumed "<<t1.elapsed()<<" ms"<<std::endl;
}

void Yuan_pattern::decode_threefrequencyHeterodyne(cv::Mat mask,std::vector<cv::Mat> &srcImageSequence,int stepNum,std::vector<cv::Mat> &dstImageSequence,cv::Mat &unwrap_x,cv::Mat &unwrap_y)
{
    QTime t1;
    t1.start();

    int imageHeight=srcImageSequence[0].rows;
    int imageWidth =srcImageSequence[0].cols;

    //解相对相位

    std::vector<cv::Mat> x_mat;
    std::vector<cv::Mat> y_mat;

    unsigned int totalNum=stepNum*6;

    for(int i= 0;i< totalNum/2;i+=stepNum)
    {
        cv::Mat result;
        std::vector<cv::Mat> images;
        images.push_back(srcImageSequence[i]);
        images.push_back(srcImageSequence[i+1]);
        images.push_back(srcImageSequence[i+2]);
        if(stepNum==4) images.push_back(srcImageSequence[i+3]);

        decode_sinusoidal(mask,images,result,stepNum);

        cv::Mat XsinWrapPhase=cv::Mat::zeros(imageHeight,imageWidth,CV_64FC1);
        result.convertTo(XsinWrapPhase,CV_64FC1,2*CV_PI,0);

        x_mat.push_back(XsinWrapPhase);//横向相位

        dstImageSequence.push_back(XsinWrapPhase);//输出图片
    }
    for(int i= totalNum/2; i < totalNum ;i+=stepNum)
    {
        cv::Mat result;
        std::vector<cv::Mat> images;
        images.push_back(srcImageSequence[i]);
        images.push_back(srcImageSequence[i+1]);
        images.push_back(srcImageSequence[i+2]);
        if(stepNum==4) images.push_back(srcImageSequence[i+3]);

        decode_sinusoidal(mask,images,result,stepNum);

        cv::Mat YsinWrapPhase=cv::Mat::zeros(imageHeight,imageWidth,CV_64FC1);
        result.convertTo(YsinWrapPhase,CV_64FC1,2*CV_PI,0);

        y_mat.push_back(YsinWrapPhase);

        dstImageSequence.push_back(YsinWrapPhase);
    }

    //解绝对
    decode_threefrequency(mask,x_mat,unwrap_x);
    decode_threefrequency(mask,y_mat,unwrap_y);

    dstImageSequence.push_back(unwrap_x);
    dstImageSequence.push_back(unwrap_y);

    std::cout<<"complete the decoding ... "<<t1.elapsed()<<" ms"<<std::endl;
}

bool Yuan_pattern::wrap_fourier_transform(const cv::Mat &src, cv::Mat &dst, kernel _kernel,int center_x,int radius)
{
    if(src.channels()!=1)
    {
        std::cout<<"Error: the channel is not correct ... \n";
        return false;
    }
    cv::Mat srcGray=src.clone();
//    cv::imwrite("../ScanData/pattern.bmp",srcGray);
    cv::imshow("original pattern",srcGray);

    int m = cv::getOptimalDFTSize(srcGray.rows); //2,3,5的倍数有更高效率的傅里叶变换
    int n = cv::getOptimalDFTSize(srcGray.cols);

    cv::Mat padded;
    //把灰度图像放在左上角,在右边和下边扩展图像,扩展部分填充为0;
    cv::copyMakeBorder(srcGray, padded, 0, m - srcGray.rows,0, n - srcGray.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    padded.convertTo(padded,CV_64FC1);

    //这里是获取了两个Mat,一个用于存放dft变换的实部，一个用于存放虚部,初始的时候,实部就是图像本身,虚部全为零
    cv::Mat planes[] = {padded, cv::Mat::zeros(padded.size(),CV_64F)};
    cv::Mat complexImg;
    //将几个单通道的mat融合成一个多通道的mat,这里融合的complexImg既有实部又有虚部
    cv::merge(planes,2,complexImg);

    //一维
    cv::Mat image = complexImg.rowRange(200,201);
    std::cout<<"image row: "<<image.rows<<" image col: "<<image.cols<<std::endl;
    cv::dft(image,image);
    int cx = image.cols/2;
    cv::Mat q0(image, cv::Rect(0,0,cx,1));
    cv::Mat q1(image, cv::Rect(cx,0,cx,1));
    cv::Mat tmp;
    q0.copyTo(tmp);
    q1.copyTo(q0);
    tmp.copyTo(q1);
    std::vector<cv::Mat> real_img_mat;
    cv::split(image,real_img_mat);
    cv::Mat mag=cv::Mat::zeros(image.rows,image.cols,CV_32FC1);
    cv::magnitude(real_img_mat[0],real_img_mat[1],mag);//幅值
    //=> log(1 + sqrt(Re(DFT(I))^2 +Im(DFT(I))^2))
    mag += cv::Scalar::all(1);
    cv::log(mag, mag);
    cv::normalize(mag,mag,0,1,cv::NORM_MINMAX);
    cv::imshow("dft_frequency_spectrum_pattern", mag);
    image_paint.push_back(mag);

    //二维
//    //对上边合成的mat进行傅里叶变换,傅里叶变换结果为复数.通道1存的是实部,通道二存的是虚部
//    cv::dft(complexImg,complexImg);

//    int cx = complexImg.cols/2;
//    int cy = complexImg.rows/2;
//    cv::Mat q0(complexImg, cv::Rect(0,0,cx,cy));    // Top-Left
//    cv::Mat q1(complexImg, cv::Rect(cx,0,cx,cy));   // Top-Right
//    cv::Mat q2(complexImg, cv::Rect(0,cy,cx,cy));   // Bottom-Left
//    cv::Mat q3(complexImg, cv::Rect(cx,cy,cx,cy));  // Bottom-Right
//    cv::Mat tmp;
//    q0.copyTo(tmp);
//    q3.copyTo(q0);
//    tmp.copyTo(q3);
//    q1.copyTo(tmp);
//    q2.copyTo(q1);
//    tmp.copyTo(q2);

//    cv::split(complexImg,planes);
//    cv::Mat mag=cv::Mat::zeros(padded.rows,padded.cols,CV_32FC1);
//    cv::magnitude(planes[0],planes[1],mag);//幅值
//    //=> log(1 + sqrt(Re(DFT(I))^2 +Im(DFT(I))^2))
//    mag += cv::Scalar::all(1);
//    cv::log(mag, mag);
//    cv::normalize(mag,mag,0,1,cv::NORM_MINMAX);
//    cv::imshow("dft_frequency_spectrum_pattern", mag);
//    image_paint.push_back(mag);

//    //******************************频域滤波***********************//
//        cv::Mat gaussianBlur(padded.size(),CV_64FC2);

//        float D0=2*3*3;

//        int y=floor((double)padded.rows/2);
//        int x=floor((double)padded.cols/2)+center_x;

//        unsigned int i=0,j=0;

//        for(i=0;i<padded.rows;++i)
//        {
//            double*p=gaussianBlur.ptr<double>(i);

//            for(j=0;j<padded.cols;++j)
//            {
//                float d=pow(i-padded.rows/2,2)+pow(j-padded.cols/2,2);

//                switch (_kernel) {
//                case Gauss:
//                {
//                    //高斯低通滤波
//                        p[2*j]  =expf(-d/D0);
//                        p[2*j+1]=expf(-d/D0);
//                }
//                    break;
//                case Butterworth:
//                {
//                    //巴特沃斯低通滤波
//                      p[2*j]  =1/(1+pow(d/D0,20));
//                      p[2*j+1]=1/(1+pow(d/D0,20));
//                }
//                    break;
//                case Circle:
//                {
//                    if( sqrt(pow(i-y,2) + pow(j-x,2) ) < radius)
//                    {
//                        p[2*j]  =1;
//                        p[2*j+1]=1;
//                    }else{
//                        p[2*j]  =0;
//                        p[2*j+1]=0;
//                    }
//                }
//                    break;

//                }
//            }
//        }
//        cv::multiply(complexImg,gaussianBlur,gaussianBlur);

//        cv::split(gaussianBlur,planes);
//        cv::Mat mag1=cv::Mat::zeros(padded.rows,padded.cols,CV_32FC1);
//        cv::magnitude(planes[0],planes[1],mag1);//幅值
//        mag1 += cv::Scalar::all(1);
//        cv::log(mag1, mag1);
//        cv::normalize(mag1,mag1,0,1,cv::NORM_MINMAX);
//        cv::imshow("filter_frequency_spectrum_pattern", mag1);

//        image_paint.push_back(mag1);
//    //********************************************************//

//    int cx1 = gaussianBlur.cols/2;
//    int cy1 = gaussianBlur.rows/2;
//    cv::Mat p0(gaussianBlur, cv::Rect(0,0,cx1,cy1));    // Top-Left
//    cv::Mat p1(gaussianBlur, cv::Rect(cx1,0,cx1,cy1));   // Top-Right
//    cv::Mat p2(gaussianBlur, cv::Rect(0,cy1,cx1,cy1));   // Bottom-Left
//    cv::Mat p3(gaussianBlur, cv::Rect(cx1,cy1,cx1,cy1));  // Bottom-Right
//    cv::Mat tmp1;
//    p0.copyTo(tmp1);
//    p3.copyTo(p0);
//    tmp1.copyTo(p3);
//    p1.copyTo(tmp1);
//    p2.copyTo(p1);
//    tmp1.copyTo(p2);

//    cv::Mat ifft;
//    cv::idft(gaussianBlur,ifft);
//      std::cout<<"the channel of ifft : "<<ifft.channels()<<std::endl;
//    dst=ifft.clone();
    return true;
}

void Yuan_pattern::decode_multi_step_phaseShift(std::vector<cv::Mat> &src, cv::Mat &dst, int stepNum,int phaseShitf, const cv::Mat &mask)
{

    if(src.size()!=stepNum)
    {
        std::cout<<"Error: the size of src is not equal to stepNum ... \n";
        return;
    }
    QTime t1;
    t1.start();

    int ImageHeight=src[0].rows;
    int ImageWidth =src[0].cols;

    double numerator=0;
    double denominator=0;
    double coefficient = phaseShitf*CV_PI/stepNum;

    if(src[0].isContinuous() && dst.isContinuous() && mask.isContinuous() )
    {
        ImageWidth *= ImageHeight;
        ImageHeight=1;
    }

    for(int i=0;i<ImageHeight;++i)
    {
        double* result_ptr=dst.ptr<double>(i);
        const uchar* mask_ptr= mask.ptr<uchar>(i);

        for(int j=0;j<ImageWidth;++j)
        {
            if(mask_ptr[j]!=0)
            {
                numerator=0;
                denominator=0;
                for(unsigned int n=0;n<stepNum;++n)
                {             
                    uchar *ptr=src[n].ptr<uchar>(i);
                    numerator += ptr[j]*sin(n*coefficient);
                    denominator += ptr[j]*cos(n*coefficient);
                }

                result_ptr[j] = -atan2(numerator,denominator)+CV_PI;//+CV_PI;
            }

        }

    }
    //cv::normalize(dst,dst,2*CV_PI,0.0,cv::NORM_MINMAX);
    std::cout<<"the time of multi_step_phaseShift is "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::decode_grayCode(std::vector<cv::Mat> &src, cv::Mat &dst, const cv::Mat &mask)
{
    if(src.size()!=4)
    {
        std::cout<<"Error: the num of gray_code patterns is not correct ... \n";
        return;
    }
    QTime t1;
    t1.start();

    unsigned int imageHeight = src[0].rows;
    unsigned int imageWidth = src[0].cols;

    if(src[0].isContinuous())
    {
        imageWidth=imageHeight*imageWidth;
        imageHeight=1;
    }

    for(unsigned int i=0;i<imageHeight;++i)
    {
        uchar* Ptr1=src[0].ptr<uchar>(i);
        uchar* Ptr2=src[1].ptr<uchar>(i);
        uchar* Ptr3=src[2].ptr<uchar>(i);
        uchar* Ptr4=src[3].ptr<uchar>(i);
        const uchar* maskPtr = mask.ptr<uchar>(i);
        uchar* resultPtr = dst.ptr<uchar>(i);

        for(unsigned int j=0;j<imageWidth;++j)
        {
            if(maskPtr[j]!=0)
            {
                Ptr1[j] = Ptr1[j] > Ptr3[j] ? 1 : 0;
                Ptr2[j] = Ptr2[j] > Ptr4[j] ? 1 : 0;
                Ptr2[j] = Ptr2[j] ^ Ptr1[j];
                resultPtr[j]=2 * Ptr1[j] + 1 * Ptr2[j];
            }
        }

    }
    std::cout<<"decode_grayCode consumed "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::decode_multi_grayCode(std::vector<cv::Mat> &src, cv::Mat &dst, const cv::Mat &mask)
{
    if(src.size()!=8)
    {
        std::cout<<"Error: the num of gray_code patterns is not correct ... \n";
        return;
    }
    QTime t1;
    t1.start();

    unsigned int imageHeight = src[0].rows;
    unsigned int imageWidth = src[0].cols;
    cv::Mat result = cv::Mat::zeros(imageHeight,imageWidth,CV_8UC1);

    if(src[0].isContinuous())
    {
        imageWidth=imageHeight*imageWidth;
        imageHeight=1;
    }

    for(unsigned int i=0;i<imageHeight;++i)
    {
        uchar* Ptr1=src[0].ptr<uchar>(i);
        uchar* Ptr2=src[1].ptr<uchar>(i);
        uchar* Ptr3=src[2].ptr<uchar>(i);
        uchar* Ptr4=src[3].ptr<uchar>(i);
        uchar* Ptr5=src[4].ptr<uchar>(i);
        uchar* Ptr6=src[5].ptr<uchar>(i);
        uchar* Ptr7=src[6].ptr<uchar>(i);
        uchar* Ptr8=src[7].ptr<uchar>(i);
        const uchar* maskPtr = mask.ptr<uchar>(i);
        uchar* resultPtr = result.ptr<uchar>(i);

        for(unsigned int j=0;j<imageWidth;++j)
        {
            if(maskPtr[j]!=0)
            {
                Ptr1[j] = Ptr1[j] > Ptr5[j] ? 1 : 0;
                Ptr2[j] = Ptr2[j] > Ptr6[j] ? 1 : 0;
                Ptr3[j] = Ptr3[j] > Ptr7[j] ? 1 : 0;
                Ptr4[j] = Ptr4[j] > Ptr8[j] ? 1 : 0;

                Ptr2[j] = Ptr2[j] ^ Ptr1[j];
                Ptr3[j] = Ptr3[j] ^ Ptr2[j];
                Ptr4[j] = Ptr4[j] ^ Ptr3[j];

                resultPtr[j]= 8 * Ptr1[j] + 4 * Ptr2[j] + 2 * Ptr3[j] + 1 * Ptr4[j];
            }
        }

    }
    dst=result.clone();
    std::cout<<"decode_grayCode consumed "<<t1.elapsed()<<" ms\n";
}

double Yuan_pattern::gamma_active_calibration(std::vector<cv::Mat> &phaseSequence,std::vector<double> &diff, int rowNum)
{
    double *gamma;
    double maxValue=0;

    int col=phaseSequence[0].cols;

    std::vector<double> temp_phaseDifference;
    temp_phaseDifference.reserve(col+1);

    double *phaseReal=phaseSequence[0].ptr<double>(rowNum);
    double *phaseImg=phaseSequence[1].ptr<double>(rowNum);

    for(unsigned int j=0;j<col;++j)
    {
        double difference=abs(phaseReal[j]-phaseImg[j]);
        temp_phaseDifference.push_back(difference);
    }
    std::sort(temp_phaseDifference.begin(),temp_phaseDifference.end(),std::less<double>() );
    maxValue=temp_phaseDifference[col-1];

    diff.insert(diff.end(),temp_phaseDifference.begin(),temp_phaseDifference.end());
    std::cout<<"the max value of phase difference: "<<maxValue<<std::endl;

    double G = abs( sin(maxValue) );

    gamma=quadratic_equation_of_one_variable( 1-G ,-5*G-3 ,2-9*G );

    std::cout<<"the gamma[0] : "<<gamma[0]<<" , the gamma[1] : "<<gamma[1]<<"\n";

    double gammaValue =0 ;
    if(gamma[0]>=2 && gamma[0]<=5)
    {
        gammaValue=gamma[0];
    }
    else if(gamma[1]>=2 && gamma[1]<=5)
    {
        gammaValue=gamma[1];
    }
    else
    {
        gammaValue = (gamma[0] > gamma[1] ) ? gamma[0]:gamma[1];
        //std::cout<<"Error: the gamma result is not correct ... \n";
    }

    return gammaValue;

    delete gamma;
}

double *Yuan_pattern::quadratic_equation_of_one_variable(double a, double b, double c)
{
    double d,m,n;
    double *x = new double[2];
    double esp=pow(10,-7);

    d=b*b-4*a*c;

    if(d==0||d>0)
    {
        if(d==0)
        {
            x[0]=x[1]=(-b+sqrt(d))/(2*a);
            std::cout<<"x1 = x2 = "<<x[0]<<std::endl;
        }
        else
        {
            x[0]=(-b+sqrt(d))/(2*a);
            x[1]=(-b-sqrt(d))/(2*a);
            std::cout<<"x1 = "<<x[0]<<" , x2 = "<< x[1]<<std::endl;
        }
    }
    else
    {
        m=-b/(2*a)+esp;
        n=sqrt(-d)/(2*a);
        std::cout<<"x1 = "<<m<<"+"<<n<<"i"<<" , x2 = "<<m<<"-"<<n<<"i"<<std::endl;
        std::cout<<"the result is  complex number ... "<<std::endl;
        return NULL;
    }

    return x;

}

hash_table Yuan_pattern::create_LookUpTable(std::vector<cv::Mat> &phase,cv::Mat &mask)
{
    if(phase.empty())
    {
        std::cout<<"\nthe phase vector is empty ... \n";
        return NULL;
    }

    std::ofstream txtFile;
    txtFile.open("../ScanData/pointXY.txt");

    unsigned int imageRow=phase[0].rows;
    unsigned int imageCol=phase[0].rows;

//    std::vector<int> minLoc;
//    cv::Mat rowPhase;

//        for(unsigned int n=0;n<rowPhase.cols;++n)
//        {
//            double *ptr1=rowPhase.ptr<double>(0);

//            if( n>10
//                && ptr1[n] < ptr1[n-1] && ptr1[n] < ptr1[n-2] && ptr1[n] < ptr1[n-3] && ptr1[n] < ptr1[n-4] && ptr1[n] < ptr1[n-5]
//                    && ptr1[n] < ptr1[n-6] && ptr1[n] < ptr1[n-7] && ptr1[n] < ptr1[n-8] && ptr1[n] < ptr1[n-9] && ptr1[n] < ptr1[n-10]
//                && ptr1[n] < ptr1[n+1] && ptr1[n] < ptr1[n+2] && ptr1[n] < ptr1[n+3] && ptr1[n] < ptr1[n+4] && ptr1[n] < ptr1[n+5]
//                    && ptr1[n] < ptr1[n+6] && ptr1[n] < ptr1[n+7] && ptr1[n] < ptr1[n+8] && ptr1[n] < ptr1[n+9] && ptr1[n] < ptr1[n+10]
//                    )
//            {
//                minLoc.push_back(n);
//            }

//        }
//    std::cout<<"\nminLoc: "<<std::endl;
//    for(unsigned int n=0;n<minLoc.size();++n){
//        std::cout<<minLoc[n]<<" ";
//    }
//    std::cout<<std::endl;

//    if(minLoc.size()<2){
//        std::cout<<"\nError: the number of minimal value is not enough ... \n";
//        return NULL;
//    }

//    int p1TableFirst = minLoc[minLoc.size()/2-1];
//    int p1TableLast  = minLoc[minLoc.size()/2];
//    std::cout<<"p1TableFirst : "<<p1TableFirst<<" , p1TableLast : "<<p1TableLast<<std::endl;

//    if(p1TableFirst < 0 || p1TableLast > rowPhase.cols ){
//        std::cout<<"\nError: the TableLast is bigger than rowPhase.cols ... \n";
//        return NULL;
//    }

    std::vector<double> p1_difference;
    std::vector<double> p1_phase;

    pointXY tempPoint;

    double difference=0;

    point_paint.clear();
    for(unsigned int i=0;i<imageRow;i+=150)
    {
        uchar* maskPtr=mask.ptr<uchar>(i);
        double* realPtr=phase[1].ptr<double>(i);
        double* actualPtr=phase[0].ptr<double>(i);

        for(unsigned int j=0; j < imageCol;++j)
        {
            if(maskPtr[j]!=0)
            {
                difference=realPtr[j]-actualPtr[j];
                if(abs(difference)<1)
                {
                    tempPoint.x = actualPtr[j];
                    tempPoint.y = difference;
                    point_paint.push_back(tempPoint);

                    p1_difference.push_back(difference);
                    p1_phase.push_back(actualPtr[j]);

                    txtFile<<actualPtr[j]<<" "<<difference<<"\n";
                }
            }
        }
    }

    txtFile.close();

        ////*************************multipleLinearRegression******************////

    double Aconst;
    std::vector<double> Bcoefficient;

//    p1_phase.clear();
//    p1_difference.clear();
//    for(int i=0;i<100;++i)
//    {
//        p1_phase.push_back(i);
//        p1_difference.push_back(pow(i,2));
//    }

    if(!multipleLinearRegression(p1_phase,p1_difference, 7 ,Aconst,Bcoefficient))
    {
        std::cout<<"\nError: Failed to finish the Linear Regression ...\n";
        return NULL;
    }

//    pointXY temp;
//    point_paint.clear();
//    for(int i=0;i<100;++i)
//    {
//        temp.x = i;
//        temp.y = Aconst+Bcoefficient[0]*pow(i,2);
//        point_paint.push_back(temp);
//    }

    double value=0;
    double tempPhase=0;
    hash_table temp_table;

    for(int n=0;n < 256;++n)
    {
        tempPhase = n*2*CV_PI/256;
        value = fitting_polynomial(Aconst,Bcoefficient,tempPhase);
        if(value>1)value=0;
        temp_table.insert(n,value);
    }

    /*********************************************/

    //temp_table.show();

    return temp_table;

    //return NULL;

}

void Yuan_pattern::correct_phase(cv::Mat &src, cv::Mat &dst, const cv::Mat &mask,int Num)
{
    QTime t1;
    t1.start();

    if(src.type()!= CV_64FC1)
    {
        std::cout<<"\nthe type of the input image is not correct ... "<<std::endl;
        return;
    }

    unsigned int imageRow=src.rows;
    unsigned int imageCol=src.cols;

    hash_table errorTable;
    if(Num==1)errorTable=phaseError_table;
    else if(Num==2)errorTable=table_p2;
    else errorTable=table_p3;

    int key=0;

    cv::Mat result=cv::Mat::zeros(imageRow,imageCol,CV_64FC1);
    if(src.isContinuous())
    {
        imageCol=imageRow*imageCol;
        imageRow=1;
    }

    for(unsigned int i=0;i<imageRow;++i)
    {

        const uchar *maskPtr=mask.ptr<uchar>(i);
        double *srcPtr=src.ptr<double>(i);
        double *resultPtr=result.ptr<double>(i);

        for(unsigned int j=0;j<imageCol;++j)
        {
            key = (int)(srcPtr[j]*256/(2*CV_PI));

            if(maskPtr[j]!=0)
            {
                resultPtr[j] = srcPtr[j] + errorTable.find(key);
            }
        }
    }

    dst=result.clone();

    std::cout<<"Founction: correct_phase  takes time : "<<t1.elapsed()<<" ms"<<std::endl;
}

bool Yuan_pattern::multipleLinearRegression(const std::vector<double> &x, const std::vector<double> &y, int numRegression, double &A_const, std::vector<double> &B_coefficient)
{
    if(x.empty() || y.empty() )
    {
        std::cout<<"\nError: the x or y is empty ... \n";
        return false;
    }
    try
    {
        MultipleLinearRegression mMultipleLinearRegression(numRegression);

        unsigned int exp_size=y.size();
        std::cout<<"exemple size :"<<exp_size<<std::endl;
        std::vector<double> vectorTrain;
        vectorTrain.reserve(21);

        for(unsigned int i=0;i<exp_size;++i)
        {
           vectorTrain.clear();
           for(unsigned int j=1;j<=numRegression;++j)
           {
               vectorTrain.push_back( pow(x[i],j) );
           }
           mMultipleLinearRegression.addXY(vectorTrain,y[i]);
        }
        if(mMultipleLinearRegression.isValid())
        {
            QString nameA,nameB;
            nameA="A : ";
            nameB="B";

            A_const= mMultipleLinearRegression.getA();
            std::cout << nameA.toStdString() << mMultipleLinearRegression.getA() << std::endl;

            B_coefficient = mMultipleLinearRegression.getB();
            for(int i = 0; i < B_coefficient.size(); ++i)
            {
                std::cout << nameB.toStdString() << i+1 << ": " << B_coefficient.at(i) << std::endl;
            }
        }
        return true;
    }
    catch(std::string error)
    {
        std::cout << error << std::endl;
        return false;
    }
}

double Yuan_pattern::fitting_polynomial(const double &A,const std::vector<double> &B,const double &x)
{
    double result=A;

    for(unsigned int i=1;i <= B.size();++i)
    {
        result += B[i-1] * pow( x ,i );
    }
    return result;
}

void Yuan_pattern::difference_phase(const cv::Mat &original, const cv::Mat &real,int rowNum, std::vector<double> &diff,const cv::Mat &mask)
{

    diff.clear();
    int count = 0;
//    double* oriPtr = original.ptr<double>(rowNum);
//    double* realPtr = real.ptr<double>(rowNum);
    std::cout<<"original.cols : "<<original.cols<<std::endl;
    for(unsigned int i=0;i<original.cols;++i)
    {

        double difference =0;
        if(mask.at<uchar>(rowNum,i)!=0){
            count++;
            difference = original.at<double>(rowNum,i) - real.at<double>(rowNum,i);
            //if(abs(difference)<2)
            diff.push_back(difference);
        }

    }
    std::cout<<"count : "<<count<<std::endl;

}

void Yuan_pattern::difference_phase_pointXY(cv::Mat &original, cv::Mat &real, int rowNum, std::vector<pointXY> &diff,cv::Mat &mask)
{
    double difference =0;
    int nc = original.cols;

    pointXY tempPoint;
    diff.clear();

    double* oriPtr = original.ptr<double>(rowNum);
    double* realPtr = real.ptr<double>(rowNum);
    uchar* maskPtr = mask.ptr<uchar>(rowNum);
    for(unsigned int i=0;i<nc;++i)
    {
        if(maskPtr[i]!=0){
            difference = oriPtr[i] - realPtr[i];
            if(abs(difference)<2)
            {
                tempPoint.x = realPtr[i];
                tempPoint.y = difference;
                diff.push_back(tempPoint);
            }
        }
    }
}

void Yuan_pattern::meanValue_gradual_gray(const std::vector<cv::Mat> &srcImages, std::vector<double> &result)
{
    QTime t1;
    t1.start();

    for(unsigned int n = 0;n<srcImages.size();++n)
    {
        double mean = 0;
        unsigned int count = 0;
        double sum = 0;

        std::vector<cv::Mat> patterns;
        patterns.reserve(4);
        cv::split(srcImages[n],patterns);

        for(unsigned int i = 500;i<700;++i)
        {
            const uchar *ptr = patterns[0].ptr<uchar>(i);
            for(unsigned int j = 500;j<7000;++j)
            {
                sum += ptr[j];
                count++;
            }
        }
        mean = sum/count;
        result.push_back(mean);
    }
    std::cout<<"meanValue_gradual_gray consumed : "<<t1.elapsed()<<" ms ... \n";
}

void Yuan_pattern::build_gamma_Table(const double &gammaA, const std::vector<double> &gammaB, int numRegression)
{
    QTime t1;
    t1.start();

    gammaTable = new double[256];
    for(int i = 0; i < 256; ++i){
        gammaTable[i] = gammaA ;
        for(int n = 0; n < numRegression; n++){
            gammaTable[i] += gammaB[n]*pow(i,n+1);                      
        }
        if(gammaTable[i]<0)gammaTable[i] = 0;
        else if(gammaTable[i]>255)gammaTable[i] = 255;

        if(i%5==0)std::cout<< i <<" , "<<gammaTable[i]<<std::endl;
    }

    std::cout<<"build_gamma_Table consumed : "<<t1.elapsed()<<" ms"<<std::endl;
}

void Yuan_pattern::decode_threefrequency(cv::Mat mask,std::vector<cv::Mat> &srcImageSequence,cv::Mat &unwrap,int stepNum)
{
    QTime t1;
    t1.start();

    if(srcImageSequence.empty()){
        std::cout<<"Decode Error: the image is empty ... \n";
        return ;
    }

    unsigned int imageHeight = srcImageSequence[0].rows;
    unsigned int imageWidth = srcImageSequence[0].cols;

    unwrap = cv::Mat::zeros(imageHeight,imageWidth,CV_64FC1);

    cv::Mat image_p1= srcImageSequence[0];
    cv::Mat image_p2= srcImageSequence[1];
    cv::Mat image_p3= srcImageSequence[2];

    double P1 = wavelength_P1;
    double P2 = wavelength_P2;
    double P3 = wavelength_P3;

    double p12 = (double)P2*P1/(P2- P1);
    double c3 = p12/(P3-p12);
    double cv2PI = CV_PI*2;

    if(image_p1.isContinuous())
    {
        if(image_p2.isContinuous())
        {
            if(image_p3.isContinuous())
            {
                if(mask.isContinuous())
                {
                    columns = rows*columns;
                    rows = 1;
                }
            }
        }
    }

    for(unsigned int i= 0;i< imageHeight; ++i)
    {
        double* ptr_w1 = image_p1.ptr<double>(i);
        double* ptr_w2 = image_p2.ptr<double>(i);
        double* ptr_w3 = image_p3.ptr<double>(i);
        const uchar *ptr_mask = mask.ptr<uchar>(i);
        double* unwrapPtr = unwrap.ptr<double>(i);

        for(unsigned int j= 0;j< imageWidth; ++j)
        {
           if(ptr_mask[j]!=0 )
           {
               //包裹相位1、2叠加
               //解包裹相位wrap_3，1、直接按比例换算123-3，2、计算出k值再换算，3、两者综合作误差补偿
               double delta_w1 = ptr_w1[j]/cv2PI;
               double delta_w2 = ptr_w2[j]/cv2PI;
               double delta_w3 = ptr_w3[j]/cv2PI;
               double delta_w12 = ( delta_w2 <= delta_w1 )? delta_w1 - delta_w2 : delta_w1 - delta_w2 +1 ;
               double delta_w123 = ( delta_w3 <= delta_w12 )? delta_w12 - delta_w3 : delta_w12 - delta_w3 +1 ;
               /***************************************************************************************/

               //比例展开wrap3，小跃变
               double value_unw3 = cv2PI * c3 * delta_w123;

               //取整展开wrap3,大跃变
               double value_un_k3= cv2PI * floor(c3*delta_w123)+ ptr_w3[j];

               //结合比例展开、k值展开部分进行相位补偿comp_unwrap_3
               double value_un_err3= value_un_k3 - value_unw3;

               if( CV_PI < value_un_err3)
               {
                    value_un_k3 -= cv2PI;
               }

               if( -CV_PI > value_un_err3)
               {
                    value_un_k3 += cv2PI;
               }

               double correct_unwrap3 = value_un_k3;

               //理论上的unwrap1
               double value_unw1= correct_unwrap3*P3/P1;
               //校正
               double k1 = round((value_unw1 - ptr_w1[j])/cv2PI);

               unwrapPtr[j] = cv2PI * k1 + ptr_w1[j];
              // unwrapPtr[j] = value_unw3;
          }

        }
    }

    std::cout<<"decode_threefrequency consumed "<<t1.elapsed()<<" ms\n";

}

void Yuan_pattern::decode_doublefrequencyHeterodyne(cv::Mat &mask, std::vector<cv::Mat> &srcImageSequence, int T1, int T2, cv::Mat &unwrap,Direction _direction)
{
    QTime t1;
       t1.start();

       int imageHeight = srcImageSequence[0].rows;
       int imageWidth  = srcImageSequence[0].cols;

       double ratio_T2 = T2/(T2-T1);
       double ratio_T1 = T1/(T2-T1);
       double my2PI = CV_PI*2;
       double CV_PIperStep = CV_PI/5;

   //    if(srcImageSequence[0].isContinuous()){
   //        if(srcImageSequence[1].isContinuous()){
   //            if(mask.isContinuous()){
   //                if(unwrap.isContinuous()){
   //                    imageWidth *= imageHeight;
   //                    imageHeight = 1;
   //                }
   //            }
   //        }
   //    }

       unsigned int i,j;
       cv::Mat unwrap1 = cv::Mat::zeros(imageHeight,imageWidth,CV_64FC1);
       cv::Mat unwrap2 = cv::Mat::zeros(imageHeight,imageWidth,CV_64FC1);
       for( i = 0; i < imageHeight; ++i){

           uchar* maskPtr = mask.ptr<uchar>(i);
           double* phase1Ptr = srcImageSequence[0].ptr<double>(i);
           double* phase2Ptr = srcImageSequence[1].ptr<double>(i);
           double* phase3Ptr = srcImageSequence[2].ptr<double>(i);
           double* phase4Ptr = srcImageSequence[3].ptr<double>(i);
           double* unwrapPtr = unwrap.ptr<double>(i);
           double* unwrapPtr1 = unwrap1.ptr<double>(i);
           double* unwrapPtr2 = unwrap2.ptr<double>(i);

           for( j = 0; j < imageWidth; ++j){

               if(maskPtr[j]!=0){

//                   double value_w12 = (phase1Ptr[j] - phase2Ptr[j])/my2PI;
//                   double value_w34 = (phase3Ptr[j] - phase4Ptr[j])/my2PI;
//                   if(value_w12 < 0) value_w12 += 1;
//                   if(value_w34 < 0) value_w34 += 1;

//                   double tempUnwrap1 = my2PI * value_w12 * ratio_T2 ;
//                   double tempUnwrap2 = my2PI* floor(value_w12 * ratio_T2) + phase1Ptr[j];
//                   double difference12 = tempUnwrap2 - tempUnwrap1;

//                   if(difference12 > CV_PI/4) tempUnwrap2 -= my2PI;
//                   if(difference12 < -CV_PI) tempUnwrap2 += CV_PI;

//                   double tempUnwrap3 = my2PI * value_w34 * ratio_T2 ;
//                   double tempUnwrap4 = my2PI* floor(value_w34 * ratio_T2) + phase3Ptr[j];
//                   double difference34 = tempUnwrap4 - tempUnwrap3;

//                   if(difference34 > CV_PI/3) tempUnwrap4 -= my2PI;
//                   if(difference34 < -CV_PI) tempUnwrap4 += CV_PI;

//                   double unwrap3 = (tempUnwrap1+tempUnwrap3)/2;


                   /********************************************/
                  double phasef1 = (phase1Ptr[j] <= phase3Ptr[j]) ? (phase1Ptr[j] + phase3Ptr[j] - CV_PIperStep)*0.5 : (phase1Ptr[j] + phase3Ptr[j] - CV_PIperStep + my2PI)*0.5 ;
                  double phasef2 = (phase2Ptr[j] <= phase4Ptr[j]) ? (phase2Ptr[j] + phase4Ptr[j] - CV_PIperStep)*0.5 : (phase2Ptr[j] + phase4Ptr[j] - CV_PIperStep + my2PI)*0.5 ;
                  phasef1 += CV_PI;
                  phasef2 += CV_PI;

                  double value_w12 = (phasef1 - phasef2)/my2PI;
                  if(value_w12 < 0) value_w12 += 1;

                  double tempUnwrap1 = my2PI * value_w12 * ratio_T2;
                  double tempUnwrap2 = my2PI * (floor(value_w12 * ratio_T2 + 100)-100) + phasef1;
                  double difference = tempUnwrap2 - tempUnwrap1;

                  double unwrap3 = tempUnwrap2;
                  ////彩色复合
                  if(_direction==Vertical){
                      if(difference > CV_PI/1.5) unwrap3 -= my2PI;
                      if(difference < -CV_PI*1.05) unwrap3 += my2PI;
                  }else{
                      if(difference > CV_PI/2) unwrap3 -= my2PI;
                      if(difference < -CV_PI) unwrap3 += my2PI;
                  }

                  ////彩色双三步
//                    if(difference > CV_PI/4) unwrap3 -= my2PI;
//                    if(difference < -my2PI) unwrap3 += my2PI;

                   /**********************************************/

                  unwrapPtr1[j] = tempUnwrap1;
                  unwrapPtr2[j] = tempUnwrap2;
                  unwrapPtr[j] = unwrap3;

//                  unwrapPtr1[j] = phase1Ptr[j];
//                  unwrapPtr2[j] = phase3Ptr[j];
//                  unwrapPtr[j] = phasef1;
                  }

            }
       }
       image_paint.push_back(unwrap1);
       image_paint.push_back(unwrap2);
       image_paint.push_back(unwrap);
       std::cout<<"decode_doublefrequencyHeterodyne consumed "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::createCorrectPoints(cv::Mat unwrapX,cv::Mat unwrapY,int pixels
                                       ,std::vector<cv::Point2f> &l_points, std::vector<cv::Point2f> &r_points,cv::Mat mask)
{
    QTime t1;
    t1.start();

    l_points.clear();
    r_points.clear();

    int nr= unwrapX.rows;
    int nc= unwrapX.cols;

    double pixelsPerTwoPi =  pixels/(2*CV_PI);

    double dlp_x_value = 0,dlp_y_value = 0;

    for(int r= 0;r< nr; ++r)
    {

        double* x_ptr= unwrapX.ptr<double>(r);
        double* y_ptr= unwrapY.ptr<double>(r);
        uchar*  m_ptr= mask.ptr<uchar>(r);

        for(int c= 0;c< nc; ++c)
        {

            if(m_ptr[c]!= 0)
            {

                    dlp_x_value= x_ptr[c]*pixelsPerTwoPi;
                    dlp_y_value= y_ptr[c]*pixelsPerTwoPi;

                    l_points.push_back(cv::Point2f(c,r));
                    r_points.push_back(cv::Point2f(dlp_x_value,dlp_y_value));

                   //cv::undistortPoints(l_points, l_points, left_intrinsic, left_distCoeffs, cv::noArray(), cv::noArray());
                   //cv::undistortPoints(r_points, r_points, right_intrinsic, right_distCoeffs, cv::noArray(), cv::noArray());
             }

         }

    }
    std::cout<<"createCorrectPoints consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::reconstruct3D(std::vector<cv::Point2f> leftPoints, std::vector<cv::Point2f> rightPoints, std::vector<cv::Point3f> &pCloud)
{
    QTime t1;
    t1.start();

    int pointsNum= leftPoints.size();
    pCloud.clear();

#if 1
    cv::Mat A1(3,3,CV_64F),A2(3,3,CV_64F);//逆矩阵
    cv::Mat d1,d2;//常数项
    cv::invert(M_1.colRange(0,3),A1);
    cv::invert(M_2.colRange(0,3),A2);
    M_1.col(3).copyTo(d1);
    M_2.col(3).copyTo(d2);
    cv::Mat Ad1=A1*d1;
    cv::Mat Ad2=A2*d2;
    cv::Mat AD=Ad1-Ad2;
    double *pA1=(double *)A1.data;
    double *pA2=(double *)A2.data;
    double *pAd1=(double*)Ad1.data;
    double *pAd2=(double*)Ad2.data;
    double *pAD=(double*)AD.data;
    //会变化的变量
    double res1[3];
    double res2[3];
    double PRES[3];
    double b1[3];
    double b2[3];
    double B1[3];
    double B2[3];
    double B[6];
    double BN[6];
    double BxBN[4];
    double invBxBN[4];
    double invB[6];
    double S[2];
#endif

#if 0//useallmat
    cv::Mat leftM2=cv::Mat::zeros(6,5,CV_64F);
    cv::Mat zleftM2=cv::Mat::zeros(5,6,CV_64F);
    cv::Mat nleftM2=cv::Mat::zeros(5,5,CV_64F);
    cv::Mat B3(6,1,CV_64F);
    cv::Mat zB3(1,6,CV_64F);
    M_1.colRange(0,3).copyTo(leftM2.colRange(0,3).rowRange(0,3));
    M_2.colRange(0,3).copyTo(leftM2.colRange(0,3).rowRange(3,6));
    leftM2.at<double>(2,3)=-1;
    leftM2.at<double>(5,4)=-1;
    M_1.col(3).copyTo(B3.rowRange(0,3));
    M_2.col(3).copyTo(B3.rowRange(3,6));
    cv::Mat res(5,1,CV_64F);
#endif


#if 0//usefourmat
    cv::Mat leftM= cv::Mat::zeros(4,3,CV_64FC1);
    cv::Mat rightM= cv::Mat::zeros(4,1,CV_64FC1);
    cv::Mat point= cv::Mat::zeros(3,1,CV_64FC1);
#endif
    cv::Point3f resultP;
    cv::Point2f cameraP;
    cv::Point2f dlpP;
   // #pragma omp parallel for
    for(int i= 0;i< pointsNum;++i)
    {
        resultP;
        cameraP= leftPoints.at(i);
        dlpP= rightPoints.at(i);

        if( dlpP.x > 1280 || dlpP.x < 0 || dlpP.y > 800 || dlpP.y < 0)
        {
            continue;
        }
#if 1// usesfirst
        //使用先求出放大倍数S的方法，只需要对二维矩阵求逆
        b1[0]=cameraP.x;
        b1[1]=cameraP.y;
        b1[2]=1;
        b2[0]=dlpP.x;
        b2[1]=dlpP.y;
        b2[2]=1;
        double temp1;
        double temp2;
        //求B与B的转置
        for (int var = 0; var <3; ++var) {
            temp1=pA1[var*3]*b1[0]+pA1[var*3+1]*b1[1]+pA1[var*3+2]*b1[2];
            temp2=pA2[var*3]*b2[0]+pA2[var*3+1]*b2[1]+pA2[var*3+2]*b2[2];
            B1[var]=temp1;
            B2[var]=temp2;
            B[var*2]=temp1;
            B[var*2+1]=-temp2;
            BN[var]=temp1;
            BN[var+3]=-temp2;
        }
        //B的转置与B相乘
        BxBN[0]=BN[0]*B[0]+BN[1]*B[2]+BN[2]*B[4];
        BxBN[1]=BN[0]*B[1]+BN[1]*B[3]+BN[2]*B[5];
        BxBN[2]=BN[3]*B[0]+BN[4]*B[2]+BN[5]*B[4];
        BxBN[3]=BN[3]*B[1]+BN[4]*B[3]+BN[5]*B[5];
//        print_matrix2((char*)"bbt",2,3,BN,3);
//        print_matrix2((char*)"b",3,2,B,2);
//        print_matrix2((char*)"bbn",2,2,BxBN,2);
        //计算行列式
        double det=1/(BxBN[0]*BxBN[3]-BxBN[1]*BxBN[2]);
        //二维矩阵求逆
        invBxBN[0]=BxBN[3]*det;
        invBxBN[1]=-BxBN[1]*det;
        invBxBN[2]=-BxBN[2]*det;
        invBxBN[3]=BxBN[0]*det;
        //BxBN逆矩阵与B相乘
        for (int var = 0; var < 3; ++var) {
            invB[var]=invBxBN[0]*BN[var]+invBxBN[1]*BN[var+3];
            invB[var+3]=invBxBN[2]*BN[var]+invBxBN[3]*BN[var+3];
        }
        //求出两个放大系数S
        S[0]=invB[0]*pAD[0]+invB[1]*pAD[1]+invB[2]*pAD[2];
        S[1]=invB[3]*pAD[0]+invB[4]*pAD[1]+invB[5]*pAD[2];
        //std::cout<<S[0]<<" "<<S[1]<<std::endl;
        //分别求出坐标，取平均
        for (int var = 0; var < 3; ++var) {
            res1[var]=B1[var]*S[0]-pAd1[var];
            res2[var]=B2[var]*S[1]-pAd2[var];
            PRES[var]=(res1[var]+res2[var])/2;
        }
        resultP.x= PRES[0];
        resultP.y= PRES[1];
        resultP.z= PRES[2];
#endif

#if 0//useallmat
        //全矩阵运算
        leftM2.at<double>(0,3)=-cameraP.x;
        leftM2.at<double>(1,3)=-cameraP.y;
        leftM2.at<double>(3,4)=-dlpP.x;
        leftM2.at<double>(4,4)=-dlpP.y;
        transpose(leftM2,zleftM2);
        cv::invert(zleftM2*leftM2,nleftM2);
        res=nleftM2*zleftM2*B3;
        //std::cout <<leftM2<<std::endl;
        //cv::solve(leftM2,B3,res,CV_SVD);
        resultP.x= res.at<double>(0,0);
        resultP.y= res.at<double>(1,0);
        resultP.z= res.at<double>(2,0);
#endif
#if 0//uselapacke
        //使用lapack 十分难用(暂时无用)
        leftM2.at<double>(0,3)=-cameraP.x;
        leftM2.at<double>(1,3)=-cameraP.y;
        leftM2.at<double>(3,4)=-dlpP.x;
        leftM2.at<double>(4,4)=-dlpP.y;
        lapack_int m,n,lda,ldb,nrhs;
        lapack_int info;
        //lapack_int ipiv[5];
        m =6;
        n = 5;
        nrhs = 1;
        lda =5;
        ldb = 1;
        int ipiv[30];
        //transpose(leftM2,zleftM2);
        //transpose(B3,zB3);
        double *a=(double *)leftM2.data;
//        double *b=(double *)B3.data;
        std::cout<<leftM2<<std::endl;
        print_matrix2( (char*)"Entry Matrix A", m, n, a, lda );
        info = LAPACKE_dgetrf(LAPACK_ROW_MAJOR,m,n,a,lda,ipiv);
//        /* Print Right Rand Side */
//        print_matrix2((char*) "Right Hand Side b", m, nrhs, b, ldb );
//        info = LAPACKE_dgels(LAPACK_ROW_MAJOR,'N',m,n,nrhs,a,lda,b,ldb);
//        //print_matrix((char*)"l",n,nrhs,b,lda);
//        //cv::solve(leftM2,B3,res,CV_SVD);
//        print_matrix2((char*)"l",n,nrhs,b,ldb);
//        resultP.x= b[0];
//        resultP.y= b[1];
//        resultP.z= b[2];
#endif

#if 0//usefourmat
        //去除两个未知量运算
        leftM.at<double>(0,0)= cameraP.x*M_1.at<double>(2,0)- M_1.at<double>(0,0);
        leftM.at<double>(0,1)= cameraP.x*M_1.at<double>(2,1)- M_1.at<double>(0,1);
        leftM.at<double>(0,2)= cameraP.x*M_1.at<double>(2,2)- M_1.at<double>(0,2);
        leftM.at<double>(1,0)= cameraP.y*M_1.at<double>(2,0)- M_1.at<double>(1,0);
        leftM.at<double>(1,1)= cameraP.y*M_1.at<double>(2,1)- M_1.at<double>(1,1);
        leftM.at<double>(1,2)= cameraP.y*M_1.at<double>(2,2)- M_1.at<double>(1,2);
        leftM.at<double>(2,0)= dlpP.x*M_2.at<double>(2,0)- M_2.at<double>(0,0);
        leftM.at<double>(2,1)= dlpP.x*M_2.at<double>(2,1)- M_2.at<double>(0,1);
        leftM.at<double>(2,2)= dlpP.x*M_2.at<double>(2,2)- M_2.at<double>(0,2);
        leftM.at<double>(3,0)= dlpP.y*M_2.at<double>(2,0)- M_2.at<double>(1,0);
        leftM.at<double>(3,1)= dlpP.y*M_2.at<double>(2,1)- M_2.at<double>(1,1);
        leftM.at<double>(3,2)= dlpP.y*M_2.at<double>(2,2)- M_2.at<double>(1,2);
        rightM.at<double>(0,0)= M_1.at<double>(0,3)- cameraP.x*M_1.at<double>(2,3);
        rightM.at<double>(1,0)= M_1.at<double>(1,3)- cameraP.y*M_1.at<double>(2,3);
        rightM.at<double>(2,0)= M_2.at<double>(0,3)- dlpP.x*M_2.at<double>(2,3);
        rightM.at<double>(3,0)= M_2.at<double>(1,3)- dlpP.y*M_2.at<double>(2,3);

        cv::solve(leftM,rightM,point,CV_SVD);
        resultP.x= point.at<double>(0,0);
        resultP.y= point.at<double>(1,0);
        resultP.z= point.at<double>(2,0);
#endif
        pCloud.push_back(resultP);

//        int k = omp_get_thread_num();

//        if(k==0)cloud1.push_back(resultP);
//        if(k==1)cloud2.push_back(resultP);
//        if(k==2)cloud3.push_back(resultP);
//        if(k==3)cloud4.push_back(resultP);

    }

//    pCloud.insert(pCloud.end(),cloud1.begin(),cloud1.end());
//    pCloud.insert(pCloud.end(),cloud2.begin(),cloud2.end());
//    pCloud.insert(pCloud.end(),cloud3.begin(),cloud3.end());
//    pCloud.insert(pCloud.end(),cloud4.begin(),cloud4.end());

    std::cout<<"reconstruct3D consumed : "<<t1.elapsed()<<" ms\n";

//    std::cout<<"cloud1 size : "<<cloud1.size()<<std::endl;
//    std::cout<<"cloud2 size : "<<cloud2.size()<<std::endl;
//    std::cout<<"cloud3 size : "<<cloud3.size()<<std::endl;
//    std::cout<<"cloud4 size : "<<cloud4.size()<<std::endl;
}

void Yuan_pattern::savePointCloud(std::vector<cv::Point3f> pCloud,QString path)
{
    QTime t1;
    t1.start();

    QString savePoints= path;

    if(savePoints.isEmpty())
    {
        return ;
    }

    QFile p_file(savePoints);
    if(!p_file.open(QIODevice::WriteOnly)){
        qDebug()<<"Open Error!!!";
        return ;
    }

    QTextStream p_out(&p_file);

    //.ply头
    p_out<<"ply";
    p_out<<"\r\n";
    p_out<<"format ascii 1.0";
    p_out<<"\r\n";
    p_out<<"comment made by scan";
    p_out<<"\r\n";

    p_out<<"element vertex "<<pCloud.size();
    p_out<<"\r\n";

    p_out<<"property float x ";
    p_out<<"\r\n";


    p_out<<"property float y ";
    p_out<<"\r\n";

    p_out<<"property float z ";
    p_out<<"\r\n";

    p_out<<"end_header";
    p_out<<"\r\n";


    for(int i= 0;i< pCloud.size();++i)
    {
        cv::Point3f resultP= pCloud.at(i);

        p_out<<resultP.x<<" "<<resultP.y<<" "<<resultP.z;
        p_out<<"\r\n";
    }

    p_file.close();
    std::cout<<"savePointCloud consumed : "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::matrix_calculate(double *a, double *b, double *c, int cols)
{

        c[0]=0;
        c[1]=0;
        c[2]=0;
        for(int var = 0;var<cols;++var){
            c[0]+=a[var]*b[var];
            c[1]+=a[var+cols]*b[var];
            c[2]+=a[var+2*cols]*b[var];
        }

}

void Yuan_pattern::decode_phaseCode_sin_pattern(std::vector<cv::Mat> &srcImage,cv::Mat &dstImage,cv::Mat &mask,Direction _direction)
{   
    //the mat type is double !  0~2pi  wrapPhase
    QTime t1;
    t1.start();

      unsigned int imageRow=srcImage[0].rows;
      unsigned int imageCol=srcImage[0].cols;

//      if(srcImage[0].isContinuous())
//      {
//          imageCol=imageRow*imageCol;
//          imageRow=1;
//      }

      bool overOneStair = true;
      bool oneStair = false;

      cv::Mat sinusoidal=srcImage[0].clone();
      cv::Mat phaseCode =srcImage[1].clone();
      cv::Mat assistPhaseCode=srcImage[2].clone();

      double phaseCodeMinValue=0;
      double phaseCodeMaxValue=0;
      double denominator=0;
      cv::minMaxLoc(phaseCode,&phaseCodeMinValue,&phaseCodeMaxValue,NULL,NULL,mask);
      //std::cout<<"phaseCodeMinValue : "<<phaseCodeMinValue<<" , phaseCodeMaxValue : "<<phaseCodeMaxValue<<std::endl;
      denominator = phaseCodeMaxValue - phaseCodeMinValue;

      double assitPhaseCodeMinValue=0;
      double assitPhaseCodeMaxValue=0;
      double assitDenominator=0;
      cv::minMaxLoc(assistPhaseCode,&assitPhaseCodeMinValue,&assitPhaseCodeMaxValue,NULL,NULL,mask);
      //std::cout<<"phaseCodeMinValue : "<<phaseCodeMinValue<<" , phaseCodeMaxValue : "<<phaseCodeMaxValue<<std::endl;
      assitDenominator = assitPhaseCodeMaxValue - assitPhaseCodeMinValue;

      cv::Mat unwrapPhase=cv::Mat::zeros(sinusoidal.rows,sinusoidal.cols,CV_64FC1);

      unsigned int i=0,j=0;

      for(i=0;i<imageRow;++i)
      {
          double *phaseCodePtr=phaseCode.ptr<double>(i);
          double *sinusoidalPtr=sinusoidal.ptr<double>(i);
          double *assistPhaseCodePtr=assistPhaseCode.ptr<double>(i);
          uchar *maskPtr=mask.ptr<uchar>(i);
          double *unwrapPtr=unwrapPhase.ptr<double>(i);

          for(j=0;j<imageCol;++j)
          {
              if(maskPtr[j]!=0)
              {
                  if(_direction == Vertical)
                  {
                      //// one stair
if(oneStair){
//                    phaseCodePtr[j]= 31 - floor(phaseCodePtr[j]*32/(2*CV_PI));
//                    if(phaseCodePtr[j]==-1)phaseCodePtr[j]=0;
//                    unwrapPtr[j]=(phaseCodePtr[j])*2*CV_PI+sinusoidalPtr[j];

                      phaseCodePtr[j]= 31 - floor( 32*(phaseCodePtr[j]-phaseCodeMinValue)/denominator );
                      if(phaseCodePtr[j]==-1)phaseCodePtr[j]=0;
                      unwrapPtr[j]=(phaseCodePtr[j])*2*CV_PI+sinusoidalPtr[j];
}
                    //// over one stair
if(overOneStair){

                    phaseCodePtr[j]=  7*(phaseCodePtr[j]-phaseCodeMinValue)/denominator ;
                    if( phaseCodePtr[j] < 6.5 && phaseCodePtr[j] >0.5 ) phaseCodePtr[j]=floor( phaseCodePtr[j] );
                    else phaseCodePtr[j] = 7;
                    phaseCodePtr[j]= 7 - phaseCodePtr[j] ;

                    assistPhaseCodePtr[j]= 3*(assistPhaseCodePtr[j]-assitPhaseCodeMinValue)/assitDenominator;
                    if(assistPhaseCodePtr[j]<2.7 && assistPhaseCodePtr[j]>0.1 )assistPhaseCodePtr[j]=floor( assistPhaseCodePtr[j] );
                    else assistPhaseCodePtr[j]=3;
                    assistPhaseCodePtr[j] = 3 - assistPhaseCodePtr[j] ;
                    //unwrapPtr[j]=phaseCodePtr[j];
                    //unwrapPtr[j]=assistPhaseCodePtr[j];
                    unwrapPtr[j]=(phaseCodePtr[j]+assistPhaseCodePtr[j]*8)*2*CV_PI+sinusoidalPtr[j];

}
                  }else{
                       //// one stair
if(oneStair){
//                       phaseCodePtr[j]= 19 - floor(phaseCodePtr[j]*20/(2*CV_PI));
//                      if(phaseCodePtr[j]==-1)phaseCodePtr[j]=0;
//                      unwrapPtr[j]=(phaseCodePtr[j])*2*CV_PI+sinusoidalPtr[j];
                      phaseCodePtr[j]= 19 - floor( 20*(phaseCodePtr[j]-phaseCodeMinValue)/denominator );
                      if(phaseCodePtr[j]==-1)phaseCodePtr[j]=0;
                      unwrapPtr[j]=(phaseCodePtr[j])*2*CV_PI+sinusoidalPtr[j];
}

                      //// over one stair
if(overOneStair){
                    phaseCodePtr[j]=  4*(phaseCodePtr[j]-phaseCodeMinValue)/denominator ;
                    if( phaseCodePtr[j] < 3.5 && phaseCodePtr[j] >0.5 ) phaseCodePtr[j]=floor( phaseCodePtr[j] );
                    else phaseCodePtr[j] = 4;
                    phaseCodePtr[j]= 4 - phaseCodePtr[j] ;

                    assistPhaseCodePtr[j]= 3*(assistPhaseCodePtr[j]-assitPhaseCodeMinValue)/assitDenominator ;
                    if(assistPhaseCodePtr[j]<2.7 && assistPhaseCodePtr[j]>0.1 )assistPhaseCodePtr[j]=floor( assistPhaseCodePtr[j] );
                    else assistPhaseCodePtr[j]=3;
                    assistPhaseCodePtr[j] = 3 - assistPhaseCodePtr[j] ;
                    //unwrapPtr[j]=phaseCodePtr[j];
                    //unwrapPtr[j]=assistPhaseCodePtr[j];
                    unwrapPtr[j]=(phaseCodePtr[j]+assistPhaseCodePtr[j]*5)*2*CV_PI+sinusoidalPtr[j];

}
                  }

              }
          }
      }

      dstImage=unwrapPhase.clone();

//      if(_direction==Vertical )
//      {

//          for(i=0;i<imageRow;++i)
//          {
//              uchar *maskPtr = mask.ptr<uchar>(i);
//              double* dstPtr = dstImage.ptr<double>(i);
//              double *sinusoidalPtr=sinusoidal.ptr<double>(i);
//              double *phaseCodePtr=phaseCode.ptr<double>(i);
//              double *assistPhaseCodePtr=assistPhaseCode.ptr<double>(i);
//              for(j=0;j<imageCol;++j)
//              {
//                  if(maskPtr[j]!=0)
//                  {
//                      if( j==0 || j==1 ){
//                           assistPhaseCodePtr[j] = 0;
//                      }
//                      else if( j>0 &&  abs( phaseCodePtr[j] - phaseCodePtr[j-1] ) < 5 )
//                      {
//                          assistPhaseCodePtr[j] = assistPhaseCodePtr[j-1];
//                      }else if( j>0 &&  abs( phaseCodePtr[j] - phaseCodePtr[j-1] ) >= 5 )
//                        {
//                            assistPhaseCodePtr[j] = assistPhaseCodePtr[j-1] +1;
//                        }
//                      dstPtr[j]=(phaseCodePtr[j]+assistPhaseCodePtr[j]*8)*2*CV_PI+sinusoidalPtr[j];
//                  }
//              }
//          }

//          for(i=0;i<imageRow;++i)
//          {
//              uchar *maskPtr = mask.ptr<uchar>(i);
//              double* dstPtr = dstImage.ptr<double>(i);
//              for(j=0;j<imageCol;++j)
//              {
//                  if(maskPtr[j]!=0)
//                  {
////                    if( j>0 && dstPtr[j] - dstPtr[j-1] > CV_PI
////                            && dstPtr[j] - dstPtr[j-1] < 2.5*CV_PI ) dstPtr[j] -= 2*CV_PI;
////                    else if( j>0 && dstPtr[j-1] - dstPtr[j] > CV_PI
////                                 && dstPtr[j-1] - dstPtr[j] < 2.5*CV_PI ) dstPtr[j] += 2*CV_PI;

////                    if( j>0 && dstPtr[j] - dstPtr[j-1] > CV_PI
////                            && dstPtr[j] - dstPtr[j-1] < 2*CV_PI ) dstPtr[j] -= 2*CV_PI;
////                    else if( j>0 && dstPtr[j-1] - dstPtr[j] > CV_PI
////                                 && dstPtr[j-1] - dstPtr[j] < 2*CV_PI ) dstPtr[j] += 2*CV_PI;
//                  }
//              }
//          }

//      }else{

//          for(i=0;i<imageRow;++i)
//          {
//              for(j=0;j<imageCol;++j)
//              {
//                  if(mask.at<uchar>(i,j)!=0)
//                  {
//                      if(i==0||i==1) assistPhaseCode.at<double>(i,j) = 0;
//                      else if( i>0 &&  abs( phaseCode.at<double>(i,j) -  phaseCode.at<double>(i-1,j) ) < 3 )
//                      {
//                          assistPhaseCode.at<double>(i,j) = assistPhaseCode.at<double>(i-1,j);
//                      }else if( i>0 &&  abs( phaseCode.at<double>(i,j) -  phaseCode.at<double>(i-1,j) ) >= 3  )
//                        {
//                             assistPhaseCode.at<double>(i,j) = assistPhaseCode.at<double>(i-1,j) +1 ;
//                        }
//                      dstImage.at<double>(i,j) =(phaseCode.at<double>(i,j) + assistPhaseCode.at<double>(i,j)*5)*2*CV_PI+sinusoidal.at<double>(i,j);
//                  }
//              }
//          }

//          for(i=0;i<imageRow;++i)
//          {
//              for(j=0;j<imageCol;++j)
//              {
//                  if(mask.at<uchar>(i,j)!=0)
//                  {
////                    if( i>0 && dstImage.at<double>(i,j) - dstImage.at<double>(i-1,j) > CV_PI
////                            &&dstImage.at<double>(i,j) - dstImage.at<double>(i-1,j) < 2.5*CV_PI )
////                        dstImage.at<double>(i,j) -= 2*CV_PI;

////                      if( i>0 && dstImage.at<double>(i-1,j) - dstImage.at<double>(i,j) > CV_PI
////                             &&dstImage.at<double>(i-1,j) - dstImage.at<double>(i,j) < 2.5*CV_PI )
////                        dstImage.at<double>(i,j) += 2*CV_PI;

//                  }
//              }
//          }
//      }

      image_paint.push_back(sinusoidal);
      image_paint.push_back(phaseCode);
      image_paint.push_back(assistPhaseCode);
      image_paint.push_back(dstImage);

      std::cout<<"the time of decode_phaseCode_sin_pattern is "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::decode_pure_phaseCode2016_pattern(std::vector<cv::Mat> &srcImage, cv::Mat &dstImage, cv::Mat &mask, Direction _direction)
{
    QTime t1;
    t1.start();

      unsigned int imageRow=srcImage[0].rows;
      unsigned int imageCol=srcImage[0].cols;

      double phaseCodeMinValue=0;
      double phaseCodeMaxValue=0;
      double denominator=0;
      cv::minMaxLoc(srcImage[1],&phaseCodeMinValue,&phaseCodeMaxValue,NULL,NULL,mask);
      denominator = phaseCodeMaxValue - phaseCodeMinValue;
      unsigned int i=0,j=0;
      std::vector<std::vector<int>> jumpPoint;
      jumpPoint.reserve(2000);

      for(i=0;i<imageRow;++i)
      {
          double *phaseCodePtr  =srcImage[1].ptr<double>(i);
          uchar *maskPtr=mask.ptr<uchar>(i);

          for(j=0;j<imageCol;++j)
          {
              if(maskPtr[j]!=0)
              {
                  if(_direction==Vertical){

                      phaseCodePtr[j] =   (3 * (phaseCodePtr[j] - phaseCodeMinValue)/denominator);
                      if( phaseCodePtr[j] < 2.5 && phaseCodePtr[j] >0.5 ) phaseCodePtr[j]=floor( phaseCodePtr[j] );
                      else phaseCodePtr[j] = 3;
                       phaseCodePtr[j]= 3 - phaseCodePtr[j] ;

                  }else{
                      phaseCodePtr[j] =   (3 * (phaseCodePtr[j] - phaseCodeMinValue)/denominator);
                      if( phaseCodePtr[j] < 2.5 && phaseCodePtr[j] >0.5 ) phaseCodePtr[j]=floor( phaseCodePtr[j] );
                      else phaseCodePtr[j] = 3;
                       phaseCodePtr[j]= 3 - phaseCodePtr[j] ;
                  }
              }
          }
      }
      cv::Mat tempPhase = srcImage[1].clone();
      if(_direction==Vertical){

      for(i=0;i<imageRow;++i)
      {
          double *phaseCodePtr  =srcImage[1].ptr<double>(i);
          uchar *maskPtr=mask.ptr<uchar>(i);
          std::vector<int> tempNum;
          tempNum.reserve(5);
          for(j=0;j<imageCol;++j)
          {
              if(maskPtr[j]!=0)
              {

                      if( j>100 && j<imageCol-1 && abs(3 - phaseCodePtr[j] - phaseCodePtr[j] ) > 2
                             &&  phaseCodePtr[j-1] == 3 && phaseCodePtr[j+1] == 0 )
                      {
                          switch( tempNum.size() )
                          {
                              case 0:
                                  tempNum.push_back(j);
                                  break;
                              case 1:
                                  if( (j-tempNum[0])>200)tempNum.push_back(j);
                                  break;
                              case 2:
                                  if( (j-tempNum[1])>200)tempNum.push_back(j);
                                  break;
                              case 3:
                                  if( (j-tempNum[2])>200)tempNum.push_back(j);
                                  break;

                          }
                      }

                }
          }
          jumpPoint.push_back(tempNum);
      }

      }else{

          for(j=0;j<imageCol;++j)
          {
              std::vector<int> tempNum;
              tempNum.reserve(5);
              for(i=0;i<imageRow;++i)
              {
                  if(mask.at<uchar>(i,j)!=0)
                  {

                      if( i>200 && i<imageRow-1 && abs( 3 - srcImage[1].at<double>(i,j) - srcImage[1].at<double>(i,j) ) > 2
                             &&  srcImage[1].at<double>(i-1,j) == 3 && srcImage[1].at<double>(i+1,j) == 0)
                      {
                          switch(tempNum.size())
                          {
                              case 0:
                                  tempNum.push_back(i);
                                  break;
                              case 1:
                                  if((i-tempNum[0])>100)tempNum.push_back(i);
                                  break;
                              case 2:
                                  if((i-tempNum[1])>100)tempNum.push_back(i);
                                  break;
                              case 3:
                                  if((i-tempNum[2])>100)tempNum.push_back(i);
                                  break;

                          }
                      }

                  }
              }
              jumpPoint.push_back(tempNum);
          }
      }
      std::cout<<"jumpPoint : "<<jumpPoint[500][0]<<" "<<jumpPoint[500][1]<<std::endl;

      for(i=0;i<imageRow;++i)
      {
          double *phaseCodePtr  =srcImage[1].ptr<double>(i);
          uchar *maskPtr=mask.ptr<uchar>(i);

          for(j=0;j<imageCol;++j)
          {
              if(maskPtr[j]!=0)
              {
                  if(_direction==Vertical){
                      switch( jumpPoint[i].size() )
                      {
                          case 2:
                              if(j>jumpPoint[i][0] && j<=jumpPoint[i][1])phaseCodePtr[j] += 4;
                              else if(j>jumpPoint[i][1])phaseCodePtr[j] += 8;
                              break;
                          case 3:
                              if(j>jumpPoint[i][0] && j<=jumpPoint[i][1])phaseCodePtr[j] += 4;
                              else if(j>jumpPoint[i][1] && j<=jumpPoint[i][2]) phaseCodePtr[j] += 8;
                              else if(j>jumpPoint[i][2])  phaseCodePtr[j] += 12;
                              break;
                          case 4:
                              if(j>jumpPoint[i][0] && j<=jumpPoint[i][1])phaseCodePtr[j] += 4;
                              else if(j>jumpPoint[i][1] && j<=jumpPoint[i][2]) phaseCodePtr[j] += 8;
                              else if(j>jumpPoint[i][2] && j<=jumpPoint[i][3]) phaseCodePtr[j] += 12;
                              else if(j>jumpPoint[i][3]) phaseCodePtr[j] += 16;
                              break;
                      }
                  }else{
                      switch(jumpPoint[j].size())
                      {
                          case 2:
                              if(i>jumpPoint[j][0] && i<=jumpPoint[j][1])phaseCodePtr[j] += 4;
                              else if(i>jumpPoint[j][1])phaseCodePtr[j] += 8;
                              break;
                          case 3:
                              if(i>jumpPoint[j][0] && i<=jumpPoint[j][1])phaseCodePtr[j] += 4;
                              else if(i>jumpPoint[j][1] && i<=jumpPoint[j][2]) phaseCodePtr[j] += 8;
                              else if(i>jumpPoint[j][2] ) phaseCodePtr[j] += 12;
                              break;
                          case 4:
                              if(i>jumpPoint[j][0] && i<=jumpPoint[j][1])phaseCodePtr[j] += 4;
                              else if(i>jumpPoint[j][1] && i<=jumpPoint[j][2]) phaseCodePtr[j] += 8;
                              else if(i>jumpPoint[j][2] && i<=jumpPoint[j][3]) phaseCodePtr[j] += 12;
                              else if(i>jumpPoint[j][3]) phaseCodePtr[j] += 16;
                              break;
                      }
                  }
              }
          }
      }

      cv::Mat assitCode = cv::Mat::zeros(imageRow,imageCol,CV_64FC1);
      cv::Mat Code = cv::Mat::zeros(imageRow,imageCol,CV_64FC1);
      for(i=0;i<imageRow;++i)
      {
          double *phaseCodePtr  =srcImage[1].ptr<double>(i);
          double *assitPtr = assitCode.ptr<double>(i);
          double *codePtr = Code.ptr<double>(i);
          double *sinPtr = srcImage[0].ptr<double>(i);
          double *dstPtr = dstImage.ptr<double>(i);
          uchar *maskPtr=mask.ptr<uchar>(i);

          for(j=0;j<imageCol;++j)
          {
              if(maskPtr[j]!=0  )
              {
                      if(_direction==Vertical){
                          if(j<1860-57){
                              assitPtr[j] = phaseCodePtr[j+57];
                          }
                          else{
                              assitPtr[j] = phaseCodePtr[1860]+1;
                          }
                          codePtr[j] = assitPtr[j] + phaseCodePtr[j];
                          dstPtr[j]   = sinPtr[j] + codePtr[j]*2*CV_PI;
                      }else{
                          if(i<1160-57){
                              assitCode.at<double>(i,j) = srcImage[1].at<double>(i+57,j);
                          }
                          else{
                              assitCode.at<double>(i,j) = srcImage[1].at<double>(1160,j)+1;
                          }
                          codePtr[j] = assitPtr[j] + phaseCodePtr[j];
                          dstPtr[j]   = sinPtr[j] + codePtr[j]*2*CV_PI;
                      }

              }
          }
      }


      if(_direction==Vertical )
      {
          for(i=0;i<imageRow;++i)
          {
              uchar *maskPtr=mask.ptr<uchar>(i);
              double* dstPtr = dstImage.ptr<double>(i);
              for(j=0;j<imageCol;++j)
              {
                  if(maskPtr[j]!=0)
                  {
                      if( j>0 && dstPtr[j] - dstPtr[j-1] > CV_PI
                              && dstPtr[j] - dstPtr[j-1] < 2.5*CV_PI ) dstPtr[j] -= 2*CV_PI;
                      else if( j>0 && dstPtr[j-1] - dstPtr[j] > CV_PI
                                   && dstPtr[j-1] - dstPtr[j] < 2.5*CV_PI ) dstPtr[j] += 2*CV_PI;
                  }
              }
          }
      }else{

          for(i=0;i<imageRow;++i)
          {
              for(j=0;j<imageCol;++j)
              {
                  if(mask.at<uchar>(i,j)!=0)
                  {
                      if( i>0 && dstImage.at<double>(i,j) - dstImage.at<double>(i-1,j) > CV_PI
                              &&dstImage.at<double>(i,j) - dstImage.at<double>(i-1,j) < 2.5*CV_PI )
                          dstImage.at<double>(i,j) -= 2*CV_PI;
                      else if( i>0 && dstImage.at<double>(i-1,j) - dstImage.at<double>(i,j) > CV_PI
                               &&dstImage.at<double>(i-1,j) - dstImage.at<double>(i,j) < 2.5*CV_PI )
                          dstImage.at<double>(i,j) += 2*CV_PI;
                  }
              }
          }
      }


      image_paint.push_back(srcImage[0]);
//      image_paint.push_back(srcImage[1]);
//      image_paint.push_back(assitCode);
      image_paint.push_back(tempPhase);
      image_paint.push_back(Code);
      image_paint.push_back(dstImage);
      std::cout<<"the time of decode_pure_phaseCode2016_pattern is "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::decode_phaseCode_grayCode_sin_pattern(std::vector<cv::Mat> &srcImage, cv::Mat &dstImage, cv::Mat &mask, Direction _direction)
{
    QTime t1;
    t1.start();

      unsigned int imageRow=srcImage[0].rows;
      unsigned int imageCol=srcImage[0].cols;

      double phaseCodeMinValue=0;
      double phaseCodeMaxValue=0;
      double denominator=0;
      cv::minMaxLoc(srcImage[1],&phaseCodeMinValue,&phaseCodeMaxValue,NULL,NULL,mask);
      //std::cout<<"phaseCodeMinValue : "<<phaseCodeMinValue<<" , phaseCodeMaxValue : "<<phaseCodeMaxValue<<std::endl;
      denominator = phaseCodeMaxValue - phaseCodeMinValue;
      unsigned int i=0,j=0;

      for(i=0;i<imageRow;++i)
      {
          double *sinusoidalPtr =srcImage[0].ptr<double>(i);
          double *phaseCodePtr  =srcImage[1].ptr<double>(i);
          uchar *grayCodePtr    =srcImage[2].ptr<uchar>(i);
          uchar *maskPtr=mask.ptr<uchar>(i);
          double *unwrapPtr=dstImage.ptr<double>(i);

          for(j=0;j<imageCol;++j)
          {
              if(maskPtr[j]!=0)
              {
                  if(_direction == Vertical)
                  {
                      phaseCodePtr[j] =   (7 * (phaseCodePtr[j] - phaseCodeMinValue)/denominator);
                      if( phaseCodePtr[j] < 6.5 && phaseCodePtr[j] >0.5 ) phaseCodePtr[j]=floor( phaseCodePtr[j] );
                      else phaseCodePtr[j] = 7;

                    if(grayCodePtr[j]%2 != 0) phaseCodePtr[j]= 7 - phaseCodePtr[j];
                    if(phaseCodePtr[j]==-1) phaseCodePtr[j]=0;

                    unwrapPtr[j]=(phaseCodePtr[j]+grayCodePtr[j]*8)*2*CV_PI+sinusoidalPtr[j];
                   //unwrapPtr[j]=phaseCodePtr[j]+grayCodePtr[j]*8;

                  }else{

                      phaseCodePtr[j] =   (4 * (phaseCodePtr[j] - phaseCodeMinValue)/denominator);
                      if( phaseCodePtr[j] < 3.5 && phaseCodePtr[j] >0.5 ) phaseCodePtr[j]=floor( phaseCodePtr[j] );
                      else phaseCodePtr[j] = 4;

                    if(grayCodePtr[j]%2 != 0) phaseCodePtr[j]= 4 - phaseCodePtr[j];
                    if(phaseCodePtr[j]==-1) phaseCodePtr[j]=0;

                      unwrapPtr[j]=(phaseCodePtr[j]+grayCodePtr[j]*5)*2*CV_PI+sinusoidalPtr[j];
                     // unwrapPtr[j]=phaseCodePtr[j]+grayCodePtr[j]*5;
                  }

              }
          }
      }

      if(_direction==Vertical )
      {
          for(i=0;i<imageRow;++i)
          {
              uchar *maskPtr=mask.ptr<uchar>(i);
              double* dstPtr = dstImage.ptr<double>(i);
              for(j=0;j<imageCol;++j)
              {
                  if(maskPtr[j]!=0)
                  {
                      if( j>0 && dstPtr[j] - dstPtr[j-1] > CV_PI
                              && dstPtr[j] - dstPtr[j-1] < 2.5*CV_PI ) dstPtr[j] -= 2*CV_PI;
                      else if( j>0 && dstPtr[j-1] - dstPtr[j] > CV_PI
                                   && dstPtr[j-1] - dstPtr[j] < 2.5*CV_PI ) dstPtr[j] += 2*CV_PI;
                  }
              }
          }
      }else{

          for(i=0;i<imageRow;++i)
          {
              for(j=0;j<imageCol;++j)
              {
                  if(mask.at<uchar>(i,j)!=0)
                  {
                      if( i>0 && dstImage.at<double>(i,j) - dstImage.at<double>(i-1,j) > CV_PI
                              &&dstImage.at<double>(i,j) - dstImage.at<double>(i-1,j) < 2.5*CV_PI )
                          dstImage.at<double>(i,j) -= 2*CV_PI;
                      else if( i>0 && dstImage.at<double>(i-1,j) - dstImage.at<double>(i,j) > CV_PI
                               &&dstImage.at<double>(i-1,j) - dstImage.at<double>(i,j) < 2.5*CV_PI )
                          dstImage.at<double>(i,j) += 2*CV_PI;
                  }
              }
          }
      }


      image_paint.push_back(srcImage[0]);
      image_paint.push_back(srcImage[1]);
      image_paint.push_back(srcImage[2]);
      image_paint.push_back(dstImage);
      std::cout<<"the time of decode_phaseCode_grayCode_sin_pattern is "<<t1.elapsed()<<" ms\n";
}

void Yuan_pattern::decode_grayCode_sin_pattern(std::vector<cv::Mat> &srcImage, cv::Mat &dstImage, cv::Mat &mask,Direction _direction)
{
    QTime t1;
    t1.start();

      unsigned int imageRow=srcImage[0].rows;
      unsigned int imageCol=srcImage[0].cols;

      cv::Mat sinusoidal=srcImage[0].clone();
      cv::Mat grayCode=srcImage[1].clone();

      cv::Mat unwrapPhase=cv::Mat::zeros(sinusoidal.rows,sinusoidal.cols,CV_64FC1);

      unsigned int i=0,j=0;

      for(i=0;i<imageRow;++i)
      {
          double *sinusoidalPtr=sinusoidal.ptr<double>(i);
          uchar *grayCodePtr=grayCode.ptr<uchar>(i);
          uchar *maskPtr=mask.ptr<uchar>(i);
          double *unwrapPtr=unwrapPhase.ptr<double>(i);

          for(j=0;j<imageCol;++j)
          {
              if(maskPtr[j]!=0)
              {
                 unwrapPtr[j]=grayCodePtr[j]*2*CV_PI+sinusoidalPtr[j];
              }
          }
      }
      for(i=0;i<imageRow;++i)
      {
          uchar *maskPtr=mask.ptr<uchar>(i);
          for(j=0;j<imageCol;++j)
          {
              if(maskPtr[j]!=0)
              {
                  if(_direction==Vertical )
                  {
                      if( j>0 && unwrapPhase.at<double>(i,j)- unwrapPhase.at<double>(i,j-1) > CV_PI) unwrapPhase.at<double>(i,j)-=2*CV_PI;
                      else if( j>0 && unwrapPhase.at<double>(i,j-1)- unwrapPhase.at<double>(i,j) > CV_PI) unwrapPhase.at<double>(i,j)+=2*CV_PI;
                  }
                  else if(_direction==Horizontal )
                  {
                      if( i>0 && unwrapPhase.at<double>(i,j)- unwrapPhase.at<double>(i-1,j) > CV_PI) unwrapPhase.at<double>(i,j)-=2*CV_PI;
                      else if( i>0 && unwrapPhase.at<double>(i-1,j)- unwrapPhase.at<double>(i,j) > CV_PI) unwrapPhase.at<double>(i,j)+=2*CV_PI;
                  }
              }
          }
      }


      dstImage=unwrapPhase.clone();

      image_paint.push_back(sinusoidal);
      image_paint.push_back(grayCode);
      image_paint.push_back(unwrapPhase);
      std::cout<<"the time of decode_grayCode_sin_pattern is "<<t1.elapsed()<<" ms\n";
}














