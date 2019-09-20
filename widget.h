#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "view.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
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
#include <QImage>
#include <QMessageBox>
#include <QButtonGroup>
#include <QThread>
#include "imageitem.h"
#include "calibrate.h"
#include <QDir>
#include "calibratemachine.h"
#include "mutil_wavelength.h"
#include <common/returncode.hpp>
#include <common/debug.hpp>
#include <common/parameters.hpp>
#include <common/capture/capture.hpp>
#include <common/pattern/pattern.hpp>
#include <common/disparity_map.hpp>
#include <common/pattern/pattern.hpp>
#include <QMessageBox>
#include <QCoreApplication>
#include <dlp_sdk.hpp>
#include <structured_light/structured_light.hpp>
#include <structured_light/gray_code/gray_code.hpp>
#include <structured_light/three_phase/three_phase.hpp>
#include "capturethread.h"
#include "binary_code.h"
#include "reconstruct3d.h"

#include <QCoreApplication>
#include <QLabel>
#include <QDesktopWidget>
#include <QApplication>
#include <QCheckBox>
#include "yuan_pattern.h"
#include "paintdialog.h"
#include "MultipleLinearRegression.h"

#define PI 3.141592654

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    void showImage(cv::Mat img);

    void showMessage(QString meg);

    cv::Mat QImageToCvMat(QImage image);


    void cvShowMatFloat(cv::Mat img, std::string name,int period);

    void cvShowMat(cv::Mat img, std::string name);

    void fourStepPhaseShiftFloat(cv::Mat image_1,cv::Mat image_2,cv::Mat image_3,cv::Mat image_4,cv::Mat& resultMat);

    void multipleWavelengthUnwrapFloat(std::vector<cv::Mat> setImage,cv::Mat& resultMat) ;

    void addProjectImage(QString dir_name);


    void decodeThreeHoterodyUnwrap(std::vector<cv::Mat> in_mat,cv::Mat &unwrap_x,cv::Mat &unwrap_y,cv::Mat mask);


    void unwarpHierarchical(float rate, cv::Mat const l_wrap, cv::Mat h_wrap, cv::Mat &h_unwrap, cv::Mat &k_Mat);


    void phaseNormalization(float rate, cv::Mat unwrap_phase, cv::Mat &norm_phase);

    void unwrapThreeFrequencyHeterodyne(std::vector<cv::Mat> wrap_list,cv::Mat& unwrap_phase);

    void decodeMultiplyWavelengthUnwrap(std::vector<cv::Mat> in_mat,cv::Mat &unwrap_x,cv::Mat &unwrap_y);

    //dlp
    void connectDevice();

    void projectSignalPattern( dlp::DLP_Platform *projector, int num);

    void reconnectDevice();

    void highspeeDebugdScanPatterns(dlp::Camera *camera, dlp::DLP_Platform *projector, const bool &cam_proj_hw_synchronized,
                                    int startNum, int captureNum, std::vector<Mat> &mat_list);

    void generateBinaryPatterns(dlp::Pattern::Orientation orientation,int g_count,dlp::Pattern::Sequence& g_sequence);

    void generateScanPatterns(int squence_count, dlp::Pattern::Sequence& scan_sequence);

    void generateFourMultiplyWavelengthPatterns(dlp::Pattern::Orientation orientation,int g_count,dlp::Pattern::Sequence& g_sequence);

    void prepareScanPatterns(bool updateFlag);

    void projectSignalBinaryPattern(dlp::DLP_Platform *projector,int num);


    void SavePatternImageData(const dlp::Pattern::Sequence &sequence, int num, const std::string basename);



    /***************************************************************/
    void project_sleep(unsigned int msec);

    bool project_capture_pattern(std::vector<cv::Mat> &pattern_list);

    void gamma_compensation();

    void singleShot(int channels);

    void closeEvent( QCloseEvent * event );

    void saveCapturePattern();

    void readExistPattern();

    void parameter_setting(int row,int col){
        this->imageHeight=row;
        this->imageWidth=col;
    }

    void phaseCode_sin_reconstruction();

    void composed_sin_reconstruction();

    void Fourier_sin_reconstruction();

    void phaseCode_grayCode_sin_reconstruction();

    void multiFrequencyHeterodyne_reconstruction();

    void binary_grayCode_sin_reconstruction();

    void color_sin_phaseCode_reconstruction();

    void pure_sin_phaseCode_reconstruction();

    void composite_color_code_reconstruction();

    void color_double_frequency_reconstruction();

    //彩色三频外插
    void color_three_frequency_reconstruction();

