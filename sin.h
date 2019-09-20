#ifndef SIN_H
#define SIN_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <vector>

enum Direction{Horizontal,Vertical};
class sinusoidal : public QObject
{
    Q_OBJECT
public:
    explicit sinusoidal(QObject *parent = 0);
    ~sinusoidal();

private:
    int columns;
    int rows;
    int PixelsPerPeriod;
    Direction direction;

public:
    void ResolutionSetting(int col, int row);
    void ParameterSetting(int period ,Direction _direction);
    void GeneratePattern_sinusoidal(std::vector<cv::Mat> &PatternSequence);
signals:
    void getResult();


};
#endif // SIN_H
