#ifndef PAINTDIALOG_H
#define PAINTDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPixmap>
#include <QFont>
#include <QPaintEvent>
#include <QButtonGroup>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "yuan_pattern.h"

enum channel{red,green,blue};

enum direction{vertical,horizontal};

namespace Ui {
class paintDialog;
}

class paintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit paintDialog(QWidget *parent = 0);
    ~paintDialog();
protected:
    void paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        painter.drawPixmap(5,5,*pixmap);
    }

private:

    void crosstalk_Paint(cv::Mat &srcData,channel channel_select,cv::Mat &mask);

    void phase_Paint(cv::Mat &srcData,direction _direction,cv::Mat &mask);

    void value_Paint(std::vector<double> &src);

    void pointXY_Paint(std::vector<pointXY> &src);

private slots:

    void on_Red_Btn_clicked();

    void on_Blue_Btn_clicked();

    void on_Green_Btn_clicked();

    void on_clear_Btn_clicked();

    void on_xPhase_Btn_clicked();

    void on_yPhase_Btn_clicked();

    void on_valueBtn_clicked();

    void on_XY_Btn_clicked();

private:
    Ui::paintDialog *ui;
    QPixmap *pixmap;
    std::vector<cv::Mat> InputData;
    std::vector<std::vector<double>> valueData;
    std::vector<std::vector<pointXY>> pointXYData;
    QButtonGroup *directionGroup;
public:
    void getData(std::vector<cv::Mat> &Input);
    void getData(std::vector<std::vector<double>> &Input);
    void getData(std::vector<std::vector<pointXY>> &Input);
};

inline void paintDialog::getData(std::vector<std::vector<double>> &Input){
    valueData=Input;
}
inline void paintDialog::getData(std::vector<std::vector<pointXY>> &Input){
    pointXYData=Input;
}

#endif // PAINTDIALOG_H