public slots:
    void on_init_slot();
    void on_capture_slot();

    void on_reload_slot();
    void on_prepare_slot();

    void on_iamge_1_slot();
    void on_iamge_2_slot();

    void on_iamge_3_slot();
    void on_iamge_4_slot();

    void on_image_5_slot();
    void on_image_6_slot();

    void on_compute_slot();
    void on_result_slot();

    void on_binary_box_slot(int val);

    void on_set_binary_slot();

    void on_save_image_slot();

    void on_3qh_slot();

    void on_wmw_slot();

    void on_cv_show();


    void on_message(int flag,QString msg);

    void computeChase(cv::Mat& resultMat);

    void on_receive_slot(QImage img);


    void on_set_camera_parameter(int shutter,int gain);

    void on_get_key_press(int key);

    void on_get_calibrate_status(bool flag);

    void on_camera_connect_slot();

    /**************************************************/

    void on_reconstruction_slot();

    void on_crosstalk_gamma_slot();

    void on_paintImage_slot();

    void on_mask_button_slot();

    void on_capture_button_slot();

    void on_calibrate_button_slot();

    void on_other_button_slot();


private:

    void setupMatrix();
    void populateScene();
    QGraphicsScene *scene;

    //文体信息显示窗
    QTextBrowser *textbrowser;

    //布局分裂器
    QSplitter *h1Splitter;    
    QSplitter *vSplitter;
    QSplitter *mainSplitter;

    //总体栅格布局
    QGridLayout *layout;

    //按键
    QPushButton *init_button;
    QPushButton *capture_button;

    QPushButton *reload_button;
    QPushButton *prepare_button;


    QPushButton *image_1_button;
    QPushButton *image_2_button;

    QPushButton *image_3_button;
    QPushButton *image_4_button;


    QPushButton *image_5_button;
    QPushButton *image_6_button;

    QPushButton *compute_button;
    QPushButton *result_button;

    QPushButton *saveImg_button;


    QPushButton *wrap_3qh_button;
    QPushButton *wrap_wmw_button;


    QSpinBox *binary_box;
    QPushButton *set_binary_button;

/************************************************/
    QPushButton *camera_connect_button;
    QPushButton *crosstalk_gamma_button;
    QPushButton *reconstruction_button;
    QPushButton *paintImage_button;
    QPushButton *mask_button;
    QPushButton *caliCapture_button;
    QPushButton *calibrate_button;
    QPushButton *other_button;
    QLabel *pattern_labbel;
    QCheckBox *gamma_box;
/***********************************************/

    View *view;


    QList<cv::Mat> image_list;
    QList<cv::Mat> show_image_list;

    cv::Mat resultMat;

    QTime time;

    QString readPath;


    //相机控制类

     bool connectCameraFlag;
     ImageItem itemShow;

     //硬件触发标志位
     bool triggerFlag;

     //保存图片序号
     int captureNum;

     int scanNum;

     //二进制图像序列
     int binary_num;

     //采集标定图片标志
     bool getCalibrateImageFlag;

     bool scanFlag;

     CalibrateMachine calibrateMachine;

//     CalibrateMachine calibrateSystem;

     int saveCalibrateImageNum;

     cv::Mat CalibrateImage;

     QString saveDir;

     QString currentSaveDir;

     QString board_image_path,calib_image_path,measure_path,cross_path;

     cv::Size boardSize;

    QString savePath;

    int getNumber;


    std::vector<cv::Mat> unWrapImage;

    /****************************************************************************************************************************/
    //DLP
    dlp::LCr4500        projector_4500;
    dlp::PG_FlyCap2_C   camera_pg;
    dlp::Parameters settings;

    std::atomic_bool captureFlag;

    cv::Mat show_mat;

    cv::Mat board_mat;

    bool patterns_prepare;

    std::vector<cv::Mat> scan_mat_list;


    CaptureThread captureMachine;

    bool showFlag;
    int capture_Num;

    bool projecting;

    Reconstruct3D rebulid;

    /*************************************/
        Yuan_pattern *pattern_class;

        paintDialog *paintdialog;

        std::vector<cv::Mat> mat_list;//拍摄的图片

        std::vector<cv::Mat> paint_pattern;

        cv::Mat maskImage;
        cv::Mat boardImage;
        cv::Mat crosstalk_mat;
        cv::Mat reflect_mat;

        cv::Mat unwrapPhaseX,unwrapPhaseY;

        std::vector<cv::Point2f> left_p,right_p;
        std::vector<Point3f> pCloud;


        std::vector<cv::Mat> cali_decode_result;
        std::vector<std::vector<double>> value_paint;
        std::vector<std::vector<pointXY>> pointXY_paint;

        int imageWidth=1280,imageHeight=800;
        double gammaValue=0;
        double blue_gamma_Aconst;
        std::vector<double> blue_gamma_Bcoefficient;

        channels*** crosstalk_table;



        bool project_flag=false;
        bool gamma_flag=false;
        bool crosstalk_flag=false;
        bool phaseErrorTable_flag=false;

        int cornerFlag = 0;
};

#endif // WIDGET_H


















