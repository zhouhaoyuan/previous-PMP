#include "sin.h"
#define PI 3.1415926

sinusoidal::sinusoidal(QObject *parent) : QObject(parent)
{
    this->columns=1056;
    this->rows=1056;
    this->PixelsPerPeriod=33;

}

sinusoidal::~sinusoidal()
{

}

void sinusoidal::ResolutionSetting(int col, int row)
{
    this->columns=col;
    this->rows=row;
}
void sinusoidal::ParameterSetting(int period , Direction _direction)
{
    this->PixelsPerPeriod=period;
    this->direction=_direction;
}
void sinusoidal::GeneratePattern_sinusoidal(std::vector<cv::Mat> &PatternSequence)
{
    int rowNumber = this->rows;
    int colNumber = this->columns;
    int pixels_per_period=this->PixelsPerPeriod;
    switch(this->direction)
    {
    case Horizontal:
    {
        for(int n=0;n<4;n++)
        {
            cv::Mat sinusoidal_pattern(rowNumber, colNumber, CV_8UC1);
            for(int i=0;i<rowNumber;i++)
            {
                for(int j=0;j<colNumber;j++)
                {
                    sinusoidal_pattern.at<uchar>(i,j)=125+125*cos((i+1)*2*PI/pixels_per_period+(n-2)*PI/2);
                }
            }
            PatternSequence.push_back(sinusoidal_pattern);
        }
    }
        break;
    case Vertical:
    {

        for(int n=0;n<4;n++)
        {
            cv::Mat sinusoidal_pattern(rowNumber, colNumber, CV_8UC1);
            for(int i=0;i<rowNumber;i++)
            {
                for(int j=0;j<colNumber;j++)
                {
                    sinusoidal_pattern.at<uchar>(i,j)=125+125*cos(2*PI*(j+1)/pixels_per_period+(n-2)*PI/2);
                }
            }
            PatternSequence.push_back(sinusoidal_pattern);
        }
    }
        break;
    }
}
