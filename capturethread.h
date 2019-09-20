#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <dlp_sdk.hpp>
#include <QImage>
#include <QCoreApplication>
#include <QTime>


class CaptureThread : public QThread
{
    Q_OBJECT
public:
    explicit CaptureThread();

    void setCaptureFlag(bool flag);

    void setCamera(dlp::PG_FlyCap2_C* camera);

    void connectCamera();

    void captureImage();

    void capture_continuous_Image();

    void startCameraCapture();

signals:
    void send_capture_mat(QImage img);

public slots:


protected:
    void run();

private:
    bool capture_flag;

    dlp::PG_FlyCap2_C   camera;

    dlp::PG_FlyCap2_C* camera_point;

    bool connectFlag;
};

#endif // CAPTURETHREAD_H
