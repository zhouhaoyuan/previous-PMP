#include "widget.h"
#include <iostream>
#include <fstream>
#include <vector>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{

    this->setGeometry(150,70,1000,600);

    typedef QVector<QRect> MyArray;

    qRegisterMetaType<MyArray>("MyArray");


    binary_num= 0;
    projecting= false;

    populateScene();

    //照片显示窗口
    view = new View("Show Image");

    connect(view,SIGNAL(send_cv_show()),this,SLOT(on_cv_show()));
                 connect(view,SIGNAL(send_key_press(int)),this,SLOT(on_get_key_press(int)));

    view->view()->setScene(scene);



    //分裂器初始化
    h1Splitter = new QSplitter;
    vSplitter = new QSplitter;
    vSplitter->setOrientation(Qt::Vertical);

    mainSplitter= new QSplitter;
    mainSplitter->addWidget(view);


    textbrowser= new QTextBrowser;


    init_button= new QPushButton;
    capture_button= new QPushButton;
    image_1_button= new QPushButton;

    image_3_button= new QPushButton;
    image_4_button= new QPushButton;
    image_2_button= new QPushButton;

    image_5_button= new QPushButton;
    image_6_button= new QPushButton;

    compute_button= new QPushButton;
    result_button= new QPushButton;


    reload_button= new QPushButton;
    prepare_button= new QPushButton;

    saveImg_button= new QPushButton;

    wrap_3qh_button= new QPushButton;
    wrap_wmw_button= new QPushButton;

    binary_box= new QSpinBox;
    binary_box->setRange(0,200);
    binary_box->setValue(binary_num);
    set_binary_button= new QPushButton;


    //********************************The new button******************************//
    camera_connect_button=new QPushButton;
    reconstruction_button=new QPushButton;
    crosstalk_gamma_button=new QPushButton;
    paintImage_button=new QPushButton;
    mask_button=new QPushButton;
    caliCapture_button=new QPushButton;
    calibrate_button=new QPushButton;
    other_button=new QPushButton;
    gamma_box=new QCheckBox(this);

    camera_connect_button->setText("Camera");
    reconstruction_button->setText("reconstruction");
    crosstalk_gamma_button->setText("crosstalk_gamma");
    paintImage_button->setText("paintImage");
    mask_button->setText("mask");
    caliCapture_button->setText("capture");
    calibrate_button->setText("calibrate");
    other_button->setText("other");
    gamma_box->setText("gamma");
    //********************************The new button******************************//


    init_button->setText("Connect");
    capture_button->setText("CaptureFringe");

    reload_button->setText("Reload Patterns");
    prepare_button->setText("Prepare Patterns");

    image_1_button->setText("ProjectWhite");
    image_2_button->setText("Stop Project");

    image_3_button->setText("PhaseCompute");
    image_4_button->setText("AddPoints");

    image_5_button->setText("Rebuild");
    image_6_button->setText("StereoCalib");

    compute_button->setText("Compute");
    result_button->setText("Result");

    set_binary_button->setText("ProjectOne");
    saveImg_button->setText("SaveImage");

    wrap_3qh_button->setText("3QH");
    wrap_wmw_button->setText("WMW");

    connect(init_button,SIGNAL(clicked()),this,SLOT(on_init_slot()));
    connect(capture_button,SIGNAL(clicked()),this,SLOT(on_capture_slot()));

    connect(reload_button,SIGNAL(clicked()),this,SLOT(on_reload_slot()));
    connect(prepare_button,SIGNAL(clicked()),this,SLOT(on_prepare_slot()));

    connect(image_1_button,SIGNAL(clicked()),this,SLOT(on_iamge_1_slot()));
    connect(image_2_button,SIGNAL(clicked()),this,SLOT(on_iamge_2_slot()));

    connect(image_3_button,SIGNAL(clicked()),this,SLOT(on_iamge_3_slot()));
    connect(image_4_button,SIGNAL(clicked()),this,SLOT(on_iamge_4_slot()));

    connect(image_5_button,SIGNAL(clicked()),this,SLOT(on_image_5_slot()));
    connect(image_6_button,SIGNAL(clicked()),this,SLOT(on_image_6_slot()));


    connect(compute_button,SIGNAL(clicked()),this,SLOT(on_compute_slot()));
    connect(result_button,SIGNAL(clicked()),this,SLOT(on_result_slot()));

    connect(binary_box,SIGNAL(valueChanged(int)),this,SLOT(on_binary_box_slot(int)));
    connect(set_binary_button,SIGNAL(clicked()),this,SLOT(on_set_binary_slot()));

    connect(saveImg_button,SIGNAL(clicked()),this,SLOT(on_save_image_slot()));

    connect(wrap_3qh_button,SIGNAL(clicked()),this,SLOT(on_3qh_slot()));
    connect(wrap_wmw_button,SIGNAL(clicked()),this,SLOT(on_wmw_slot()));

    /****************************************************************************/

    connect(camera_connect_button,SIGNAL(clicked()),this,SLOT(on_camera_connect_slot())); 
    connect(reconstruction_button,SIGNAL(clicked()),this,SLOT(on_reconstruction_slot()));
    connect(crosstalk_gamma_button,SIGNAL(clicked()),this,SLOT(on_crosstalk_gamma_slot()));
    connect(paintImage_button,SIGNAL(clicked()),this,SLOT(on_paintImage_slot()));
    connect(mask_button,SIGNAL(clicked()),this,SLOT(on_mask_button_slot()));
    connect(caliCapture_button,SIGNAL(clicked()),this,SLOT(on_capture_button_slot()));
    connect(calibrate_button,SIGNAL(clicked()),this,SLOT(on_calibrate_button_slot()));
    connect(other_button,SIGNAL(clicked()),this,SLOT(on_other_button_slot()));

    /*****************************************************************************/



    QGridLayout *gridlayout= new QGridLayout;

    gridlayout->setHorizontalSpacing(10);
    gridlayout->setVerticalSpacing(2);


    gridlayout->addWidget(init_button,0,0);
    gridlayout->addWidget(capture_button,0,1);
    gridlayout->addWidget(reload_button,1,0);
    gridlayout->addWidget(prepare_button,1,1);
    gridlayout->addWidget(image_1_button,2,0);
    gridlayout->addWidget(image_2_button,2,1);
//    gridlayout->addWidget(image_3_button,3,0);
//    gridlayout->addWidget(image_4_button,3,1);

    gridlayout->addWidget(image_5_button,4,0);
    gridlayout->addWidget(image_6_button,4,1);

//    gridlayout->addWidget(compute_button,5,0);
//    gridlayout->addWidget(result_button,5,1);


    gridlayout->addWidget(binary_box,6,0);
    gridlayout->addWidget(set_binary_button,6,1);

    gridlayout->addWidget(wrap_3qh_button,7,0);
    gridlayout->addWidget(wrap_wmw_button,7,1);

    gridlayout->addWidget(saveImg_button,8,0);
    gridlayout->addWidget(gamma_box,8,1);
    /************************************************/

    gridlayout->addWidget(camera_connect_button,9,0);
    gridlayout->addWidget(mask_button,9,1);
    gridlayout->addWidget(calibrate_button,10,0);
    gridlayout->addWidget(crosstalk_gamma_button,10,1);
    gridlayout->addWidget(paintImage_button,11,1);
    gridlayout->addWidget(caliCapture_button,11,0);
    gridlayout->addWidget(reconstruction_button,12,0);
    gridlayout->addWidget(other_button,12,1);

    /**************************************************/

    vSplitter->addWidget(textbrowser);

    layout = new QGridLayout;
    layout->addWidget(mainSplitter,0,0,80,60);
    layout->addLayout(gridlayout,0,60,20,20);
    layout->addWidget(vSplitter,20,60,60,20);

//    layout->setColumnMinimumWidth(1,20);
    layout->setColumnStretch(1,10);

//    gridlayout->setSizeConstraint();

    setLayout(layout);


    //测试
//    textbrowser->setText("weiwei!");
//    textbrowser->append("正在加载");


    /***********************************************************************************************/

    triggerFlag= false;

    getCalibrateImageFlag= false;

    saveCalibrateImageNum= 0;

    scanFlag= false;

    scanNum= 0;

    getNumber= 0;



    readPath= "../ScanData/Calib_image";

    patterns_prepare= false;

    showFlag= true;

    /***************************************************************************************************************************/
    //文件夹生成

    saveDir= "../ScanData/";

    QDir dir(saveDir);
    if(!dir.exists())
    {
       bool ok = dir.mkdir(saveDir);//只创建一级子目录，即必须保证上级目录存在
       qDebug()<<"create dir: "<<saveDir<<" "<<ok;
    }
    saveDir= dir.absolutePath();



    board_image_path= saveDir+"/board_image/";

    QDir board_image_dir(board_image_path);
    if(!board_image_dir.exists())
    {
       bool ok = board_image_dir.mkdir(board_image_path);//只创建一级子目录，即必须保证上级目录存在
       qDebug()<<"create dir: "<<board_image_path<<" "<<ok;
    }
    board_image_path= board_image_dir.absolutePath();




    calib_image_path= saveDir+"/calib_image/";

    QDir calib_image_dir(calib_image_path);
    if(!calib_image_dir.exists())
    {
       bool ok = calib_image_dir.mkdir(calib_image_path);//只创建一级子目录，即必须保证上级目录存在
       qDebug()<<"create dir: "<<calib_image_path<<" "<<ok;
    }
    calib_image_path= calib_image_dir.absolutePath();




    measure_path= saveDir+"/measure_image/";

    QDir measure_image_dir(measure_path);
    if(!measure_image_dir.exists())
    {
       bool ok = measure_image_dir.mkdir(measure_path);//只创建一级子目录，即必须保证上级目录存在
       qDebug()<<"create dir: "<<measure_path<<" "<<ok;
    }
    measure_path= measure_image_dir.absolutePath();


    cross_path= saveDir+"/cross_image/";

    QDir cross_image_dir(cross_path);
    if(!cross_image_dir.exists())
    {
       bool ok = cross_image_dir.mkdir(cross_path);//只创建一级子目录，即必须保证上级目录存在
       qDebug()<<"create dir: "<<cross_path<<" "<<ok;
    }
    cross_path= cross_image_dir.absolutePath();




    boardSize.width = 11;
    boardSize.height = 13;


    /*****************************************************************************************************************************/
    QDir dir_calib(calib_image_path);
    QStringList dir_list_calib; //标定文件夹的绝对路径
    QStringList name_list_calib;//标定文件夹的代号
    foreach (QFileInfo fileInfo, dir_calib.entryInfoList(QDir::Dirs | QDir::Files))
    {
        QString strName = fileInfo.fileName();
//        qDebug()<<strName;
        if ((strName == QString(".")) || (strName == QString("..")))
            continue;
        if (fileInfo.isDir())
        {

            QString str = fileInfo.absolutePath() + "/" + strName + "/";
            dir_list_calib.push_back(str);

//            qDebug()<<fileInfo.fileName();

//            name_list_calib.push_back(fileInfo.fileName());

            name_list_calib.push_back(strName);

        }

    }

    saveCalibrateImageNum= dir_list_calib.size();

    qDebug()<<"saveCalibrateImageNum: "<<saveCalibrateImageNum;






    /******************************************************************************************************************************/


//    connectCameraFlag= false;
//    camera= new PGCamera;
//    connect(camera,SIGNAL(send_image(QImage)),this,SLOT(on_receive_slot(QImage)));

    connect(&calibrateMachine,SIGNAL(send_found_circle(bool)),this,SLOT(on_get_calibrate_status(bool)));

    connect(&captureMachine,SIGNAL(send_capture_mat(QImage)),this,SLOT(on_receive_slot(QImage)));

    //连接相机
   // connectDevice();

    QPixmap *gdutImage;
    gdutImage=new QPixmap("../StructuredLight3D/GDUT.jpg");
    itemShow.setPixmap(*gdutImage);

    pattern_labbel = new QLabel;

    pattern_class = new Yuan_pattern;

    //初始化串扰查找表
    crosstalk_table = new channels**[256];

    for(int b = 0; b < 256; b++){
        crosstalk_table[b] = new channels*[256];
        for(int g = 0; g < 256; g++){
            crosstalk_table[b][g] = new channels[256];
        }
    }
    for(int b = 0; b < 256; b++){
        for(int g = 0; g < 256; g++){
            for(int r = 0; r < 256; r++){
                crosstalk_table[b][g][r].blue = 0;
                crosstalk_table[b][g][r].green = 0;
                crosstalk_table[b][g][r].red = 0;
            }
        }
    }
}

void Widget::decodeThreeHoterodyUnwrap(std::vector<cv::Mat> in_mat,cv::Mat &unwrap_x,cv::Mat &unwrap_y,cv::Mat mask)
{

//    PhaseShift PS_machine;

    std::vector<cv::Mat> wrapList;



    for(int i= 0;i< in_mat.size();i+= 4)
    {

        std::vector<cv::Mat> images;
        images.push_back(in_mat.at(i+0));
        images.push_back(in_mat.at(i+1));
        images.push_back(in_mat.at(i+2));
        images.push_back(in_mat.at(i+3));


        cv::Mat result;


//        PS_machine.fourStepPhaseShift(images,result);

        fourStepPhaseShiftFloat(in_mat.at(i+ 0),in_mat.at(i+ 1),in_mat.at(i+ 2),in_mat.at(i+ 3),result);



        wrapList.push_back(result);

    }



    std::vector<cv::Mat> x_list,y_list;

    for(int i= 0;i< 3;i++)
    {
        x_list.push_back(wrapList.at(i));
    }

    for(int i= 3;i< wrapList.size();i++)
    {
        y_list.push_back(wrapList.at(i));
    }

    float p_1= 52.0;
    float p_2= 58.0;
    float p_3= 620.0;

    float wavelength_p1= p_1;
//    float wavelength_p1= 40;
//    float wavelength_p1= 52;

    qDebug()<<"phase computer time: "<<time.elapsed()<<" ms";
    time.restart();

        cv::Mat unwrap_phase_x,unwrap_phase_y;
//        PS_machine.setThreeFrequenceWavelength(p_1,p_2,p_3);

         unwrapThreeFrequencyHeterodyne(x_list,unwrap_phase_x);
       unwrapThreeFrequencyHeterodyne(y_list,unwrap_phase_y);

        x_list.clear();
        y_list.clear();

        qDebug()<<"unwrapThreeFrequencyHeterodyne time: "<<time.elapsed();



    cv::Mat nor_x,nor_y;

    phaseNormalization(2.0*912.0/(wavelength_p1),unwrap_phase_x,nor_x);
    phaseNormalization(2.0*1140.0/(wavelength_p1),unwrap_phase_y,nor_y);


//    QString name_x=  "3_unwrap_phase_x";
//    QString name_y=  "3_unwrap_phase_y";

//    cvShowMatFloat(nor_x,name_x.toStdString(),1);
//    cvShowMatFloat(nor_y,name_y.toStdString(),1);


    unwrap_x= nor_x.clone();
    unwrap_y= nor_y.clone();
}


Widget::~Widget()
{

}

void Widget::on_cv_show()
{

}

void Widget::on_message(int flag,QString msg)
{
    switch(flag)
    {
        case 0:
        {
            showMessage(msg);
        }
        break;
        case 1:
        {

        }
        break;
        case 2:
        {

        }
        break;
        case 3:
        {

        }
        break;
        case 4:
        {

        }
        break;

    }
}

void Widget::showMessage(QString meg)
{
    textbrowser->append(meg);
}

void Widget::showImage(cv::Mat img)
{


    cv::Mat in_img= img.clone();


    std::vector<cv::Mat> planes;

    planes.push_back(in_img);
    planes.push_back(in_img);
    planes.push_back(in_img);

    cv::Mat temp_img;

    cv::merge(planes,temp_img);


//    for(int i= 0;i< in_img.rows;i++)
//    {
//        uchar* in_ptr= in_img.ptr<uchar>(i);
//        cv::Vec3b* color_ptr= temp_img.ptr<cv::Vec3b>(i);

//        for(int j= 0;j< in_img.cols;j++)
//        {
//            if(255== in_ptr[j])
//            {
//                color_ptr[j][0] = 0;
//                color_ptr[j][1] = 0;
//                color_ptr[j][2] = 255;
//            }
//        }
//    }

    QImage image;

    if(temp_img.channels()==3)
    {
        //cvt Mat BGR 2 QImage RGB
        cvtColor(temp_img,temp_img,CV_BGR2RGB);
        image =QImage((const unsigned char*)(temp_img.data),
                      temp_img.cols,temp_img.rows,
                      temp_img.cols*temp_img.channels(),
                      QImage::Format_RGB888);
    }
    else
    {
        image =QImage((const unsigned char*)(temp_img.data),
                      temp_img.cols,temp_img.rows,
                      temp_img.cols*temp_img.channels(),
                      QImage::Format_Grayscale8);
    }




    itemShow.setPixmap(QPixmap::fromImage(image));






}

void Widget::on_reload_slot()
{
    QMessageBox::StandardButton ok= QMessageBox::warning(this,"Warning","Do You want to Update Firmware?",QMessageBox::Yes |QMessageBox::No, QMessageBox::No);


    if(ok== QMessageBox::Yes)
    {
//        reloadFarewareFourStepFrequence(6, true);

        prepareScanPatterns(true);
        patterns_prepare= true;
        reconnectDevice();
    }



}

void Widget::on_prepare_slot()
{
//    reloadFarewareFourStepFrequence(6, false);
    prepareScanPatterns(false);
    patterns_prepare= true;
}
//单纯拍图片
void Widget::on_capture_slot()
{
    if(!patterns_prepare)
    {
//        reloadFarewareFourStepFrequence(6, false);

        prepareScanPatterns(false);
        patterns_prepare= true;
    }

    int capture_count= 1;


    while(capture_count-- >0)
    {
        captureMachine.setCaptureFlag(false);

        dlp::Time::Sleep::Microseconds(200000);

        projector_4500.StopPatternSequence();

        dlp::Time::Sleep::Microseconds(200000);


        scan_mat_list.clear();

        highspeeDebugdScanPatterns(&camera_pg,&projector_4500,true,48,24,scan_mat_list);

        qDebug()<<capture_count<<" scan_mat_list: "<<scan_mat_list.size();


    }

    QString path;



    for(int i= 0;i< scan_mat_list.size();i++)
    {
        if(i< 10)
        {
            path= measure_path+ "/capture_0"+ QString::number(i)+ ".bmp";
        }else{
            path= measure_path+ "/capture_"+ QString::number(i)+ ".bmp";
        }

        imwrite(path.toStdString(),scan_mat_list.at(i));

    }
    qDebug()<<measure_path;







}



void Widget::on_init_slot()
{


//    connectDevice();
    reconnectDevice();

//    captureFlag= true;
//    projectSignalPattern(&camera_pg,&projector_4500,1);

//    if(connectCameraFlag)
//    {
//        //断开连接


//        _video_input.stop_camera();

//            disconnect(&_video_input, SIGNAL(new_image(QImage)), this, SLOT(on_receive_slot(QImage)));
//            disconnect(view,SIGNAL(send_camera_parameter(int,int)),this,SLOT(on_set_camera_parameter(int,int)));
//            disconnect(view,SIGNAL(send_key_press(int)),this,SLOT(on_get_key_press(int)));
//            init_button->setText("Connect");
//            connectCameraFlag= false;

////            init_button->setText("Error Disconnect");


//    }else{
//        //初始化相机


//         if(_video_input.start_camera())
//         {
//             init_button->setText("Disconnect");

//             float scur, smin, smax, gcur, gmin, gmax;

//             _video_input.GetCurShutter(&scur);
//             _video_input.GetShutterRange(&smin, &smax);

//             _video_input.GetCurGain(&gcur);
//             _video_input.GetGainRange(&gmin, &gmax);

//             view->setRangeValue(scur, smin, smax, gcur, gmin, gmax);

//             connectCameraFlag= true;
//             _video_input.SetCurExposure(0.719);
//             _video_input.SetCurShutter(16.712);
////                          _video_input.SetCurShutter(1);
//             _video_input.SetCurGain(0);
//             _video_input.SetCurFrameRate(25);

//             connect(&_video_input, SIGNAL(new_image(QImage)), this, SLOT(on_receive_slot(QImage)));
//             connect(view,SIGNAL(send_camera_parameter(int,int)),this,SLOT(on_set_camera_parameter(int,int)));
//             connect(view,SIGNAL(send_key_press(int)),this,SLOT(on_get_key_press(int)));




//             PTTriggerMode mode= _video_input.GetTriggerMode();

//             qDebug()<<"mode: "<<mode;

//         }else{
//             init_button->setText("Error Connect");
//         }
//    }



}

//定义场景scene，可以显示图片
void Widget::populateScene()
{
    scene = new QGraphicsScene;

//    item= new ImageItem;

    scene->addItem(&itemShow);

//    cv::Mat img= cv::imread("../Image/2007814224354787_2.jpg");
//    cv::cvtColor(img,img,CV_BGR2RGB);
//    QImage image= QImage((const unsigned char*)(img.data),img.cols,img.rows,QImage::Format_RGB888);


//    QPixmap pixmap;
//    pixmap.fromImage(image);
//    scene->addPixmap(QPixmap::fromImage(image));



}


//点亮投影仪，投白光，相机也启动了，拍一帧
void Widget::on_iamge_1_slot()
{
//    if(scanFlag)
//    {
//        scanFlag= false;
//        image_1_button->setText("Scan");
//    }else{
//        scanNum++;
//        captureNum= 0;

//        image_1_button->setText("ScanReady");
//        scanFlag= true;
//    }

//    captureFlag= true;




//    capture_Num= 0;
//    captureMachine.setCamera(&camera_pg);
//    captureMachine.start();

    projectSignalPattern(&projector_4500,1);









}
//停止投影
void Widget::on_iamge_2_slot()
{



    captureMachine.setCaptureFlag(false);

    dlp::Time::Sleep::Microseconds(200000);

    projector_4500.StopPatternSequence();

//    captureFlag= false;


//    if(saveCalibrateImageNum< 5)
//    {
//        QString show= "Error: Image Number< 5 ";
//        showMessage(show);
//    }else{

//        showMessage("Start Calibrate");

//        cv::Size boardSize;
//        boardSize.width = 11;
//        boardSize.height = 13;

//         double error= calibrateMachine.CalibrateCamera(boardSize);

//        QString show= "Reprojection Error: "+ QString::number(error,'f');

//        showMessage(show);
//    }






}

//主要用来显示绝对相位
void Widget::cvShowMatFloat(cv::Mat img, std::string name,int period)
{

    int nr= img.rows;
    int nc= img.cols;

    cv::Mat showMat(nr,nc,CV_8U);


    if(img.isContinuous())
    {
        nc= nc*nr;
        nr= 1;
    }

    for(int i= 0;i< nr;i++)
    {
        double* ptr_img= img.ptr<double>(i);
        uchar* ptr_show= showMat.ptr<uchar>(i);
        for(int j= 0;j< nc;j++)
        {
            ptr_show[j]= 255*ptr_img[j]/(PI*period);
        }
    }




//    for(int r= 0;r< nr;r++)
//    {
//        for(int c= 0;c< nc;c++)
//        {
////            showMat.at<uchar>(r,c)= 255*img.at<double>(r,c)/(PI*pow(2.0,period));
//            showMat.at<uchar>(r,c)= 255*img.at<double>(r,c)/(PI*((period)));
//        }
//    }
//    imwrite("../"+name+".bmp",showMat);

    float scale= 0.5;
    cv::Size dsize;

    dsize = cv::Size(showMat.cols*scale,showMat.rows*scale);

    cv::resize(showMat,showMat,dsize);

    cv::namedWindow(name);
    cv::imshow(name,showMat);




}
//cv显示
void Widget::cvShowMat(cv::Mat img,std::string name)
{
    cv::Mat showMat= img.clone();

    float scale= 0.25;
    cv::Size dsize;

    dsize = cv::Size(showMat.cols*scale,showMat.rows*scale);

    cv::resize(showMat,showMat,dsize);

    cv::namedWindow(name);
    cv::imshow(name,showMat);

    cv::waitKey(0);

//    setMouseCallback(name,on_mouse,(void*) &showMat.data);
}

void Widget::addProjectImage(QString dir_name)
{


//    QString dir_name= QFileDialog::getExistingDirectory(this,"Select Folder",readPath);

    QDir dir(dir_name);

     dir.setFilter(QDir::Files | QDir::NoSymLinks);


     dir.setSorting(QDir::Name);


     QStringList filter;
     filter<<QString("*.bmp");
     QFileInfoList  path=  dir.entryInfoList(filter);



     for (int i = 0; i < path.size(); ++i) {

         QString filename = path.at(i).absoluteFilePath();

         qDebug() << filename;
     }






    int flag= path.size()%4;

    if(1 != flag)
    {
        qDebug()<<"Error: Image number is not time of 4!";
        return;
    }


    QString saveName= path.at(0).absoluteFilePath();

    int length= saveName.length();

    saveName= saveName.left(length- 5);

//    qDebug()<<"saveName: "<<saveName;

    /**************************************************************************************************/

    cv::Mat cameraImage= cv::imread(path.at(0).absoluteFilePath().toStdString(),0);


//    cvShowMat(cameraImage,"cameraImage");

    std::vector<cv::Point2f> cameraCirclePoints;

//    bool found= calibrateSystem.findCircleBoardCorners(cameraImage,boardSize,cameraCirclePoints);
    bool found= calibrateMachine.findCircleBoardCorners(cameraImage,boardSize,cameraCirclePoints);




    cv::Mat    show_camera_image= cv::imread(path.at(0).absoluteFilePath().toStdString(),1);
    drawChessboardCorners(show_camera_image,boardSize,cameraCirclePoints,found);


    cvShowMat(show_camera_image,"cameraImage");

    if(found)
    {

    }
    else
    {
        qDebug()<<"Error: failure to find circle points";
        return;
    }


    cv::Mat unwrapMap_x;
    cv::Mat unwrapMap_y;

//    wrapRoi= imread("../roi.bmp",0);

//    cvShowMat(wrapRoi,"wrapRoi");


    int imgNum= path.size()-1;


    int subNum= 8;

    /************************************************************************************************************/
    //y方向相位展开

    //周期相位
    for(int i= 1; i< 1+ imgNum/2- subNum; i+= 4)
    {


            QString file_full, file_name;
            QFileInfo fi;

            cv::Mat wrapImage;

            file_full= path.at(0+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();

            savePath= fi.absolutePath();



            cv::Mat Mat_0= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_0,Mat_0,5);

//            cv::GaussianBlur(Mat_0,Mat_0,cv::Size(5,5),1.5);
        //    cvShowMat(Mat_0,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(0+i).absoluteFilePath();

            file_full= path.at(1+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();

            cv::Mat Mat_1= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_1,Mat_1,5);


//            cv::GaussianBlur(Mat_1,Mat_1,cv::Size(5,5),1.5);
        //    cvShowMat(Mat_1,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(1+i).absoluteFilePath();

            file_full= path.at(2+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();
            cv::Mat Mat_2= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_2,Mat_2,5);

//            cv::GaussianBlur(Mat_2,Mat_2,cv::Size(5,5),1.5);
//            cvShowMat(Mat_2,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(2+i).absoluteFilePath();

            file_full= path.at(3+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();
            cv::Mat Mat_3= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_3,Mat_3,5);


//            cv::GaussianBlur(Mat_3,Mat_3,cv::Size(5,5),1.5);
//            cvShowMat(Mat_3,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(3+i).absoluteFilePath();


//            fourStepPhaseShift(true,Mat_0,Mat_1,Mat_2,Mat_3,wrapImage);

            fourStepPhaseShiftFloat(Mat_0,Mat_1,Mat_2,Mat_3,wrapImage);


            unWrapImage.push_back(wrapImage);


//            QString name= "wrapImage_"+QString::number(unWrapImage.size());

//            QString savePath= saveName+ name+ ".bmp";

//            cv::imwrite(savePath.toStdString(),wrapImage);

    }



//    multipleWavelengthUnwrap(unWrapImage,unwrapMap_x);
    multipleWavelengthUnwrapFloat(unWrapImage,unwrapMap_y);



//    qDebug()<<"unwrapMap_y "<< unwrapMap_y.type();
//     qDebug()<<"rows: "<< unwrapMap_x.rows<<" cols: "<< unwrapMap_x.cols;



//    matToTxt(unwrapMap_x,"unwrapMap_x",2);

    unWrapImage.clear();


    /******************************************************************************************************************************/
    //x方向相位展开
    //周期相位
    for(int i= 1+ imgNum/2; i< path.size()- subNum; i+= 4)
    {


            QString file_full, file_name;
            QFileInfo fi;

            cv::Mat wrapImage;

            file_full= path.at(0+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();

            savePath= fi.absolutePath();



            cv::Mat Mat_0= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_0,Mat_0,5);

//            cv::GaussianBlur(Mat_0,Mat_0,cv::Size(5,5),1.5);
        //    cvShowMat(Mat_0,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(0+i).absoluteFilePath();

            file_full= path.at(1+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();

            cv::Mat Mat_1= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_1,Mat_1,5);

//            cv::GaussianBlur(Mat_1,Mat_1,cv::Size(5,5),1.5);
        //    cvShowMat(Mat_1,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(1+i).absoluteFilePath();

            file_full= path.at(2+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();
            cv::Mat Mat_2= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_2,Mat_2,5);

//            cv::GaussianBlur(Mat_2,Mat_2,cv::Size(5,5),1.5);
//            cvShowMat(Mat_2,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(2+i).absoluteFilePath();

            file_full= path.at(3+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();
            cv::Mat Mat_3= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_3,Mat_3,5);


//            cv::GaussianBlur(Mat_3,Mat_3,cv::Size(5,5),1.5);
        //    cvShowMat(Mat_3,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(3+i).absoluteFilePath();


//            fourStepPhaseShift(true,Mat_0,Mat_1,Mat_2,Mat_3,wrapImage);
            fourStepPhaseShiftFloat(Mat_0,Mat_1,Mat_2,Mat_3,wrapImage);

            unWrapImage.push_back(wrapImage);

//            QString name= "wrapImage_"+QString::number(unWrapImage.size());

//            QString savePath= saveName+ name+ ".bmp";

//            cv::imwrite(savePath.toStdString(),wrapImage);


    }

//    multipleWavelengthUnwrap(unWrapImage,unwrapMap_y);


    multipleWavelengthUnwrapFloat(unWrapImage,unwrapMap_x);

//    matToTxt(unwrapMap_y,"unwrapMap_y",2);

    unWrapImage.clear();



    /****************************************************************************************************************************/
    //坐标转换

//    std::vector<cv::Point2f> dlpCirclePoints;

    int period= (path.size()-1)/8- subNum/4;

    qDebug()<<"period: "<<period;

//    phaseToPixel(unwrapMap_x,unwrapMap_y,period,cameraCirclePoints,dlpCirclePoints);

//    phaseToPixelFloat(unwrapMap_x,unwrapMap_y,period,cameraCirclePoints,dlpCirclePoints);




//    qDebug()<<"num: "<<dlpCirclePoints.size();


//    cvShowUnwrapMap(unwrapMap_x,period,"unwrapMap_x");

//    cvShowUnwrapMap(unwrapMap_y,period,"unwrapMap_y");


    cvShowMatFloat(unwrapMap_x,"unwrapMap_x",period-1);
    cvShowMatFloat(unwrapMap_y,"unwrapMap_y",period-1);
}

//没用上
void Widget::on_iamge_3_slot()
{


    QString dir_name= QFileDialog::getExistingDirectory(this,"Select Folder",readPath);

    QDir dir(dir_name);

     dir.setFilter(QDir::Files | QDir::NoSymLinks);


     dir.setSorting(QDir::Name);


     QStringList filter;
     filter<<QString("*.bmp");
     QFileInfoList  path=  dir.entryInfoList(filter);



     for (int i = 0; i < path.size(); ++i) {

         QString filename = path.at(i).absoluteFilePath();

         qDebug() << filename;
     }






    int flag= path.size()%4;

    if(1 != flag)
    {
        qDebug()<<"Error: Image number is not time of 4!";
        return;
    }


    QString saveName= path.at(0).absoluteFilePath();

    int length= saveName.length();

    saveName= saveName.left(length- 5);

//    qDebug()<<"saveName: "<<saveName;

    /**************************************************************************************************/

    cv::Mat cameraImage= cv::imread(path.at(0).absoluteFilePath().toStdString(),0);


//    cvShowMat(cameraImage,"cameraImage");

    std::vector<cv::Point2f> cameraCirclePoints;

//    bool found= calibrateSystem.findCircleBoardCorners(cameraImage,boardSize,cameraCirclePoints);
    bool found= calibrateMachine.findCircleBoardCorners(cameraImage,boardSize,cameraCirclePoints);




    cv::Mat    show_camera_image= cv::imread(path.at(0).absoluteFilePath().toStdString(),1);
    drawChessboardCorners(show_camera_image,boardSize,cameraCirclePoints,found);


    cvShowMat(show_camera_image,"cameraImage");

    if(found)
    {

    }
    else
    {
        qDebug()<<"Error: failure to find circle points";
        return;
    }


    cv::Mat unwrapMap_x;
    cv::Mat unwrapMap_y;

//    wrapRoi= imread("../roi.bmp",0);

//    cvShowMat(wrapRoi,"wrapRoi");


    int imgNum= path.size()-1;


    int subNum= 8;

    /************************************************************************************************************/
    //y方向相位展开

    //周期相位
    for(int i= 1; i< 1+ imgNum/2- subNum; i+= 4)
    {


            QString file_full, file_name;
            QFileInfo fi;

            cv::Mat wrapImage;

            file_full= path.at(0+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();

            savePath= fi.absolutePath();



            cv::Mat Mat_0= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_0,Mat_0,5);

//            cv::GaussianBlur(Mat_0,Mat_0,cv::Size(5,5),1.5);
        //    cvShowMat(Mat_0,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(0+i).absoluteFilePath();

            file_full= path.at(1+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();

            cv::Mat Mat_1= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_1,Mat_1,5);


//            cv::GaussianBlur(Mat_1,Mat_1,cv::Size(5,5),1.5);
        //    cvShowMat(Mat_1,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(1+i).absoluteFilePath();

            file_full= path.at(2+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();
            cv::Mat Mat_2= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_2,Mat_2,5);

//            cv::GaussianBlur(Mat_2,Mat_2,cv::Size(5,5),1.5);
//            cvShowMat(Mat_2,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(2+i).absoluteFilePath();

            file_full= path.at(3+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();
            cv::Mat Mat_3= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_3,Mat_3,5);


//            cv::GaussianBlur(Mat_3,Mat_3,cv::Size(5,5),1.5);
//            cvShowMat(Mat_3,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(3+i).absoluteFilePath();


//            fourStepPhaseShift(true,Mat_0,Mat_1,Mat_2,Mat_3,wrapImage);

            fourStepPhaseShiftFloat(Mat_0,Mat_1,Mat_2,Mat_3,wrapImage);


            unWrapImage.push_back(wrapImage);


//            QString name= "wrapImage_"+QString::number(unWrapImage.size());

//            QString savePath= saveName+ name+ ".bmp";

//            cv::imwrite(savePath.toStdString(),wrapImage);

    }



//    multipleWavelengthUnwrap(unWrapImage,unwrapMap_x);
    multipleWavelengthUnwrapFloat(unWrapImage,unwrapMap_y);



//    qDebug()<<"unwrapMap_y "<< unwrapMap_y.type();
//     qDebug()<<"rows: "<< unwrapMap_x.rows<<" cols: "<< unwrapMap_x.cols;



//    matToTxt(unwrapMap_x,"unwrapMap_x",2);

    unWrapImage.clear();


    /******************************************************************************************************************************/
    //x方向相位展开
    //周期相位
    for(int i= 1+ imgNum/2; i< path.size()- subNum; i+= 4)
    {


            QString file_full, file_name;
            QFileInfo fi;

            cv::Mat wrapImage;

            file_full= path.at(0+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();

            savePath= fi.absolutePath();



            cv::Mat Mat_0= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_0,Mat_0,5);

//            cv::GaussianBlur(Mat_0,Mat_0,cv::Size(5,5),1.5);
        //    cvShowMat(Mat_0,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(0+i).absoluteFilePath();

            file_full= path.at(1+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();

            cv::Mat Mat_1= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_1,Mat_1,5);

//            cv::GaussianBlur(Mat_1,Mat_1,cv::Size(5,5),1.5);
        //    cvShowMat(Mat_1,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(1+i).absoluteFilePath();

            file_full= path.at(2+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();
            cv::Mat Mat_2= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_2,Mat_2,5);

//            cv::GaussianBlur(Mat_2,Mat_2,cv::Size(5,5),1.5);
//            cvShowMat(Mat_2,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(2+i).absoluteFilePath();

            file_full= path.at(3+ i).absoluteFilePath();
            fi = QFileInfo(file_full);
            file_name = fi.fileName();
            cv::Mat Mat_3= cv::imread(file_full.toStdString(),0);
//            cv::medianBlur(Mat_3,Mat_3,5);


//            cv::GaussianBlur(Mat_3,Mat_3,cv::Size(5,5),1.5);
        //    cvShowMat(Mat_3,file_name.toStdString());
            qDebug()<<"Path: "<<path.at(3+i).absoluteFilePath();


//            fourStepPhaseShift(true,Mat_0,Mat_1,Mat_2,Mat_3,wrapImage);
            fourStepPhaseShiftFloat(Mat_0,Mat_1,Mat_2,Mat_3,wrapImage);

            unWrapImage.push_back(wrapImage);

//            QString name= "wrapImage_"+QString::number(unWrapImage.size());

//            QString savePath= saveName+ name+ ".bmp";

//            cv::imwrite(savePath.toStdString(),wrapImage);


    }

//    multipleWavelengthUnwrap(unWrapImage,unwrapMap_y);


    multipleWavelengthUnwrapFloat(unWrapImage,unwrapMap_x);

//    matToTxt(unwrapMap_y,"unwrapMap_y",2);

    unWrapImage.clear();



    /****************************************************************************************************************************/
    //坐标转换

//    std::vector<cv::Point2f> dlpCirclePoints;

    int period= (path.size()-1)/8- subNum/4;

    qDebug()<<"period: "<<period;

//    phaseToPixel(unwrapMap_x,unwrapMap_y,period,cameraCirclePoints,dlpCirclePoints);

//    phaseToPixelFloat(unwrapMap_x,unwrapMap_y,period,cameraCirclePoints,dlpCirclePoints);




//    qDebug()<<"num: "<<dlpCirclePoints.size();


//    cvShowUnwrapMap(unwrapMap_x,period,"unwrapMap_x");

//    cvShowUnwrapMap(unwrapMap_y,period,"unwrapMap_y");


    cvShowMatFloat(unwrapMap_x,"unwrapMap_x",period-1);
    cvShowMatFloat(unwrapMap_y,"unwrapMap_y",period-1);

}
//没用上
void Widget::on_iamge_4_slot()
{

    QString dir_name= QFileDialog::getExistingDirectory(this,"Select Folder",saveDir);

    QDir dir(dir_name);

     dir.setFilter(QDir::Files | QDir::NoSymLinks);


     dir.setSorting(QDir::Name);


     QStringList filter;
     filter<<QString("*.bmp");
     QFileInfoList  path=  dir.entryInfoList(filter);



     for (int i = 0; i < path.size(); ++i) {

         QString filename = path.at(i).absoluteFilePath();

         qDebug() << filename;
     }


}
//多波长
void Widget::on_image_5_slot()
{
//    int extra_img= 0;

//      QString dir_name= QFileDialog::getExistingDirectory(this,"Select Folder","../ScanData/");

//      if(dir_name.isEmpty())
//      {
//          return;
//      }

//      QString calib_path;
//      QString mask_path;
//      QString out_path;
//      QStringList list= dir_name.split("/");
//      calib_path= "../ScanData/calibResult.txt";
//      mask_path= "../ScanData/mask.bmp";
//      out_path= "../ScanData/"+ list.at(list.size()-1);

//      cv::Mat measure_mask= cv::imread(mask_path.toStdString(),0);

//      qDebug()<<"out_path: "<<out_path;
//      QDir dir(dir_name);

//       dir.setFilter(QDir::Files | QDir::NoSymLinks);


//       dir.setSorting(QDir::Name);


//       QStringList filter;
//       filter<<QString("*.bmp");
//       QFileInfoList  path=  dir.entryInfoList(filter);



//       QStringList path_list;
//       QStringList absoluteList;
//       int patternNum= 6;

//        for (int i = 0; i < path.size(); i++) {

//            QString filename = path.at(i).absoluteFilePath();

//            absoluteList.push_back(filename);

////             qDebug() << filename;
//        }

//        int startNum= absoluteList.size()/2;



//        for(int i= 0;i< 4*patternNum;i++)
//        {
//                 path_list.push_back(absoluteList.at(i));
//        }

//        for(int i= startNum ;i< startNum+ 4*patternNum;i++)
//        {
//                 path_list.push_back(absoluteList.at(i));
//        }





////       qDebug()<<"path num: "<<path_list.size();


//            std::vector<cv::Mat> in_mat;

//            for(int i= 0;i< path_list.size();i++)
//            {
////                qDebug()<<path_list.at(i);
//                in_mat.push_back(cv::imread(path_list.at(i).toStdString(),0));

//            }



            /***************************************************************************************/

            QString calib_path;
            QString mask_path;
            QString out_path;

            calib_path= "../ScanData/calibResult.txt";
            mask_path= "../ScanData/mask.bmp";
            out_path= "../ScanData/";

            cv::Mat measure_mask= cv::imread(mask_path.toStdString(),0);

            if(!patterns_prepare)
            {
                prepareScanPatterns(false);
                patterns_prepare= true;
            }

            captureMachine.setCaptureFlag(false);

            dlp::Time::Sleep::Microseconds(200000);

            projector_4500.StopPatternSequence();

            dlp::Time::Sleep::Microseconds(200000);



            int capture_count= 1;


            while(capture_count-- >0)
            {
                scan_mat_list.clear();

                highspeeDebugdScanPatterns(&camera_pg,&projector_4500,true,0,48,scan_mat_list);

                qDebug()<<capture_count<<" scan_mat_list: "<<scan_mat_list.size();


            }

            if(48!= scan_mat_list.size())
            {
                qDebug()<<"Capture Image Error: "<<scan_mat_list.size();
            }



            std::vector<cv::Mat> in_mat;

            in_mat= scan_mat_list;

            /*****************************************************************************************************************/

            cv::Mat unwrap_x,unwrap_y;




            QTime time;
            time.start();

            decodeMultiplyWavelengthUnwrap(in_mat,unwrap_x,unwrap_y);

//            qDebug()<<"decodeMultiplyWavelengthUnwrap: "<<time.elapsed();

            std::vector<cv::Point2f> left_p,right_p;
            std::vector<Point3f> pCloud;



//            cvShowMatFloat(unwrap_x,"unwrap_x",1);

//            cvShowMatFloat(unwrap_y,"unwrap_y",1);


            rebulid.createCorrectPoints(unwrap_x,unwrap_y ,left_p,right_p,measure_mask);

            pCloud.clear();



            rebulid.readCalibPara();


            rebulid.restruct3D(left_p, right_p, pCloud);
            qDebug()<<"points: "<<pCloud.size();


            QString pointCloud_path= QFileDialog::getSaveFileName(this,"Save PointCloud Path","../pCloud.ply","*.ply");


            rebulid.savePointCloud(pCloud, pointCloud_path);

            qDebug()<<pointCloud_path;


            QString open_file= pointCloud_path;

            QProcess *process = new QProcess;
            QStringList str;
            str << open_file;
            process->start("D:/meshlab/meshlab.exe",str);


            projectSignalPattern(&projector_4500,1);



}

//系统标定
void Widget::on_image_6_slot()
{

    QString dir_name= QFileDialog::getExistingDirectory(this,"Select Folder","../ScanData/calib_image/");


    if(dir_name.isEmpty())
    {
        return;
    }

    QDir dir(dir_name);

    QStringList dir_list;


    foreach (QFileInfo fileInfo, dir.entryInfoList(QDir::Dirs | QDir::Files))
    {
        QString strName = fileInfo.fileName();
//        qDebug()<<strName;
        if ((strName == QString(".")) || (strName == QString("..")))
            continue;
        if (fileInfo.isDir())
        {

            QString str = fileInfo.absolutePath() + "/" + strName + "/";
            dir_list.push_back(str);
        }

    }


    for(int dir_i= 0;dir_i< dir_list.size(); dir_i++)
    {
        QDir dir(dir_list.at(dir_i));

//        qDebug()<<"folder: "<< dir;

         dir.setFilter(QDir::Files | QDir::NoSymLinks);


         dir.setSorting(QDir::Name);


         QStringList filter;
         filter<<QString("*.bmp");
         QFileInfoList  path=  dir.entryInfoList(filter);


        QStringList path_list;
        QStringList absoluteList;
        int patternNum= 6;

         for (int i = 0; i < path.size(); i++) {

             QString filename = path.at(i).absoluteFilePath();

             absoluteList.push_back(filename);

//             qDebug() << filename;
         }


         path_list.push_back(absoluteList.at(0));

         int startNum= 1+ (absoluteList.size()-1)/2;



         for(int i= 1;i< 1+ 4*patternNum;i++)
         {
                  path_list.push_back(absoluteList.at(i));
         }

         for(int i= startNum ;i< startNum+ 4*patternNum;i++)
         {
                  path_list.push_back(absoluteList.at(i));
         }


         QTime time;
         time.start();

         std::vector<cv::Mat> image_mat;

         for(int i= 0;i< path_list.size();i++)
         {
//             qDebug()<<path_list.at(i);
             image_mat.push_back(cv::imread(path_list.at(i).toStdString(),0));
         }

         qDebug()<<"read mat time: "<<time.elapsed()<<" ms";;

         time.restart();


         calibrateMachine.decodeFourStepMultiplyHierarchical(image_mat);

         qDebug()<<"decode time: "<<time.elapsed()<<" ms";
    }


    double error= calibrateMachine.structuredLightStereoCalibrate();
}


void Widget::computeChase(cv::Mat &resultMat)
{
    cv::Mat image_1,image_2,image_3,image_4;

    QString saveFile= QFileDialog::getSaveFileName(this,"Set Save FileName: ","../result.txt");


    if(saveFile.isEmpty())
    {
        return ;
    }


    QFile file(saveFile);
    if(!file.open(QIODevice::WriteOnly)){
        qDebug()<<"Open Error!!!";
        return ;
    }

    QTextStream out(&file);

    image_1= image_list.at(0).clone();
    image_2= image_list.at(1).clone();
    image_3= image_list.at(2).clone();
    image_4= image_list.at(3).clone();

    cv::Mat result(image_1.rows,image_1.cols,CV_8U);

    int image_row= image_1.rows;
    int image_col= image_1.cols;


    qDebug()<<"value: "<<image_1.rows<<"chase: "<<image_1.cols;

    time.start();

    for(int i=0; i< image_row; i++)
    {
        for(int j= 0; j< image_col; j++)
        {
           double value= (double)(image_4.at<uchar>(i,j)- image_2.at<uchar>(i,j))/(double)(image_1.at<uchar>(i,j)- image_3.at<uchar>(i,j));
           uchar chase= (3.14159/2.0 + atan(value))*255/3.14159;

//           qDebug()<<"("<<i<<","<<j<<") "<<"value: "<<value<<"chase: "<<chase;
//           out<<chase<<"\t";
           result.at<uchar>(i,j)= chase;
        }
        out<<"\r\n";
    }

    int t= time.elapsed();

        file.close();

    resultMat= result.clone();
    qDebug()<<"finished: "<<t;
}

void Widget::on_binary_box_slot(int val)
{

    if(!projecting)
    {
        projecting= true;
        binary_num= val;

        projecting= false;
        qDebug()<<"binary_num: "<<binary_num;


    }


}
//三频
void Widget::on_3qh_slot()
{
//    QString dir_name= QFileDialog::getExistingDirectory(this,"Select Folder","../ScanData/");

//    if(dir_name.isEmpty())
//    {
//        return;
//    }

//    QString calib_path;
//    QString mask_path;
//    QString out_path;
//    calib_path= "../ScanData/calibResult.txt";
//    mask_path= "../ScanData/mask.bmp";
//    out_path= "../ScanData/";

//    cv::Mat measure_mask= cv::imread(mask_path.toStdString(),0);

//    qDebug()<<"out_path: "<<out_path;
//    QDir dir(dir_name);

//     dir.setFilter(QDir::Files | QDir::NoSymLinks);


//     dir.setSorting(QDir::Name);


//     QStringList filter;
//     filter<<QString("*.bmp");
//     QFileInfoList  path=  dir.entryInfoList(filter);



//     QStringList path_list;
//     QStringList absoluteList;

//      for (int i = 0; i < path.size(); i++) {

//          QString filename = path.at(i).absoluteFilePath();

//          absoluteList.push_back(filename);

////             qDebug() << filename;
//      }

//      int startNum= absoluteList.size()/2;



//      for(int i= 0;i< absoluteList.size();i++)
//      {
//               path_list.push_back(absoluteList.at(i));
//      }

////      for(int i= startNum ;i< startNum+ 4*patternNum;i++)
////      {
////               path_list.push_back(absoluteList.at(i));
////      }





//     qDebug()<<"path num: "<<path_list.size();


//          std::vector<cv::Mat> in_mat;

//          for(int i= 0;i< path_list.size();i++)
//          {
//              qDebug()<<path_list.at(i);
//              in_mat.push_back(cv::imread(path_list.at(i).toStdString(),0));

//          }

          /******************************************************************************************************/

          QString calib_path;
          QString mask_path;
          QString out_path;

          calib_path= "../ScanData/calibResult.txt";
          mask_path= "../ScanData/mask.bmp";
          out_path= "../ScanData/";

          cv::Mat measure_mask= cv::imread(mask_path.toStdString(),0);

          if(!patterns_prepare)
          {
              prepareScanPatterns(false);
              patterns_prepare= true;
          }



          captureMachine.setCaptureFlag(false);

          dlp::Time::Sleep::Microseconds(200000);

          projector_4500.StopPatternSequence();

          dlp::Time::Sleep::Microseconds(200000);


          int capture_count= 1;



          while(capture_count-- >0)
          {
              scan_mat_list.clear();

              highspeeDebugdScanPatterns(&camera_pg,&projector_4500,true,48,24,scan_mat_list);

              qDebug()<<capture_count<<" scan_mat_list: "<<scan_mat_list.size();


          }

          if(24!= scan_mat_list.size())
          {
              qDebug()<<"Capture Image Error: "<<scan_mat_list.size();
          }

          std::vector<cv::Mat> in_mat;

          in_mat= scan_mat_list;

          cv::Mat unwrap_x,unwrap_y;

          decodeThreeHoterodyUnwrap(in_mat,unwrap_x,unwrap_y,measure_mask);




          std::vector<cv::Point2f> left_p,right_p;
          std::vector<Point3f> pCloud;

//          time.restart();

          rebulid.createCorrectPoints(unwrap_x,unwrap_y ,left_p,right_p,measure_mask);


//          cvShowMat(measure_mask,"measure_mask");



          pCloud.clear();



          rebulid.readCalibPara();


          rebulid.restruct3D(left_p, right_p, pCloud);
          qDebug()<<"points: "<<pCloud.size();
//          qDebug()<<"restruct3D time: "<<time.elapsed()<<" ms";
          rebulid.savePointCloud(pCloud, out_path+ "_3qh_pCloud.ply");


          QString open_file= out_path+ "_3qh_pCloud.ply";

          QProcess *process = new QProcess;
          QStringList str;
          str << open_file;
          process->start("D:/Program Files/VCG/MeshLab/meshlab.exe",str);

          
          projectSignalPattern(&projector_4500,1);


//          dlp::Time::Sleep::Microseconds(200000);

//          captureMachine.setCaptureFlag(false);

//          dlp::Time::Sleep::Microseconds(200000);

//          projector_4500.StopPatternSequence();

}
//变倍率多波长
void Widget::on_wmw_slot()
{



    /*******************************************************************************************/
//    QString dir_name= QFileDialog::getExistingDirectory(this,"Select Folder","../ScanData/");

//    if(dir_name.isEmpty())
//    {
//        return;
//    }

//    QString calib_path;
//    QString mask_path;
//    QString out_path;

//    calib_path= "../ScanData/calibResult.txt";
//    mask_path= "../ScanData/mask.bmp";
//    out_path= "../ScanData/";

//    cv::Mat measure_mask= cv::imread(mask_path.toStdString(),0);

//    qDebug()<<"out_path: "<<out_path;
//    QDir dir(dir_name);

//    dir.setFilter(QDir::Files | QDir::NoSymLinks);


//    dir.setSorting(QDir::Name);


//    QStringList filter;
//    filter<<QString("*.bmp");
//    QFileInfoList  path=  dir.entryInfoList(filter);



//    QStringList path_list;
//    QStringList absoluteList;

//    for (int i = 0; i < path.size(); i++) {

//        QString filename = path.at(i).absoluteFilePath();

//        absoluteList.push_back(filename);

//        //             qDebug() << filename;
//    }

//    int startNum= absoluteList.size()/2;



//    for(int i= 0;i< absoluteList.size();i++)
//    {
//        path_list.push_back(absoluteList.at(i));
//    }

//    //      for(int i= startNum ;i< startNum+ 4*patternNum;i++)
//    //      {
//    //               path_list.push_back(absoluteList.at(i));
//    //      }





//    qDebug()<<"path num: "<<path_list.size();


//    std::vector<cv::Mat> in_mat;

//    for(int i= 0;i< path_list.size();i++)
//    {
//        qDebug()<<path_list.at(i);
//        in_mat.push_back(cv::imread(path_list.at(i).toStdString(),0));

//    }

    /*******************************************************************************************************/

    QString calib_path;
    QString mask_path;
    QString out_path;

    calib_path= "../ScanData/calibResult.txt";
    mask_path= "../ScanData/mask.bmp";
    out_path= "../ScanData/";

    cv::Mat measure_mask= cv::imread(mask_path.toStdString(),0);

    if(!patterns_prepare)
    {
        prepareScanPatterns(false);
        patterns_prepare= true;
    }

    captureMachine.setCaptureFlag(false);

    dlp::Time::Sleep::Microseconds(200000);

    projector_4500.StopPatternSequence();

    dlp::Time::Sleep::Microseconds(200000);



    int capture_count= 1;


    while(capture_count-- >0)
    {
        scan_mat_list.clear();

        highspeeDebugdScanPatterns(&camera_pg,&projector_4500,true,72,24,scan_mat_list);

        qDebug()<<capture_count<<" scan_mat_list: "<<scan_mat_list.size();


    }

    if(24!= scan_mat_list.size())
    {
        qDebug()<<"Capture Image Error: "<<scan_mat_list.size();
    }

    std::vector<cv::Mat> in_mat;

    in_mat= scan_mat_list;


    std::vector<cv::Mat> x_mat;
    std::vector<cv::Mat> y_mat;


    //    }


    /*******************************************************************************************************************/

    //    std::vector<cv::Mat> wrap_list_x;
    //    std::vector<cv::Mat> wrap_list_y;


    QTime time;
    time.start();
    qDebug()<<"Phase compute start:";


    for(int i= 0;i< in_mat.size()/2;i+= 4)
    {

        std::vector<cv::Mat> images;
        images.push_back(in_mat.at(i+ 0));
        images.push_back(in_mat.at(i+ 1));
        images.push_back(in_mat.at(i+ 2));
        images.push_back(in_mat.at(i+ 3));

        cv::Mat result;
//        PS_machine.fourStepPhaseShift(images,result);


                      fourStepPhaseShiftFloat(in_mat.at(i+ 0),in_mat.at(i+ 1),in_mat.at(i+ 2),in_mat.at(i+ 3),result);

        //              QString name= "x_"+ QString::number(i);

        //              cvShowMatFloat(result,name.toStdString(),1);


        x_mat.push_back(result);


    }


    for(int i= in_mat.size()/2; i< in_mat.size();i+= 4)
    {

        std::vector<cv::Mat> images;
        images.push_back(in_mat.at(i+ 0));
        images.push_back(in_mat.at(i+ 1));
        images.push_back(in_mat.at(i+ 2));
        images.push_back(in_mat.at(i+ 3));

        cv::Mat result;
//        PS_machine.fourStepPhaseShift(images,result);


                      fourStepPhaseShiftFloat(in_mat.at(i+ 0),in_mat.at(i+ 1),in_mat.at(i+ 2),in_mat.at(i+ 3),result);


        //              QString name= "y_"+ QString::number(i);
        //              cvShowMatFloat(result,name.toStdString(),1);

        y_mat.push_back(result);


    }



    qDebug()<<"Phase compute time: "<<time.elapsed()<<" ms";


    float rate_1= 16;
    float rate_2= 48;




    std::vector<cv::Mat> unwrap_Mat_list_x;

    cv::Mat start_wrap_x= x_mat.at(0);

    cv::Mat m_wrap_x= x_mat.at(0);

    cv::Mat f1_wrap_x= x_mat.at(0);

    unwrap_Mat_list_x.push_back(start_wrap_x);

    int nr= start_wrap_x.rows;
    int nc= start_wrap_x.cols;



    cv::Mat k_mat(nr,nc,CV_8U);



    unwarpHierarchical(rate_1,start_wrap_x,x_mat.at(1),m_wrap_x,k_mat);

    cv::Mat test_1,test_2,test_3;
    test_1= m_wrap_x.clone();
    test_2= m_wrap_x.clone();
    test_3= m_wrap_x.clone();





//    QString f1_name= QString::number(rate_1) +"_"+ QString::number(rate_2)+ "_unwrap_x";
    unwarpHierarchical(3,test_2,x_mat.at(2),f1_wrap_x,k_mat);
//    cvShowMatFloat(f1_wrap_x,f1_name.toStdString(),rate_2);
    cv::Mat n_mat_f1_x;
    phaseNormalization(rate_2,f1_wrap_x,n_mat_f1_x);







    /********************************************************************************************************************************/

    //unwrap y

    std::vector<cv::Mat> unwrap_Mat_list_y;

    cv::Mat start_wrap_y= y_mat.at(0);

    cv::Mat m_wrap_y= y_mat.at(0);

    cv::Mat f1_wrap_y= y_mat.at(0);

    unwrap_Mat_list_y.push_back(start_wrap_y);





    unwarpHierarchical(rate_1,start_wrap_y,y_mat.at(1),m_wrap_y,k_mat);



    test_1= m_wrap_y.clone();
    test_2= m_wrap_y.clone();
    test_3= m_wrap_y.clone();



    cv::Mat n_mat_m_y;
    phaseNormalization(rate_1,m_wrap_y,n_mat_m_y);





//    f1_name= QString::number(rate_1) +"_"+ QString::number(rate_2)+ "_unwrap_y";
    unwarpHierarchical(3,test_2,y_mat.at(2),f1_wrap_y,k_mat);

    cv::Mat n_mat_f1_y;
    phaseNormalization(rate_2,f1_wrap_y,n_mat_f1_y);




    /********************************************************************************************************/



    cv::Mat unwrap_x,unwrap_y;


    unwrap_x= n_mat_f1_x.clone();

    unwrap_y= n_mat_f1_y.clone();
    std::vector<cv::Point2f> left_p,right_p;
    std::vector<Point3f> pCloud;




    rebulid.createCorrectPoints(unwrap_x,unwrap_y ,left_p,right_p,measure_mask);




    pCloud.clear();



    rebulid.readCalibPara();


    rebulid.restruct3D(left_p, right_p, pCloud);
    qDebug()<<"points: "<<pCloud.size();


    rebulid.savePointCloud(pCloud, out_path+ "_wmw_pCloud.ply");


    QString open_file= out_path+ "_wmw_pCloud.ply";

    QProcess *process = new QProcess;
    QStringList str;
    str << open_file;
    process->start("D:/Program Files/VCG/MeshLab/meshlab.exe",str);


    projectSignalPattern(&projector_4500,1);

}


void Widget::on_save_image_slot()
{
    cv::Mat saveMat= show_mat.clone();

    qDebug()<<"saveMat";


//    dlp::Time::Sleep::Microseconds(200000);

    captureMachine.setCaptureFlag(false);

    dlp::Time::Sleep::Microseconds(200000);

    projector_4500.StopPatternSequence();




//    dlp::Time::Sleep::Microseconds(200000);

    QDir dir(board_image_path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);

    QStringList filter;
    filter<<QString("*.bmp");
    QFileInfoList  path=  dir.entryInfoList(filter);
    QStringList calib_list;

    for (int i = 0; i < path.size(); i++) {

        QString filename = path.at(i).absoluteFilePath();
        calib_list.push_back(filename);
//            qDebug()<<i<<": "<<filename;

    }

    int save_num= path.size();

    QString saveDir,saveName;

    if(save_num< 10)
    {
        saveDir= board_image_path+ "/" + "0" + QString::number(save_num)+".bmp";
        saveName=  QString::number(save_num)+".bmp";
    }else{
        saveDir= board_image_path+ "/" + "" + QString::number(save_num)+".bmp";
        saveName= QString::number(save_num)+".bmp";
    }


    QString save_path= QFileDialog::getSaveFileName(this,"Set Save Path: ",saveDir,"*.bmp");

    if(save_path.isEmpty())
        return;

    cv::imwrite(save_path.toStdString(),saveMat);
    qDebug()<<"save: "<<save_path;
}

void Widget::on_set_binary_slot()
{
    binary_num++;
    binary_box->setValue(binary_num);

    captureFlag= false;
    captureMachine.setCaptureFlag(false);
    dlp::Time::Sleep::Microseconds(100000);
    projector_4500.StopPatternSequence();

    projectSignalBinaryPattern(&projector_4500,binary_num);
    dlp::Time::Sleep::Microseconds(100000);
    qDebug()<<"binary_num: "<<binary_num;


}




//没用
void Widget::on_compute_slot()
{

    getNumber= 0;
    qDebug()<<"getNumber: "<<getNumber;

}
//空白
void Widget::on_result_slot()
{

}

void Widget::on_receive_slot(QImage img)
{

//    qDebug()<<"rows: "<<img.height();
//    qDebug()<<"cols: "<<img.width();

//    show_mat= QImageToCvMat(img);
//    showImage(show_mat);


//    qDebug()<<"capture_Num: "<<capture_Num++;
//    qDebug()<<"rows: "<<img.height();
//    qDebug()<<"cols: "<<img.width();

    itemShow.setPixmap(QPixmap::fromImage(img));

//    if(capture_Num++> 10)
//    {

////        itemShow.setPixmap(QPixmap::fromImage(img));

//        show_mat= QImageToCvMat(img);



//        showImage(show_mat);
//    }





    /*
//    delete scene;
//    scene= new QGraphicsScene;


//    QGraphicsPixmapItem item;


    if(scanFlag)
    {

        captureNum++;
        QString name;

        if(captureNum< 10)
        {
            name= measure_path+ tr("/capture_0%1.bmp").arg(captureNum);
        }else{
            name= measure_path+ tr("/capture_%1.bmp").arg(captureNum);
        }

        QPixmap pixmap= QPixmap::fromImage(img);

        pixmap.save(name);

        if(captureNum> 71)
        {
            image_1_button->click();


        }


    }

    if(triggerFlag)
    {
        captureNum++;

        QString name;

        if(captureNum< 10)
        {
            name= currentSaveDir+ "/dlp_calibration_capture_0"+ QString::number(captureNum)+".bmp";

        }else{
            name= currentSaveDir+ "/dlp_calibration_capture_"+ QString::number(captureNum)+".bmp";

        }



        QPixmap pixmap= QPixmap::fromImage(img);

        pixmap.save(name);

        if(captureNum> 71)
        {
            capture_button->click();


//            /******************************************************************************************/
//            addProjectImage(currentSaveDir);

//            /*******************************************************************************************/

//            if(QMessageBox::No == QMessageBox::warning(this,"warning","Do want to save these images?",QMessageBox::Yes|QMessageBox::No))
//            {
//              showMessage("don't save images");
//              saveCalibrateImageNum--;
//            }


//        }
//    }

//    if(getCalibrateImageFlag)
//    {
//        qDebug()<<"calibrate";

//        CalibrateImage= QImageToCvMat(img);

////        imwrite("../board.bmp",CalibrateImage);

//        calibrateMachine.setCalibrateBoardImage(CalibrateImage);

//        getCalibrateImageFlag= false;
//    }

//    getNumber++;
//    qDebug()<<"Capture image: "<<getNumber;


//    itemShow.setPixmap(QPixmap::fromImage(img));

//    view->update();


//    scene->addPixmap(QPixmap::fromImage(img));

//    view->view()->setScene(scene);

}

cv::Mat Widget::QImageToCvMat(QImage image)
{
    cv::Mat mat;
//    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_RGB2BGR);
        break;
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}


void Widget::on_set_camera_parameter(int shutter,int gain)
{
//    _video_input.SetCurShutter(static_cast<float> (shutter));

//    _video_input.SetCurGain(static_cast<float> (gain));
}

void Widget::on_get_key_press(int key)
{
    qDebug()<<"key: "<<key;
    if(32== key)
    {
//        getCalibrateImageFlag= true;



        board_mat= show_mat.clone();

        if(board_mat.data)
        {

            captureMachine.setCaptureFlag(false);

            dlp::Time::Sleep::Microseconds(200000);

            projector_4500.StopPatternSequence();

            dlp::Time::Sleep::Microseconds(100000);

//            qDebug()<<"start";

            calibrateMachine.setCalibrateBoardImage(board_mat);
        }


    }
}
//得到标定的条纹图案
void Widget::on_get_calibrate_status(bool flag)
{

//    qDebug()<<"flag: "<<flag;

    if(flag)
    {
        if(!patterns_prepare)
        {
//            reloadFarewareFourStepFrequence(6, false);

            prepareScanPatterns(false);
            patterns_prepare= true;
        }


//        captureMachine.setCaptureFlag(false);

//        dlp::Time::Sleep::Microseconds(200000);

//        projector_4500.StopPatternSequence();

//        dlp::Time::Sleep::Microseconds(500000);


//        dlp::Time::Sleep::Microseconds(2000000);

//        cvShowMat(show_mat,"show_mat");

        highspeeDebugdScanPatterns(&camera_pg,&projector_4500,true,0,48,scan_mat_list);


        qDebug()<<"scan_mat_list.size(): "<<scan_mat_list.size();

//        if(48!= scan_mat_list.size())
//        {
//            scan_mat_list.clear();
//            highspeeDebugdScanPatterns(&camera_pg,&projector_4500,true,0,48,scan_mat_list);

//        }

        QString showStr= "Save Calibrate Image number: "+ QString::number(saveCalibrateImageNum);

        showMessage(showStr);

        QString path;

        if(saveCalibrateImageNum< 10)
        {
            path= board_image_path+ "/board_0"+ QString::number(saveCalibrateImageNum)+ ".bmp";
        }else{
            path= board_image_path+ "/board_"+ QString::number(saveCalibrateImageNum)+ ".bmp";
        }

        bool save= imwrite(path.toStdString(),board_mat);

        qDebug()<<"save board: "<<path<<" "<<save;


        /*****************************************************************************************************/
        //创建标定文件夹
        QString temp_saveDir;
        if(saveCalibrateImageNum< 10)
        {
            temp_saveDir= calib_image_path+"/0"+QString::number(saveCalibrateImageNum)+ "/";
        }else{
            temp_saveDir= calib_image_path+"/"+QString::number(saveCalibrateImageNum)+ "/";
        }
        QDir temp_dir(temp_saveDir);
        if(!temp_dir.exists())
        {
           bool ok = temp_dir.mkdir(temp_saveDir);//只创建一级子目录，即必须保证上级目录存在
           qDebug()<<"create dir: "<<temp_saveDir<<" "<<ok;
        }

        //当前采集图片存储目录
        currentSaveDir= temp_saveDir;



//        if(saveCalibrateImageNum< 10)
//        {
            path= currentSaveDir+ "/board.bmp";
//        }else{
//            path= currentSaveDir+ "board_"+ QString::number(saveCalibrateImageNum)+ ".bmp";
//        }


        save= imwrite(path.toStdString(),board_mat);
        qDebug()<<"save board: "<<path<<" "<<save;


        for(int i= 0;i< scan_mat_list.size();i++)
        {
            if(i< 10)
            {
                path= currentSaveDir+ "capture_0"+ QString::number(i)+ ".bmp";
            }else{
                path= currentSaveDir+ "capture_"+ QString::number(i)+ ".bmp";
            }

            imwrite(path.toStdString(),scan_mat_list.at(i));

        }


        saveCalibrateImageNum++;

        /*****************************************************************************************************/



        if(!save){
            showMessage("Error: failure to save image");
        }



    }else{
        showMessage("Can Not Find Circle.");
    }

    qDebug()<<"saveCalibrateImageNum: "<<saveCalibrateImageNum;

        projectSignalPattern(&projector_4500,0);
}

/*********************************************************************************************/
//四步解相对
void Widget::fourStepPhaseShiftFloat(cv::Mat image_1,cv::Mat image_2,cv::Mat image_3,cv::Mat image_4,cv::Mat& resultMat)
{
    cv::Mat result(image_1.rows,image_1.cols,CV_64F);

    int image_row= image_1.rows;
    int image_col= image_1.cols;

    for(int i=0; i< image_row; i++)
    {
        for(int j= 0; j< image_col; j++)
        {
            float chase;
            double value;

            value= (double)(image_4.at<uchar>(i,j)- image_2.at<uchar>(i,j))/(image_1.at<uchar>(i,j)- image_3.at<uchar>(i,j));
            chase= PI/2.0 + atan(value);

            result.at<double>(i,j)= chase;
        }
    }

    resultMat= result.clone();



}



void Widget::multipleWavelengthUnwrapFloat(std::vector<cv::Mat> setImage,cv::Mat& resultMat)
{
    std::vector<cv::Mat> unwrapImage;

    cv::Mat firstFringe= setImage.at(0);

    unwrapImage.push_back(firstFringe);

    for(int i=0; i<setImage.size()-1;i++)
    {
        cv::Mat unwrapMat(firstFringe.rows,firstFringe.cols,CV_32F,cv::Scalar(0));

        cv::Mat lastUnwrapMat= unwrapImage.at(i);

        cv::Mat wrapMat= setImage.at(i+1);

        cv::Mat k_value(firstFringe.rows,firstFringe.cols,CV_8U);

        float temp= 0;

        int k= 0;


        for(int c= 0;c< firstFringe.cols;c++)
        {
            for(int r= 0;r< firstFringe.rows;r++)
            {

                temp= 0.5+ lastUnwrapMat.at<float>(r,c)/(0.5*PI) - wrapMat.at<float>(r,c)/(PI);

                k= temp;


                unwrapMat.at<float>(r,c)= k*PI + wrapMat.at<float>(r,c) ;


                k_value.at<uchar>(r,c)= k;



            }

        }

//                matToTxt(k_value,"k_value"+QString::number(i+1),1);
//                matToTxt(wrapMat,"wrapMat"+QString::number(i+1),3);
//                matToTxt(unwrapMat,"unwrapMat"+QString::number(i+1),3);

        unwrapImage.push_back(unwrapMat);
    }

//    cv::Mat finalUnwrapMat(firstFringe.rows,firstFringe.cols,CV_32F,cv::Scalar(0));

//    finalUnwrapMat= unwrapImage.at(setImage.size()-1);

    resultMat= unwrapImage.at(setImage.size()-1);

}

/***********************************************************************************************************************************/
//DLP


void Widget::connectDevice()
{
    DLP_NEW_PARAMETERS_ENTRY(ConnectIdProjector,           "CONNECT_ID_PROJECTOR",  std::string, "0");
    DLP_NEW_PARAMETERS_ENTRY(ConnectIdCamera,              "CONNECT_ID_CAMERA",     std::string, "0");

    DLP_NEW_PARAMETERS_ENTRY(ConfigFileProjector,           "CONFIG_FILE_PROJECTOR",                std::string, "config/config_projector.txt");
    DLP_NEW_PARAMETERS_ENTRY(ConfigFileCamera,              "CONFIG_FILE_CAMERA",                   std::string, "config/config_color_camera.txt");

    DLP_NEW_PARAMETERS_ENTRY(ConfigFileStructuredLight1,    "CONFIG_FILE_STRUCTURED_LIGHT_1",       std::string, "config/algorithm_vertical.txt");
    DLP_NEW_PARAMETERS_ENTRY(ConfigFileStructuredLight2,    "CONFIG_FILE_STRUCTURED_LIGHT_2",       std::string, "config/algorithm_horizontal.txt");
    DLP_NEW_PARAMETERS_ENTRY(ConfigFileCalibProjector,      "CONFIG_FILE_CALIBRATION_PROJECTOR",    std::string, "config/calibration_projector.txt");

    DLP_NEW_PARAMETERS_ENTRY(OutputNameImageCameraCalibBoard, "OUTPUT_NAME_IMAGE_CAMERA_CALIBRATION_BOARD", std::string, "camera_calibration_board");
    DLP_NEW_PARAMETERS_ENTRY(OutputNameImageCameraCalib,    "OUTPUT_NAME_IMAGE_CAMERA_CALIBRATION", std::string, "camera_calibration_capture_");

    DLP_NEW_PARAMETERS_ENTRY(ConfigFileCalibCamera,         "CONFIG_FILE_CALIBRATION_CAMERA",       std::string, "config/calibration_camera.txt");
    DLP_NEW_PARAMETERS_ENTRY(CalibDataFileCamera,           "CALIBRATION_DATA_FILE_CAMERA",         std::string, "calibration/data/camera.xml");
    DLP_NEW_PARAMETERS_ENTRY(DirCameraCalibImageOutput,     "DIRECTORY_CAMERA_CALIBRATION_IMAGE_OUTPUT",    std::string, "calibration/camera_images/");



    ConnectIdProjector          connect_id_projector;
    ConnectIdCamera             connect_id_camera;

    ConfigFileProjector         config_file_projector;
    ConfigFileCamera            config_file_camera;


    ConfigFileCalibCamera       config_file_calib_camera;
    CalibDataFileCamera         calib_data_file_camera;
    DirCameraCalibImageOutput   dir_camera_calib_image_output;
    OutputNameImageCameraCalib  output_name_image_camera_calib;

    OutputNameImageCameraCalibBoard output_name_image_camera_calib_board;
    ConfigFileStructuredLight1  config_file_structured_light_1;
    ConfigFileStructuredLight2  config_file_structured_light_2;
    ConfigFileCalibProjector    config_file_calib_projector;

    // Variables used throughout code
    dlp::ReturnCode             ret;
    dlp::StructuredLight*       module;
    dlp::Pattern::Sequence      sequence;
    dlp::Pattern::Color         color       = dlp::Pattern::Color::WHITE;   // Since all patterns are greyscale this does not matter for the saved images
    dlp::Pattern::Orientation   orientation = dlp::Pattern::Orientation::VERTICAL;
    unsigned int width  = 912;
    unsigned int height = 1140;
    unsigned int pattern_resolution = width;
    std::string basename = "pattern_";
    std::string image_name = "image";

    // Load the settings
    dlp::Parameters settings;
    settings.Load("DLP_LightCrafter_4500_3D_Scan_Application_Config.txt");

    settings.Get(&connect_id_projector);
    settings.Get(&config_file_projector);
    settings.Get(&config_file_calib_projector);
    settings.Get(&config_file_structured_light_1);
    settings.Get(&config_file_structured_light_2);

    settings.Get(&output_name_image_camera_calib_board);
    settings.Get(&connect_id_camera);
    settings.Get(&config_file_camera);


    settings.Get(&config_file_calib_camera);
    settings.Get(&calib_data_file_camera);
    settings.Get(&dir_camera_calib_image_output);
    settings.Get(&output_name_image_camera_calib);



    // Connect camera and projector
    ret = dlp::DLP_Platform::ConnectSetup(projector_4500,connect_id_projector.Get(),config_file_projector.Get(),true);
    if(ret.hasErrors()) {
        dlp::CmdLine::Print("\n\nPlease resolve the LightCrafter connection issue before proceeding to next step...\n");
    }

//    ret = dlp::Camera::ConnectSetup(camera_pg,connect_id_camera.Get(),config_file_camera.Get(),true);
//    if(ret.hasErrors()) {
//        dlp::CmdLine::Print("\n\nPlease resolve the camera connection issue before proceeding to next step...\n");
//    }

}


//投影白光
void Widget::projectSignalPattern(dlp::DLP_Platform *projector, int num)
{

    //open camera
    capture_Num= 0;
    captureMachine.setCamera(&camera_pg);
    captureMachine.start();



    /*******************************************************************************************************/
     // Check that projector is NOT null
     if(!projector){
        std::cout<<"projector is NULL\n";
         return;
     }
     // Check if projector is connected
     if(!projector->isConnected()){
         dlp::CmdLine::Print("Projector NOT connected! \n");
         return;
     }


     switch(num)
     {
     case 0:
     {
         projector->ProjectSolidBlackPattern();
     }
         break;
     case 1:
     {

         projector->ProjectSolidWhitePattern();
         std::cout<<"projector is ok\n";

     }
         break;
     }


}

void Widget::reconnectDevice()
{

        camera_pg.Disconnect();

        projector_4500.Disconnect();

        // Reconnect system objects

        connectDevice();



}

//同步加取图案
void Widget::highspeeDebugdScanPatterns(dlp::Camera *camera, dlp::DLP_Platform *projector, const bool &cam_proj_hw_synchronized,
                                int startNum, int captureNum, std::vector<Mat> &mat_list)
{
    // Check that camera is NOT null
    if(!camera) return;

    // Check if camera is connected
    if(!camera->isConnected()){
        dlp::CmdLine::Print("Camera NOT connected! \n");
        return;
    }

    // Check that projector is NOT null
    if(!projector) return;

    // Check if projector is connected
    if(!projector->isConnected()){
        dlp::CmdLine::Print("Projector NOT connected! \n");
        return;
    }



    // Variables used during scan loop
    int scan_count = 0;
    std::atomic_bool continue_scanning(true);
    dlp::Time::Chronograph  timer;
    dlp::Capture::Sequence  capture_scan;

    // Get the camera frame rate (This assumes the camera triggers the projector!)
    float frame_rate;
    camera->GetFrameRate(&frame_rate);

    // Determine the total scan time
    unsigned int period_us = 1000000 / frame_rate;

    unsigned int capture_time = 0;
    unsigned int pattern_start = startNum; // There is one calibration image so start with offse
    unsigned int pattern_count= captureNum;


    capture_time += period_us * pattern_count;

    // Open a camera view so that the target object can be placed
    // within the view of the camera and projector
    // Begin capturing images for camera calibration
    dlp::Image          camera_frame;
    dlp::Image::Window  camera_view;

    /***************************************************************************************************************************/


    // Open the camera view
//    dlp::CmdLine::Print("\nPlace the scanning target within the view of the camera and projector. \nPress SPACE or ESC from window when ready to scan...");
//    camera_view.Open("Place Target in View - press SPACE or ESC to scan");

//    // Start capturing images from the camera
//    if(camera->Start().hasErrors()){
//        dlp::CmdLine::Print("Could NOT start camera! \n");
//        return;
//    }

//    camera->ControlStrobe(1);

    // Project white so the object can be placed correctly
//    projector->ProjectSolidWhitePattern();

    // Wait for the space bar or ESC key to be pressed before scanning
//    unsigned int return_key = 0;
//    while(return_key != ' '){
//        camera_frame.Clear();               // Clear the image object
//        camera->GetFrame(&camera_frame);    // Grab the latest camera frame
//        camera_view.Update(camera_frame);   // Display the image
//        camera_view.WaitForKey(16,&return_key);     // Wait for a key to be pressed or a 50ms timeout
//        if(return_key == 27) break;                 // ESC key was pressed
//    }


//    projector->StopPatternSequence();

    // Close the image window
//    camera_view.Close();

//    if(camera->Stop().hasErrors()){
//        dlp::CmdLine::Print("Camera failed to stop! Exiting scan routine...");
//    }

    // Enter the scanning loop
    while(continue_scanning){

        dlp::CmdLine::Print("\nStarting scan ", scan_count, "...");

//        dlp::Capture::Sequence vertical_scan;
//        dlp::Capture::Sequence horizontal_scan;
        dlp::Capture::Sequence sequence_scan;

        //Peform images capture when both camera and projector are connected
        //via HW trigger signal for synchronization
        if(cam_proj_hw_synchronized == true) {


            timer.Reset();




            // Start capturing images from the camera
            if(camera->Start().hasErrors()){
                dlp::CmdLine::Print("Could NOT start camera! \n");
                return;
            }

            //open store sequence
//            camera->ControlStrobe(2);
            //set store sequence
//            camera->ControlStrobe(2);
//            //close strobe
//            camera->ControlStrobe(0);
            //close store sequence
//            camera->ControlStrobe(3);
//            timer.Reset();
//            camera->ControlStrobe(3);
//            dlp::CmdLine::Print("close Capture...\t", timer.Lap(), "ms");

//            dlp::Time::Sleep::Microseconds(100);

            // Scan the object
            dlp::ReturnCode sequence_return;
            sequence_return = projector->StartPatternSequence(pattern_start,pattern_count ,false);
            if(sequence_return.hasErrors()){
                dlp::CmdLine::Print("Sequence failed! Exiting scan routine...");
                if(camera->Stop().hasErrors()){
                    dlp::CmdLine::Print("Camera failed to stop! Exiting scan routine...");
                }
                dlp::CmdLine::Print("Sequence failed..." + sequence_return.ToString());
                return;
            }

            // Give camera time to start capturing images
//            dlp::Time::Sleep::Milliseconds(100);


            dlp::CmdLine::Print("DLP and Camera start time... \t \t", timer.Lap(), "ms");




            /**************************************************************************************************************/


           int loop_num= captureNum/3 +1;
//           if(0!= captureNum%3)
//           {
//               loop_num +=1;
//           }

//           dlp::CmdLine::Print("loop_num...\t", loop_num, "   num");

//            int reload_time= 78000;
            float period= 2.80;

            int reload_time= 78000;
//            float period= (float)delay_1/10000;


            timer.Reset();
            camera->ControlStrobe(2);
            dlp::CmdLine::Print("open Capture...\t\t", timer.Lap(), "ms");

            for(int i_capture= 0;i_capture< loop_num;i_capture++)
            {

                //open strobe
                camera->ControlStrobe(1);
                //capture sequence
                dlp::Time::Sleep::Microseconds(period*period_us);
                //close strobe
                camera->ControlStrobe(0);
                //reload sequence
                dlp::Time::Sleep::Microseconds(reload_time/3);
                //close store sequence
                camera->ControlStrobe(3);
                //reload sequence
                dlp::Time::Sleep::Microseconds(2*reload_time/3);
                //open store sequence
                camera->ControlStrobe(2);

            }
            //close strobe
            camera->ControlStrobe(1);
            //capture sequence
//            dlp::Time::Sleep::Microseconds(period*period_us);
//            dlp::Time::Sleep::Microseconds(reload_time/3);


//            dlp::Time::Sleep::Microseconds(delay_num*3*period_us);

//            for(int i_capture= 0;i_capture< captureNum/3;i_capture++)
//            {
//                camera->ControlStrobe(1);
//                dlp::Time::Sleep::Microseconds(period*period_us);
//                camera->ControlStrobe(0);
//                dlp::Time::Sleep::Microseconds(reload_time);
//            }

//                camera->ControlStrobe(1);
//                dlp::CmdLine::Print("open Capture...\t", timer.Lap(), "ms");

//                dlp::Time::Sleep::Microseconds(delay_1);

//                timer.Reset();
//                camera->ControlStrobe(0);
//                dlp::CmdLine::Print("close Capture...\t", timer.Lap(), "ms");

//                dlp::Time::Sleep::Microseconds(delay_2);

//                camera->ControlStrobe(1);

            /***********************************************************************************************************/


            // Wait for the sequence to finish and add a little extra time
            // to account for the sequence validation time required
//            dlp::Time::Sleep::Microseconds(capture_time + 3*period_us);

//                dlp::Time::Sleep::Microseconds(2*period_us);

            // Stop grabbing images from the camera
            if(camera->Stop().hasErrors()){
                dlp::CmdLine::Print("Camera failed to stop! Exiting scan routine...");
                return;
            }

            dlp::CmdLine::Print("Pattern sequence capture completed in...\t", timer.Lap(), "ms");
            projector->StopPatternSequence();

            // Grab all of the images from the buffer to find the pattern sequence
            bool            min_images = false;
            dlp::ReturnCode ret;
            dlp::Capture    capture;
            dlp::Image      capture_image;

            unsigned int iPattern = 0;

            std::vector<double> capture_sums;

            while(!min_images){

                capture_image.Clear();

                ret = camera->GetFrameBuffered(&capture_image);
                iPattern++;
                capture_image.ConvertToMonochrome();
                if(ret.hasErrors()){
                    min_images = true;
                }
                else{

                    double sum = 0;
                    capture_image.GetSum(&sum);
                    capture_sums.push_back(sum);

                    // Add the frame to the sequence
                    capture.image_data = capture_image;
                    capture.data_type  = dlp::Capture::DataType::IMAGE_DATA;
                    capture_scan.Add(capture);
                    capture_image.Clear();
                    capture.image_data.Clear();

                }

            }

            dlp::CmdLine::Print("Images retreived from buffer in...\t\t", timer.Lap(), "ms");

            // Find  the first pattern and the seperate the vertical and horizontal patterns
//            vertical_scan.Clear();
//            horizontal_scan.Clear();

            sequence_scan.Clear();

            bool first_pattern_found = false;
            unsigned int capture_offset = 0;

            double previous_sum = capture_sums.at(0);

            int patterns_added= 0;

            timer.Lap();
             mat_list.clear();

            for(unsigned int iScan = 0; iScan < capture_scan.GetCount(); iScan++)
            {

                //get all buffer images
                dlp::Capture temp_buffer;
                capture_scan.Get(iScan,&temp_buffer);
//                if(iScan< 10)
//                {

//                    temp_buffer.image_data.Save("output/scan_images/scan_buffer_capture_0"+dlp::Number::ToString(iScan)+".bmp");
//                }else{

//                    temp_buffer.image_data.Save("output/scan_images/scan_buffer_capture_"+dlp::Number::ToString(iScan)+".bmp");
//                }



                // Retrieve the image sum value
                double sum = capture_sums.at(iScan);
//                qDebug()<<iScan<<"  sum: "<<sum;
//                save capture sequence
                if(sum> previous_sum*1.05)
//                    if(sum> 30000000)
                {

                    dlp::Capture temp;
                    capture_scan.Get(iScan,&temp);

                    cv::Mat capture_mat;
                    temp.image_data.GetOpenCVData(&capture_mat);
                    mat_list.push_back(capture_mat);


//                    if(patterns_added< 10)
//                    {

//                        temp.image_data.Save("output/scan_images/capture_0"+dlp::Number::ToString(patterns_added++)+".bmp");
//                    }else{

//                        temp.image_data.Save("output/scan_images/capture_"+dlp::Number::ToString(patterns_added++)+".bmp");
//                    }
                    temp.image_data.Clear();
                }

            }
        }

        capture_scan.Clear();
        dlp::CmdLine::Print("Patterns sorted in...\t\t\t\t", timer.Lap(), "ms");
/*************************************************************************************************************************************/


    if(camera->Stop().hasErrors()){
        dlp::CmdLine::Print("Camera failed to stop! Exiting scan routine...");
    }

dlp::CmdLine::Print("camera->Stop()...\t\t", timer.Lap(), "ms");
    return;

    }


}


void Widget::generateBinaryPatterns(dlp::Pattern::Orientation orientation,int g_count,dlp::Pattern::Sequence& g_sequence)
{

    dlp::ReturnCode             ret;
    dlp::StructuredLight*       module;
    dlp::Pattern::Color         color       = dlp::Pattern::Color::WHITE;   // Since all patterns are greyscale this does not matter for the saved images
    unsigned int width  = 912;
    unsigned int height = 1140;

    module = new dlp::BinaryCode;
    settings.Clear();
    g_sequence.Clear();


    // Setup the module
    do{
        unsigned int pixels_per_period = 0;
        unsigned int bit_depth  = 0;
        unsigned int squence_count  = 0;
        dlp::Pattern::Bitdepth pattern_bitdepth;


        //                    image_name = "Speckle_Code_";

        //                std::cout << "Enter the number of pixels per period (must be multiple of 8): ";
        //                std::cin  >> pixels_per_period;
        //                image_name += dlp::Number::ToString(pixels_per_period) + "PXL_PERIOD_";

        //                std::cout << "Enter the bitdepth (maximum 8. Largest intensity will be (2^bitdepth) - 1): ";
        bit_depth= 1;
        if(bit_depth > 8) bit_depth = 8;
        if(bit_depth < 1) bit_depth = 8;
        //                    image_name += dlp::Number::ToString(bit_depth) + "BPP_VALUES_";

        // Convert integer to pattern bitdepth
        switch (bit_depth) {
        case 1:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_1BPP;
            break;
        case 2:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_2BPP;
            break;
        case 3:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_3BPP;
            break;
        case 4:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_4BPP;
            break;
        case 5:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_5BPP;
            break;
        case 6:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_6BPP;
            break;
        case 7:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_7BPP;
            break;
        case 8:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_8BPP;
            break;
        default:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_8BPP;
            break;
        }


//        std::cout << "Enter the sequence count: ";

//        std::cin  >> squence_count;

        squence_count= g_count;

        settings.Set(dlp::StructuredLight::Parameters::PatternColor(color));
        settings.Set(dlp::StructuredLight::Parameters::PatternOrientation(orientation));
        settings.Set(dlp::StructuredLight::Parameters::PatternColumns(width));
        settings.Set(dlp::StructuredLight::Parameters::PatternRows(height));

        //                            settings.Set(dlp::SpeckleCode::Parameters::Bitdepth(pattern_bitdepth));
        settings.Set(dlp::BinaryCode::Parameters::IncludeInverted(false));
        settings.Set(dlp::BinaryCode::Parameters::SequenceCount(squence_count));
        settings.Set(dlp::BinaryCode::Parameters::CodeType(1));

        // Setup the module
        ret = module->Setup(settings);

        // Print the result
        std::cout << "Setting up structured light module..." << ret.ToString() << std::endl;
    }while(ret.hasErrors());

    // Generate the patterns
    std::cout << "Generating structured light module patterns..." << std::endl;
    module->GeneratePatternSequence(&g_sequence);


    /***********************************************************************************************************/
    // Clear variables and deallocate memory
    settings.Clear();
    //                sequence.Clear();
    delete module;

}

void Widget::prepareScanPatterns(bool updateFlag)
{
    dlp::Pattern::Sequence sequence;
    dlp::Parameters force_preparation;
    dlp::ReturnCode ret;

//    generateCalibratePatterns(patterns_count,sequence);
    generateScanPatterns(6,sequence);

    force_preparation.Set(dlp::DLP_Platform::Parameters::SequencePrepared(!updateFlag));
    projector_4500.Setup(force_preparation);

    // If the firmware needs to be uploaded, display debug messages so progress can be observed
//    projector->SetDebugEnable(!previously_prepared);
    projector_4500.SetDebugEnable(updateFlag);


    // Prepare projector
    dlp::CmdLine::Print("Preparing projector for calibration and structured light...");
    ret = projector_4500.PreparePatternSequence(sequence);
    projector_4500.SetDebugEnable(false);

    if( ret.hasErrors()){
        dlp::CmdLine::Print("Projector prepare sequence FAILED: ");
        dlp::CmdLine::Print(ret.ToString());

    }


//    if(updateFlag)
//    {
//        SavePatternImageData(sequence,4,"scan_firmware_");
//        reconnectDevice();
//    }

    SavePatternImageData(sequence,4,"D:/zhangguanjin/build-StructuredLight3D-Desktop_Qt_5_7_1_MSVC2015_32bit-Release/reloadimage/scan_firmware_");


    patterns_prepare= true;

    dlp::CmdLine::Print("Projector prepared");
}


void Widget::generateScanPatterns(int squence_count, dlp::Pattern::Sequence& scan_sequence)
{
    dlp::Pattern::Sequence      sequence_v;
    dlp::Pattern::Orientation   orientation_v = dlp::Pattern::Orientation::VERTICAL;

    generateFourMultiplyWavelengthPatterns(orientation_v,squence_count,sequence_v);

    scan_sequence.Add(sequence_v);

}

//生成图片
void Widget::generateFourMultiplyWavelengthPatterns(dlp::Pattern::Orientation orientation,int g_count,dlp::Pattern::Sequence& g_sequence)
{

    dlp::ReturnCode             ret;
    dlp::StructuredLight*       module;
    dlp::Pattern::Color         color       = dlp::Pattern::Color::WHITE;   // Since all patterns are greyscale this does not matter for the saved images
    unsigned int width  = 912;
    unsigned int height = 1140;
//    unsigned int pattern_resolution = width;
//    std::string basename = "pattern_";
    std::string image_name = "image";

    module = new dlp::mutil_wavelength;
    dlp::Parameters settings;
    settings.Clear();
    g_sequence.Clear();


    // Setup the module
    do{
        unsigned int pixels_per_period = 0;
        unsigned int bit_depth  = 8;
        unsigned int squence_count  = g_count;
        dlp::Pattern::Bitdepth pattern_bitdepth;


        image_name = "MULTI_WAVELENGTH_";

        image_name += dlp::Number::ToString(bit_depth) + "BPP_VALUES_";

        // Convert integer to pattern bitdepth
        switch (bit_depth) {
        case 1:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_1BPP;
            break;
        case 2:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_2BPP;
            break;
        case 3:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_3BPP;
            break;
        case 4:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_4BPP;
            break;
        case 5:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_5BPP;
            break;
        case 6:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_6BPP;
            break;
        case 7:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_7BPP;
            break;
        case 8:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_8BPP;
            break;
        default:
            pattern_bitdepth = dlp::Pattern::Bitdepth::MONO_8BPP;
            break;
        }


        settings.Set(dlp::StructuredLight::Parameters::PatternColor(color));
        settings.Set(dlp::StructuredLight::Parameters::PatternOrientation(orientation));
        settings.Set(dlp::StructuredLight::Parameters::PatternColumns(width));
        settings.Set(dlp::StructuredLight::Parameters::PatternRows(height));

        settings.Set(dlp::mutil_wavelength::Parameters::Bitdepth(pattern_bitdepth));
        settings.Set(dlp::mutil_wavelength::Parameters::PixelsPerPeriod(pixels_per_period));
        settings.Set(dlp::mutil_wavelength::Parameters::SequenceCount(squence_count));

        // Setup the module
        ret = module->Setup(settings);

        // Print the result
        std::cout << "Setting up structured light module..." << ret.ToString() << std::endl;
    }while(ret.hasErrors());

    // Generate the patterns
    std::cout << "Generating structured light module patterns..." << std::endl;
    module->GeneratePatternSequence(&g_sequence);





    /***********************************************************************************************************/
    // Clear variables and deallocate memory
    settings.Clear();
//    sequence.Clear();
    delete module;
}


void Widget::projectSignalBinaryPattern(dlp::DLP_Platform *projector,int num)
{

    qDebug()<<"start";

    capture_Num=0;
    captureMachine.setCamera(&camera_pg);
    captureMachine.start();

    // Check that projector is NOT null
    if(!projector) return;

    // Check if projector is connected
    if(!projector->isConnected()){
        dlp::CmdLine::Print("Projector NOT connected! \n");
        return;
    }


//    projector->StopPatternSequence();
    projector->StartPatternSequence(num,1,true);


    qDebug()<<"finished";



}


void Widget::SavePatternImageData(const dlp::Pattern::Sequence &sequence, int num, const std::string basename)
{
    unsigned int count = 0;

    count = sequence.GetCount();

    for(unsigned int iPattern = 0; iPattern < count; iPattern++){
        dlp::Pattern temp_pattern;

        // Get the pattern
        sequence.Get(iPattern, &temp_pattern);

        // Save the image data
        std::cout << "Saving image " << basename + "_" +dlp::Number::ToString(iPattern) + ".bmp" << std::endl;

        if(iPattern< 10)
        {

            temp_pattern.image_data.Save(basename + "_0" +dlp::Number::ToString(iPattern) + ".bmp");
        }
        else
        {

            temp_pattern.image_data.Save(basename + "_" +dlp::Number::ToString(iPattern) + ".bmp");
        }

    }

    return;
}


void Widget::unwarpHierarchical(float rate, cv::Mat const l_wrap, cv::Mat h_wrap, cv::Mat &h_unwrap, cv::Mat &k_Mat)
{


        int nr= l_wrap.rows;
        int nc= l_wrap.cols;

        cv::Mat result(nr,nc,CV_64F);
        cv::Mat k_value(nr,nc,CV_8U);

        if(l_wrap.isContinuous())
        {
            if(h_wrap.isContinuous())
            {
                if(k_Mat.isContinuous())
                {
                        nc= nc*nr;
                        nr= 1;
                }
            }

        }


        for(int i= 0;i< nr;i++)
        {
            double const * l_ptr= l_wrap.ptr<double>(i);
            double* h_ptr= h_wrap.ptr<double>(i);
            uchar* k_ptr= k_value.ptr<uchar>(i);

            double* h_unwrap_ptr= result.ptr<double>(i);
            for(int j= 0;j< nc;j++)
            {

                double temp= 0.5+ (rate*l_ptr[j]- h_ptr[j])/(PI);

                int k= temp;

                k_ptr[j]= k;

                h_unwrap_ptr[j]= PI*k+ h_ptr[j];



            }
        }

       h_unwrap= result.clone();
       k_Mat= k_value.clone();


}

void Widget::phaseNormalization(float rate, cv::Mat unwrap_phase, cv::Mat &norm_phase)
{
    int nr= unwrap_phase.rows;
    int nc= unwrap_phase.cols;


    cv::Mat result(nr,nc,CV_64F);

    if(unwrap_phase.isContinuous())
    {
        if(result.isContinuous())
        {

            nc= nc*nr;
            nr= 1;

        }

    }


    for(int i= 0;i< nr;i++)
    {
        double* un_ptr= unwrap_phase.ptr<double>(i);
        double* norm_ptr= result.ptr<double>(i);

        for(int j= 0;j< nc;j++)
        {

            norm_ptr[j]= un_ptr[j]/(rate);



        }
    }

    norm_phase= result.clone();
}

void Widget::unwrapThreeFrequencyHeterodyne(std::vector<cv::Mat> wrap_list,cv::Mat& unwrap_phase)
{

    cv::Mat image_1= wrap_list.at(0);
    cv::Mat image_2= wrap_list.at(1);
    cv::Mat image_3= wrap_list.at(2);


    int nr= image_1.rows;
    int nc= image_1.cols;


    cv::Mat unwrap_comp_final_k1(nr,nc,CV_64F);

    //补偿界限
    float compensateBoundary= PI/2;

    float p1= 52;
    float p2= 58;
    float p3= 620;

    float p12= p2*p1/(p2- p1);
    float c3=  p12/(p3-p12);
    float c12= p3/(p3-p12);
    float c1= p2/(p2-p1);


    /***********************************************************************************************************************************/
    //包裹相位1、2叠加

     if(image_1.isContinuous())
     {
         if(image_2.isContinuous())
         {
             nc= nc*nr;
             nr= 1;

         }

     }


     for(int i= 0;i< nr;i++)
     {
         //包裹相位1、2叠加
         double* ptr_w1= image_1.ptr<double>(i);
         double* ptr_w2= image_2.ptr<double>(i);
         //包裹相位12、3叠加
         double* ptr_w3= image_3.ptr<double>(i);
         //解包裹相位wrap_3，1、直接按比例换算123-3，2、计算出k值再换算，3、两者综合作误差补偿
        //误差补偿：用比例、k值的方法对unwrap_12进行误差补偿，用补偿好的unwrap_k12,unwrap_1对unwrap_k1进行最终的误差补偿
         double* ptr_un_comp_final_k1= unwrap_comp_final_k1.ptr<double>(i);

         for(int j= 0;j< nc;j++)
         {
             //包裹相位1、2叠加
             double value_w12= ptr_w1[j]- ptr_w2[j];
             if(value_w12< 0){
                 value_w12 += PI;
             }


             /***************************************************************************************/
             //解包裹相位wrap_3，1、直接按比例换算123-3，2、计算出k值再换算，3、两者综合作误差补偿
             float temp_unw3= (value_w12 - ptr_w3[j])/PI;
             if(temp_unw3< 0){
                 temp_unw3+= 1;
             }

             //比例展开wrap3
             double value_unw3= temp_unw3*c3*PI;
             int k3= c3*temp_unw3;
             //k值展开wrap3
             double value_un_k3= k3*PI+ ptr_w3[j];

             //结合比例展开、k值展开部分进行相位补偿comp_unwrap_3
             double value_un_err3= value_un_k3- value_unw3;

             int comp_k3= k3;
             double chase_un_comp_k3= value_un_k3;

             if(compensateBoundary< value_un_err3)
             {
                 comp_k3--;
                 chase_un_comp_k3= value_un_k3- PI;
             }

             if(-compensateBoundary> value_un_err3)
             {
                 comp_k3++;
                 chase_un_comp_k3= value_un_k3+ PI;
             }

             double value_un_comp_k3= chase_un_comp_k3;

             double value_unw1= value_un_comp_k3*p3/p1;


             /************************************************************************************************/
             //误差补偿：用比例、k值的方法对unwrap_12进行误差补偿，用补偿好的unwrap_k12,unwrap_1对unwrap_k1进行最终的误差补偿

             // 比例展开wrap12
             float temp_unw12= (value_w12- ptr_w3[j])/PI;
             if(temp_unw12< 0){
                 temp_unw12+= 1;
             }

             double value_unw12= c12*temp_unw12*PI;


             //k值展开wrap12
             int k12= c12*temp_unw12;
             double value_un_k12= k12*PI+value_w12;


             //k值展开wrap1
             float temp12=  (ptr_w1[j] - ptr_w2[j])/PI;
             if(temp12< 0){
                 temp12+= 1;
             }

             /***********************************************************************************************************************/
             /***********************************************************************************************************************/
             //补偿N12
             //wrap12误差补偿
             double value_un_err12= value_un_k12- value_unw12;

             int comp_k12= k12;

             if(compensateBoundary< value_un_err12)
             {
                 comp_k12--;
             }
             if(-compensateBoundary> value_un_err12)
             {
                 comp_k12++;
             }

             //补偿unwrap_k12后对unwrap_k1展开
             int comp_k1= c1*(comp_k12+ temp12);

             int value_un_comp_k1= comp_k1*PI+ ptr_w1[j];
             /***********************************************************************************************************************/
             //经过k12补偿的unwrap_k1与比例计算的unwrap_1结合进行误差补偿
             double value_un_err= value_un_comp_k1- value_unw1;

             double chase_final= value_un_comp_k1;

             if(compensateBoundary< value_un_err)
             {
                 chase_final= value_un_comp_k1- PI;
             }

             if(-compensateBoundary> value_un_err)
             {
                 chase_final= value_un_comp_k1+ PI;
             }

             ptr_un_comp_final_k1[j] = chase_final;

             /************************************************************************************************/
         }
     }


     unwrap_phase= unwrap_comp_final_k1.clone();





}

void Widget::decodeMultiplyWavelengthUnwrap(std::vector<cv::Mat> in_mat,cv::Mat &unwrap_x,cv::Mat &unwrap_y)
{
    std::vector<cv::Mat> x_mat;
    std::vector<cv::Mat> y_mat;
    PhaseShift PS_machine;



    for(int i= 0;i< in_mat.size()/2;i+= 4)
    {

        std::vector<cv::Mat> images;
        images.push_back(in_mat.at(i+ 0));
        images.push_back(in_mat.at(i+ 1));
        images.push_back(in_mat.at(i+ 2));
        images.push_back(in_mat.at(i+ 3));

        cv::Mat result;
        PS_machine.fourStepPhaseShift(images,result);

        x_mat.push_back(result);


    }


    for(int i= in_mat.size()/2; i< in_mat.size();i+= 4)
    {

        std::vector<cv::Mat> images;
        images.push_back(in_mat.at(i+ 0));
        images.push_back(in_mat.at(i+ 1));
        images.push_back(in_mat.at(i+ 2));
        images.push_back(in_mat.at(i+ 3));

        cv::Mat result;
        PS_machine.fourStepPhaseShift(images,result);

        y_mat.push_back(result);


    }



    cv::Mat unwrap_f_x,unwrap_f_y;

    PS_machine.multipleWavelengthUnwrapFloat(x_mat,unwrap_f_x);
    PS_machine.multipleWavelengthUnwrapFloat(y_mat,unwrap_f_y);


    PS_machine.phaseNormalization(pow(2.0,x_mat.size()-1), unwrap_f_x,unwrap_x);
    PS_machine.phaseNormalization(pow(2.0,x_mat.size()-1), unwrap_f_y,unwrap_y);






}

void Widget::on_camera_connect_slot()
{
    camera_pg.Disconnect();
    projector_4500.Disconnect();

    if(captureMachine.isRunning())
    {
        textbrowser->append("\ncaptureMachine is running and now terminate ...,please wait ...");
        captureMachine.setCaptureFlag(false);
        project_sleep(1000);
        camera_connect_button->setText("reconnect");
        return;
    }
    camera_connect_button->setText("Camera");

    DLP_NEW_PARAMETERS_ENTRY(ConnectIdProjector,           "CONNECT_ID_PROJECTOR",  std::string, "0");
    DLP_NEW_PARAMETERS_ENTRY(ConnectIdCamera,              "CONNECT_ID_CAMERA",     std::string, "0");

    DLP_NEW_PARAMETERS_ENTRY(ConfigFileProjector,           "CONFIG_FILE_PROJECTOR",                std::string, "config/config_projector.txt");//D:/zhouhaoyuan/build-StructuredLight3D-Desktop_Qt_5_7_1_MSVC2015_32bit-Release/
    DLP_NEW_PARAMETERS_ENTRY(ConfigFileCamera,              "CONFIG_FILE_CAMERA",                   std::string, "config/config_color_camera.txt");

    DLP_NEW_PARAMETERS_ENTRY(ConfigFileStructuredLight1,    "CONFIG_FILE_STRUCTURED_LIGHT_1",       std::string, "config/algorithm_vertical.txt");
    DLP_NEW_PARAMETERS_ENTRY(ConfigFileStructuredLight2,    "CONFIG_FILE_STRUCTURED_LIGHT_2",       std::string, "config/algorithm_horizontal.txt");
    DLP_NEW_PARAMETERS_ENTRY(ConfigFileCalibProjector,      "CONFIG_FILE_CALIBRATION_PROJECTOR",    std::string, "config/calibration_projector.txt");

    DLP_NEW_PARAMETERS_ENTRY(OutputNameImageCameraCalibBoard, "OUTPUT_NAME_IMAGE_CAMERA_CALIBRATION_BOARD", std::string, "camera_calibration_board");
    DLP_NEW_PARAMETERS_ENTRY(OutputNameImageCameraCalib,    "OUTPUT_NAME_IMAGE_CAMERA_CALIBRATION", std::string, "camera_calibration_capture_");

    DLP_NEW_PARAMETERS_ENTRY(ConfigFileCalibCamera,         "CONFIG_FILE_CALIBRATION_CAMERA",       std::string, "config/calibration_camera.txt");
    DLP_NEW_PARAMETERS_ENTRY(CalibDataFileCamera,           "CALIBRATION_DATA_FILE_CAMERA",         std::string, "calibration/data/camera.xml");
    DLP_NEW_PARAMETERS_ENTRY(DirCameraCalibImageOutput,     "DIRECTORY_CAMERA_CALIBRATION_IMAGE_OUTPUT",    std::string, "calibration/camera_images/");



    ConnectIdProjector          connect_id_projector;
    ConnectIdCamera             connect_id_camera;

    ConfigFileProjector         config_file_projector;
    ConfigFileCamera            config_file_camera;

    ConfigFileStructuredLight1  config_file_structured_light_1;
    ConfigFileStructuredLight2  config_file_structured_light_2;
    ConfigFileCalibProjector    config_file_calib_projector;

    OutputNameImageCameraCalib  output_name_image_camera_calib;
    OutputNameImageCameraCalibBoard output_name_image_camera_calib_board;

    ConfigFileCalibCamera       config_file_calib_camera;
    CalibDataFileCamera         calib_data_file_camera;
    DirCameraCalibImageOutput   dir_camera_calib_image_output;

    dlp::ReturnCode             ret;
    // Load the settings
    dlp::Parameters settings;
    //settings.Load("DLP_LightCrafter_4500_3D_Scan_Application_Config.txt");

    settings.Get(&connect_id_projector);
    settings.Get(&config_file_projector);
    settings.Get(&config_file_calib_projector);
    settings.Get(&config_file_structured_light_1);
    settings.Get(&config_file_structured_light_2);

    settings.Get(&output_name_image_camera_calib_board);
    settings.Get(&connect_id_camera);
    settings.Get(&config_file_camera);


    settings.Get(&config_file_calib_camera);
    settings.Get(&calib_data_file_camera);
    settings.Get(&dir_camera_calib_image_output);
    settings.Get(&output_name_image_camera_calib);


     //Connect camera
    ret = dlp::Camera::ConnectSetup(camera_pg,connect_id_camera.Get(),config_file_camera.Get(),true);
    if(ret.hasErrors()) {
        textbrowser->append("\nPlease resolve the camera connection issue before proceeding to next step...");
    }

    captureMachine.setCamera(&camera_pg);
    captureMachine.start();

    QDesktopWidget *deskWgt=QApplication::desktop();
    int screen_count=deskWgt->screenCount();
    textbrowser->append("\nprojector number : "+QString::number(screen_count));

    QRect rect=deskWgt->screenGeometry(0);

    if(screen_count==1)
    {
        imageWidth=rect.width();
        imageHeight=rect.height();
    }
    else
    {
        QRect rect1=deskWgt->screenGeometry(1);
        imageWidth=rect1.width();
        imageHeight=rect1.height();
        project_flag=true;
    }

    //设置投影窗口 
    pattern_labbel->move(rect.width(),0);
    pattern_labbel->showFullScreen();
    QImage whiteMap;
    cv::Mat white=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(180));
    whiteMap =QImage((const unsigned char*)(white.data),
                      white.cols,white.rows,
                      white.cols*white.channels(),
                      QImage::Format_Grayscale8);
    pattern_labbel->setPixmap(QPixmap::fromImage(whiteMap));
    pattern_labbel->show();


}
void Widget::on_reconstruction_slot()
{
    QTime time1;
    time1.start();
    //maskImage = imread("../ScanData/mask.bmp",0);
#if 0
    ///彩色三频外差
    color_three_frequency_reconstruction();
#endif

#if 0
    /////蓝光三频外差
    multiFrequencyHeterodyne_reconstruction();
#endif

#if 0
    ////两帧彩图，正弦+相位，三步相移
    color_sin_phaseCode_reconstruction();
#endif

#if 0
    ////蓝光分段或者连续，四步相移
    pure_sin_phaseCode_reconstruction();
#endif

#if 1
    /////////phaseCode_grayCode_sin_reconstruction//分段+格雷码，四步相移
    phaseCode_grayCode_sin_reconstruction();
#endif

#if 0
    ///双三步相移，红蓝通道
    color_double_frequency_reconstruction();
#endif


#if 0
    ////彩色连续阶梯，可分段，四步相移
    phaseCode_sin_reconstruction();
#endif

#if 0
    ///*****///彩色复合双频双相移
    composite_color_code_reconstruction();
#endif

    ////////binary_grayCode_sin_reconstruction//正弦格雷码
    //binary_grayCode_sin_reconstruction();

    /////////composed_sin_reconstruction//多频率合成
    //composed_sin_reconstruction();

    /////////Fourier_sin_reconstruction//傅里叶
    //Fourier_sin_reconstruction();

    //测试gamma
//    pattern_class->parameter_setting(imageHeight,imageWidth);
//    std::vector<cv::Mat> pattern_sequence;
//    if(phaseErrorTable_flag && gamma_box->isChecked())
//    {
//        pattern_class->generate_gamma_pure_sinusoidal_pattern(pattern_sequence,200,4,Blue,Vertical,gamma_Aconst,gamma_Bcoefficient);
//        std::cout<<"the gamma correct is successfully ...  \n";
//    }else{
//        pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,200,4,Blue,Vertical);
//    }
//    /******************************capture********************************/
//    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
//    if(!project_capture_pattern(pattern_sequence) || mat_list.empty())
//    {
//        std::cout<<"Error : Failed to get the picture ... "<<std::endl;
//        return;
//    }
//    std::vector<cv::Mat> image_list;
//    image_list.reserve(mat_list.size()+1);
//    for(unsigned int n=0;n<mat_list.size();++n)
//    {
//        std::vector<cv::Mat> channels;
//        if(crosstalk_flag)
//        {
//            pattern_class->color_correction(maskImage,mat_list[n],mat_list[n],crosstalk_mat,reflect_mat);
//        }
//        cv::split(mat_list[n],channels);
//        image_list.push_back(channels[0]);
//    }
//    //std::cout<<"image_list size : "<<image_list.size()<<std::endl;
//    cv::Mat sinResult;
//    pattern_class->decode_sinusoidal(maskImage,image_list,sinResult,4);
//    paint_pattern.push_back(sinResult);


    /*******************重建并保存*****************************/
//    right_p.clear();
//    left_p.clear();
//    pCloud.clear();

//    pattern_class->createCorrectPoints(unwrapPhaseX,unwrapPhaseY,40,left_p,right_p,maskImage);

//    pattern_class->reconstruct3D(left_p,right_p,pCloud);

//    std::cout<<"3D scanning consumed : "+QString::number(time1.elapsed()).toStdString()+" ms\n";
//    qDebug()<<"point cloud size : "<<pCloud.size()<<"\n";

//    QString pointCloud_path= QFileDialog::getSaveFileName(this,"Save PointCloud Path","../ScanData/pCloud.ply","*.ply");

//    pattern_class->savePointCloud(pCloud, pointCloud_path);

//    qDebug()<<"pointCloud_path : "<<pointCloud_path<<"\n";

#if 0
    QString open_file= pointCloud_path;

    QProcess *process = new QProcess;
    QStringList str;
    str << open_file;
    process->start("D:/meshlab/meshlab.exe",str);
#endif


}
void Widget::on_mask_button_slot()
{
    captureMachine.setCaptureFlag(false);
    if(!project_flag)
    {
        textbrowser->append("\nError: the project label is not ready!");
        return;
    }

    if(camera_pg.Start().hasErrors()){
        textbrowser->append("\nError: Could NOT start camera!");
        return;
    }

    camera_pg.ControlStrobe(2);

    dlp::Time::Sleep::Microseconds(500000);

    dlp::Image      captureImage;

    camera_pg.ControlStrobe(1);
    dlp::Time::Sleep::Microseconds(100000);//90000
    camera_pg.ControlStrobe(0);

    camera_pg.GetFrame(&captureImage);


//    if(maskImage.empty()){

        captureImage.GetOpenCVData(&maskImage);
        cv::cvtColor(maskImage,maskImage,CV_BGR2GRAY);
        cv::threshold(maskImage, maskImage, 0, 255, CV_THRESH_OTSU);
        cv::imwrite((saveDir+"/mask.bmp").toStdString(),maskImage);
        textbrowser->append("\nmaskImage is Captured ... ");

//    }


//    captureMachine.setCamera(&camera_pg);
//    captureMachine.start();
}
void Widget::on_crosstalk_gamma_slot()
{

    pattern_class->parameter_setting(imageHeight,imageWidth);

    std::vector<cv::Mat> pattern_sequence;

    cv::Mat mat_Red,mat_Green,mat_Blue,
            mat_Magenta,mat_Cyan,mat_Yellow, mat_White;

    pattern_class->generate_pure_color_pattern(mat_Red,Red);
    pattern_class->generate_pure_color_pattern(mat_Green,Green);
    pattern_class->generate_pure_color_pattern(mat_Blue,Blue);
    pattern_class->generate_pure_color_pattern(mat_Magenta,Magenta);
    pattern_class->generate_pure_color_pattern(mat_Cyan,Cyan);
    pattern_class->generate_pure_color_pattern(mat_Yellow,Yellow);
    pattern_class->generate_pure_color_pattern(mat_White,White);

    pattern_sequence.push_back(mat_Red);
    pattern_sequence.push_back(mat_Green);
    pattern_sequence.push_back(mat_Blue);
    pattern_sequence.push_back(mat_Magenta);
    pattern_sequence.push_back(mat_Cyan);
    pattern_sequence.push_back(mat_Yellow);
    pattern_sequence.push_back(mat_White);

//    if(maskImage.empty())
//    {
//        std::cout<<"Error: the maskImage is empty ... "<<std::endl;
//        return;
//    }
//    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
//    if(!project_capture_pattern(pattern_sequence))
//    {
//        textbrowser->append("\nError : Failed to capture the picture ... ");
//        return;
//    }


    /*************************************仿真******************************/
    mat_list.swap(pattern_sequence);
    std::cout<<"mat_list : "<<mat_list.size()<<std::endl;
    maskImage = cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));

    /*******************************************************************/

    crosstalk_mat=cv::Mat::zeros(3,3,CV_64FC1);

    cv::Mat capture_color=cv::Mat::zeros(3,6,CV_64FC1);
    cv::Mat project_color=cv::Mat::zeros(3,6,CV_64FC1);
    cv::Mat temp1=(cv::Mat_<double>(3,6)<< 0 , 0 ,250,250,250, 0 ,
                                           0 ,250, 0 , 0 ,250,250,
                                          250, 0 , 0 ,250, 0 ,250);

    temp1.copyTo(project_color);

    std::cout<<"project_color :\n"<<project_color<< std::endl;

    reflect_mat=cv::Mat::zeros(3,3,CV_64FC1);
    cv::Mat white_capture_mat;

    for(int n=0;n<mat_list.size();n++)
    {
        double Rmean=0.0,Gmean=0.0,Bmean=0.0;
        pattern_class->meanValue(maskImage,mat_list[n],Rmean,Gmean,Bmean);
        cv::Mat temp=(cv::Mat_<double>(3,1)<<Bmean,Gmean,Rmean);
        if(n != mat_list.size()-1)
        {
            temp.copyTo(capture_color.colRange(n,n+1));
        }
        else{
            white_capture_mat=temp.clone();
        }
    }

    std::cout<<"capture_color :\n"<<capture_color<< std::endl;

    reflect_mat.at<double>(0,0)=white_capture_mat.at<double>(0,0)/250;
    reflect_mat.at<double>(1,1)=white_capture_mat.at<double>(1,0)/250;
    reflect_mat.at<double>(2,2)=white_capture_mat.at<double>(2,0)/250;

    std::cout<<"reflect mat :\n"<<reflect_mat<< std::endl;

    crosstalk_mat=capture_color*project_color.inv(DECOMP_SVD)*reflect_mat.inv();//

    std::cout<<"crosstalk_mat :\n"<<crosstalk_mat<<std::endl;

    pattern_class->build_crosstalk_look_up_table(crosstalk_table,crosstalk_mat,reflect_mat);

    //校正
    cv::Mat correct_capture_color=cv::Mat::zeros(3,6,CV_64FC1);

    for(int n=0;n<6;n++)
    {
        cv::Mat tempResult=cv::Mat::zeros(mat_list[n].rows,mat_list[n].cols,CV_8UC3);

        pattern_class->correct_crosstalk_by_LookUpTable( crosstalk_table, mat_list[n],tempResult,maskImage);

        //绘图
//        paint_pattern.push_back(mat_list[n]);
//        paint_pattern.push_back(tempResult);

        QString original_path=cross_path + "/original_"+QString::number(n)+".bmp";
        QString correct_path =cross_path + "/corrcet_"+QString::number(n)+".bmp";
        cv::imwrite(original_path.toStdString(),mat_list[n]);
        cv::imwrite(correct_path.toStdString(),tempResult);

        double Rmean1=0.0,Gmean1=0.0,Bmean1=0.0;
        pattern_class->meanValue(maskImage,tempResult,Rmean1,Gmean1,Bmean1);

        cv::Mat temp=(cv::Mat_<float>(3,1)<<Bmean1,Gmean1,Rmean1);
        temp.copyTo(correct_capture_color.colRange(n,n+1));
    }
    std::cout<<"correct_capture_color :"<<std::endl;
    std::cout<<correct_capture_color<<std::endl;

    crosstalk_flag=true;

    std::cout<<"the crosstalk matrix is calculated ... "<<std::endl;

}
void Widget::project_sleep(unsigned int msec)
{
    QTime reachTime=QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime()<reachTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
}
void Widget::on_paintImage_slot()
{
    if(paint_pattern.empty() && value_paint.empty() && pointXY_paint.empty())
    {
        std::cout<<"Failed to transmit data!...... "<<std::endl;
        QMessageBox message(QMessageBox::Warning,"warning","can't transmit the data!",QMessageBox::Yes);
        if(message.exec()==QMessageBox::Yes)
        {
            return;
        }
    }
    if(maskImage.empty())
    {
        std::cout<<"maskImage is empty! ... "<<std::endl;
        maskImage=cv::Mat(paint_pattern[0].rows,paint_pattern[0].cols,CV_8UC1,cv::Scalar(255));
    }
    paint_pattern.push_back(maskImage);

    std::cout<<"the number of paint_pattern is "<<paint_pattern.size()<<std::endl;
    std::cout<<"the number of value_paint is "<<value_paint.size()<<std::endl;
    std::cout<<"the number of pointXY_paint is "<<pointXY_paint.size()<<std::endl;

    paintdialog=new paintDialog(this);

    paintdialog->getData(paint_pattern);

    paintdialog->getData(value_paint);

    paintdialog->getData(pointXY_paint);

    paintdialog->show();

}
bool Widget::project_capture_pattern(std::vector<cv::Mat> &pattern_list)
{
        captureMachine.setCaptureFlag(false);
        dlp::Time::Sleep::Microseconds(100000);
        if(!project_flag)
        {
            textbrowser->append("\nError: the project label is not ready!");
            return false;
        }
    //    pattern_labbel->show();
    //    float frame_rate;
    //    camera_pg.GetFrameRate(&frame_rate);
    //    textbrowser->append("\nthe frameRate of camera is "+QString::number(frame_rate));
    //    unsigned int period_us = 1000000 / frame_rate;
    //    float exposure_time;
    //    camera_pg.GetExposure(&exposure_time);
    //    textbrowser->append("\nthe exposure time of camera is "+QString::number(exposure_time));


        if(camera_pg.Start().hasErrors()){
            textbrowser->append("\nError: Could NOT start camera!\n");
            return false;
        }

        textbrowser->append("\nopen Capture......\n");

        QTime time1;

        camera_pg.ControlStrobe(2);

        dlp::Time::Sleep::Microseconds(50000);
        time1.start();

        dlp::Image      captureImage;
        mat_list.clear();

        for(int n=0;n<pattern_list.size();n++)
        {
            cv::Mat srcimage=pattern_list.at(n).clone();

            QImage project_image;

            if(srcimage.channels()==3)
                {
                    //cvt Mat BGR 2 QImage RGB
                cv::cvtColor(srcimage,srcimage,CV_BGR2RGB);
                project_image =QImage((const unsigned char*)(srcimage.data),
                                  srcimage.cols,srcimage.rows,
                                  srcimage.cols*srcimage.channels(),
                                  QImage::Format_RGB888);
                }
                else
                {
                    project_image =QImage((const unsigned char*)(srcimage.data),
                                srcimage.cols,srcimage.rows,
                                srcimage.cols*srcimage.channels(),
                                QImage::Format_Grayscale8);

                }



            pattern_labbel->setPixmap(QPixmap::fromImage(project_image));

            textbrowser->append(QString::number(n));
            project_sleep(85);
            if(n==0)
            {
                dlp::Time::Sleep::Microseconds(500000);
            }
            camera_pg.ControlStrobe(1);

            dlp::Time::Sleep::Microseconds(90000);//90000
            camera_pg.ControlStrobe(0);

            camera_pg.GetFrame(&captureImage);

            cv::Mat captureMat;
            captureImage.GetOpenCVData(&captureMat);

            cv::cvtColor(captureMat,captureMat,CV_RGB2BGR);
            cv::medianBlur(captureMat,captureMat,3);

            mat_list.push_back(captureMat);
        }
        textbrowser->append("\nPattern sequence capture completed in ... "+QString::number(time1.elapsed())+" ms");

        QPixmap whiteMap;
        whiteMap.fill(Qt::white);
        pattern_labbel->setPixmap(whiteMap);
        //pattern_labbel->hide();
        //open strobe
        //camera_pg.ControlStrobe(1);

        if(camera_pg.Stop().hasErrors()){
            textbrowser->append("\nCamera failed to stop! Exiting scan routine...");
            return false;
        }
        return true;
}

void Widget::on_calibrate_button_slot()
{
    if(maskImage.empty())
    {
        std::cout<<"Error: the maskImage is empty ... "<<std::endl;
        return;
    }
    //maskImage = cv::imread("../ScanData/mask.bmp",0);

    cali_decode_result.clear();

    QString dir_name=QFileDialog::getExistingDirectory(
                                     this,
                                     "Select Calibration Folder",
                                     "../ScanData");
    if(dir_name.isEmpty())
    {
        std::cout<<"Error: Can not find the calibration file ... "<<std::endl;
        return;
    }

    QDir dir(dir_name);
    QStringList dir_list;

    //dir.entryInfoList()此函数的返回值是一个QFileInfoList对象，包含当前目录下的所有文件和子目录。
    foreach (QFileInfo fileInfo, dir.entryInfoList(QDir::Dirs | QDir::Files))
    {
        QString strName = fileInfo.fileName();
//        qDebug()<<strName;
        if ((strName == QString(".")) || (strName == QString("..")))
            continue;
        if (fileInfo.isDir())
        {

            QString str = fileInfo.absolutePath() + "/" + strName + "/";
            dir_list.push_back(str);
        }
    }
    calibrateMachine.objectPoints.clear();
    calibrateMachine.cameraPoints.clear();
    calibrateMachine.dlpPoints.clear();

    calibrateMachine.boardSize.width=12;
    calibrateMachine.boardSize.height=11;

    for(int dir_i= 0;dir_i< dir_list.size(); dir_i++)
    {
        QDir dir(dir_list.at(dir_i));

//        qDebug()<<"folder: "<< dir;

         dir.setFilter(QDir::Files | QDir::NoSymLinks);//QDir::NoSymLinks - 不列出符号连接
                                                        //QDir::Files - 只列出文件
         dir.setSorting(QDir::Name);

         QStringList filter;
         filter<<QString("*.bmp");
         QFileInfoList  path=  dir.entryInfoList(filter);

        QStringList absoluteList;

         for (int i = 0; i < path.size(); i++) {

             QString filename = path.at(i).absoluteFilePath();

             absoluteList.push_back(filename);

//             qDebug() << filename;
         }

         QTime time1;
         time1.start();

         std::vector<cv::Mat> image_mat;

         for(int i= 0;i< absoluteList.size();i++)
         {
//             qDebug()<<path_list.at(i);
             cv::Mat srcImage=cv::imread(absoluteList.at(i).toStdString(),1);
             std::vector<cv::Mat> temp;
             cv::split(srcImage,temp);
             image_mat.push_back(temp[0]);
         }
         std::cout<<"image_mat size : "<<image_mat.size()<<std::endl;

         std::cout<<"read mat time: "<<QString::number(time1.restart()).toStdString()<<" ms\n";

         if(calibrateMachine.decodeThreeFrequencyHeterodyne(maskImage,image_mat,cali_decode_result,pattern_class) )
         {
             cornerFlag += 1;
         }

    }
    if( cornerFlag >=4 )
    {
        double error= calibrateMachine.structuredLightStereoCalibrate();
        std::cout<<"the system Stereo calibration error : "<<error<<std::endl;
        std::cout<<"the system calibration was finished ... "<<std::endl;
        paint_pattern.swap(cali_decode_result);
    }else{
        std::cout<<"Error: the number of calibration patterns is not enough! "
                   "\nError:Failed to calibrate the system ..."<<std::endl;
    }
}


void Widget::on_capture_button_slot()
{
    QTime time;
    time.start();

    std::vector<cv::Mat> pattern_sequence;

    pattern_class->parameter_setting(imageHeight,imageWidth);

    if(phaseErrorTable_flag && gamma_box->isChecked())
    {
        pattern_class->generate_gamma_pure_sinusoidal_pattern(pattern_sequence, 40,4,Blue,Vertical,blue_gamma_Aconst,blue_gamma_Bcoefficient);
        pattern_class->generate_gamma_pure_sinusoidal_pattern(pattern_sequence, 46,4,Blue,Vertical,blue_gamma_Aconst,blue_gamma_Bcoefficient);
        pattern_class->generate_gamma_pure_sinusoidal_pattern(pattern_sequence,350,4,Blue,Vertical,blue_gamma_Aconst,blue_gamma_Bcoefficient);

        pattern_class->generate_gamma_pure_sinusoidal_pattern(pattern_sequence, 40,4,Blue,Horizontal,blue_gamma_Aconst,blue_gamma_Bcoefficient);
        pattern_class->generate_gamma_pure_sinusoidal_pattern(pattern_sequence, 46,4,Blue,Horizontal,blue_gamma_Aconst,blue_gamma_Bcoefficient);
        pattern_class->generate_gamma_pure_sinusoidal_pattern(pattern_sequence,350,4,Blue,Horizontal,blue_gamma_Aconst,blue_gamma_Bcoefficient);
    }else{
        pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,40,4,Blue,Vertical);
        pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,46,4,Blue,Vertical);
        pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,350,4,Blue,Vertical);

        pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,40,4,Blue,Horizontal);
        pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,46,4,Blue,Horizontal);
        pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,350,4,Blue,Horizontal);
    }
    singleShot(1);
    if(!project_capture_pattern(pattern_sequence))
    {
        std::cout<<"Error : Failed to get the picture ... "<<std::endl;
        return;
    }

    //dlp::Time::Sleep::Microseconds(200000);
//    captureMachine.setCamera(&camera_pg);
//    captureMachine.start();

    /*********************************保存******************************/
    saveCalibrateImageNum += 1;
    QString showStr= "Save Calibrate Image number: "+ QString::number(saveCalibrateImageNum);

    showMessage(showStr);

    QString path;
    QString path1;

    if(saveCalibrateImageNum< 10)
    {
        path= board_image_path+ "/board_0"+ QString::number(saveCalibrateImageNum)+ ".bmp";
        path1= board_image_path+ "/binaryboard_0"+ QString::number(saveCalibrateImageNum)+ ".bmp";
    }else{
        path= board_image_path+ "/board_"+ QString::number(saveCalibrateImageNum)+ ".bmp";
        path1= board_image_path+ "/binaryboard_0"+ QString::number(saveCalibrateImageNum)+ ".bmp";
    }

    bool save= imwrite(path.toStdString(),boardImage);

    qDebug()<<"save board: "<<path<<" "<<save;

    cv::Mat binaryCornerImage = boardImage.clone();
    cv::threshold(binaryCornerImage,binaryCornerImage,5,255,cv::THRESH_BINARY);
    cv::imwrite(path1.toStdString(),binaryCornerImage);


    /*****************************************************************************************************/
    //创建标定文件夹
    QString temp_saveDir;
    if(saveCalibrateImageNum< 10)
    {
        temp_saveDir= calib_image_path+"/0"+QString::number(saveCalibrateImageNum)+ "/";
    }else{
        temp_saveDir= calib_image_path+"/"+QString::number(saveCalibrateImageNum)+ "/";
    }
    QDir temp_dir(temp_saveDir);
    if(!temp_dir.exists())
    {
       bool ok = temp_dir.mkdir(temp_saveDir);//只创建一级子目录，即必须保证上级目录存在
       qDebug()<<"create dir: "<<temp_saveDir<<" "<<ok;
    }

    //当前采集图片存储目录
    currentSaveDir= temp_saveDir;



//        if(saveCalibrateImageNum< 10)
//        {
        path= currentSaveDir+ "board.bmp";
//        }else{
//            path= currentSaveDir+ "board_"+ QString::number(saveCalibrateImageNum)+ ".bmp";
//        }


    save= imwrite(path.toStdString(),boardImage);
    qDebug()<<"save board: "<<path<<" "<<save;

    for(int i= 0;i< mat_list.size();i++)
    {
        if(i< 10)
        {
            path= currentSaveDir+ "capture_0"+ QString::number(i)+ ".bmp";
        }else{
            path= currentSaveDir+ "capture_"+ QString::number(i)+ ".bmp";
        }

        imwrite(path.toStdString(),mat_list.at(i));

    }

    captureMachine.setCamera(&camera_pg);
    captureMachine.start();

}
void Widget::singleShot(int channels)
{
    captureMachine.setCaptureFlag(false);
    dlp::Time::Sleep::Microseconds(100000);
    if(!project_flag)
    {
        textbrowser->append("\nthe project label is not ready!");
        return;
    }
    pattern_labbel->show();
    if(camera_pg.Start().hasErrors()){
        textbrowser->append("\nCould NOT start camera!");
        return;
    }

    camera_pg.ControlStrobe(2);

    dlp::Time::Sleep::Microseconds(500000);

    dlp::Image      captureImage;

    camera_pg.ControlStrobe(1);
    dlp::Time::Sleep::Microseconds(100000);//90000
    camera_pg.ControlStrobe(0);

    camera_pg.GetFrame(&captureImage);

    captureImage.GetOpenCVData(&boardImage);

    if(channels==1)
        cv::cvtColor(boardImage,boardImage,CV_RGB2GRAY);
    else{
        cv::cvtColor(boardImage,boardImage,CV_RGB2BGR);
    }

    //textbrowser->append("\nboardImage is Captured ... ");
    std::cout<<"boardImage is Captured ...\n";
}
void Widget::closeEvent( QCloseEvent * event )
{
    captureMachine.setCaptureFlag(false);
    int r=QMessageBox::question(this,tr("Last Hint"),tr("Are you sure you want to quit?"),QMessageBox::Yes|QMessageBox::Default,QMessageBox::No|QMessageBox::Escape);
    if(r==QMessageBox::Yes)    
    {
        event->accept();
        if(pattern_class != NULL)
            delete pattern_class;
        project_sleep(500);
        if(pattern_labbel != NULL)
        {
           delete  pattern_labbel;
        }

        for(int i = 0; i < 256; i++){
            for(int j = 0; j <256; j++){
                delete [] crosstalk_table[i][j];
            }
            delete [] crosstalk_table[i];
        }
        delete [] crosstalk_table;


    }
    else
        event->ignore();
}

void Widget::saveCapturePattern()
{
    QString filename = QFileDialog::getExistingDirectory(this,
        tr("Save Image"),
        "../ScanData"); //选择路径
    if(!filename.isEmpty())
    {
        QString path;
        for(int num=0;num<mat_list.size();num++)
        {
            if(num< 10)
            {
                path= filename+ "/capture_0"+ QString::number(num)+ ".bmp";
            }else{
                path= filename+ "/capture_"+ QString::number(num)+ ".bmp";
            }

            imwrite(path.toStdString(),mat_list.at(num));
            textbrowser->append("saving pattern ... "+QString::number(num));
        }
    }
}

void Widget::readExistPattern()
{
    QTime t1;
    t1.start();

    QString filename = QFileDialog::getExistingDirectory(this,
        tr("Read Image"),
        "../ScanData"); //选择路径
    if(filename.isEmpty())
    {
        std::cout<<"Error: Can not find the pattern file ... "<<std::endl;
        return;
    }

    QDir dir(filename);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);

    QStringList filter;
    filter<<QString("*.bmp");
    QFileInfoList path = dir.entryInfoList(filter);

    for(int num=0;num<path.size();++num)
    {
        QString filename = path.at(num).absoluteFilePath();
        cv::Mat srcImage=cv::imread(filename.toStdString(),1);
        mat_list.push_back(srcImage);
    }
    std::cout<<"read mat time: "<<t1.elapsed()<<" ms  ... "<<std::endl;
}

void Widget::phaseCode_sin_reconstruction()
{
    pattern_class->parameter_setting(imageHeight,imageWidth);
    pattern_class->image_paint.clear();

    int stepNum=4;

    std::vector<cv::Mat> pattern_sequence;
    if(gamma_box->isChecked())
    {
        pattern_class->generate_phaseCode_sin_pattern(pattern_sequence,40,Vertical,stepNum,1.8);
        pattern_class->generate_phaseCode_sin_pattern(pattern_sequence,40,Horizontal,stepNum,1.8);
        std::cout<<"gamma 1.8 compensation ... "<<std::endl;
    }
    else{
        pattern_class->generate_phaseCode_sin_pattern(pattern_sequence,40,Vertical,stepNum);
        pattern_class->generate_phaseCode_sin_pattern(pattern_sequence,40,Horizontal,stepNum);

    }

    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
    if(!project_capture_pattern(pattern_sequence))
    {
        textbrowser->append("\nError : Failed to capture the picture ... ");
        return;
    }

    //simulation
//    mat_list.swap(pattern_sequence);
//    maskImage=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));

    std::vector<cv::Mat> Rmat_list, Gmat_list, Bmat_list;
    for(int n=0;n<mat_list.size();n++)
    {
        std::vector<cv::Mat> mat;
        //串扰前
        cv::Mat beforeMat=mat_list[n].clone();
        //paint_pattern.push_back(beforeMat);

        if(crosstalk_flag)
        {
            //pattern_class->color_correction(maskImage,mat_list[n],mat_list[n],crosstalk_mat,reflect_mat);
            pattern_class->correct_crosstalk_by_LookUpTable(crosstalk_table,mat_list[n],mat_list[n],maskImage);
            //解耦后
            //paint_pattern.push_back(mat_list[n]);
        }

        cv::split(mat_list[n],mat);
        Bmat_list.push_back(mat[0]);
        Gmat_list.push_back(mat[1]);
        Rmat_list.push_back(mat[2]);
    }

    std::vector<cv::Mat> wrapPhaseX,wrapPhaseY,wrapPhase;

    for(int i=0;i<Bmat_list.size();i+=stepNum)
    {

        cv::Mat sinResult=cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_64FC1);
        cv::Mat codeResult=cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_64FC1);
        cv::Mat assistCodeResult = cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_64FC1);

        std::vector<cv::Mat> images1,images2,images3;
        images1.insert(images1.begin(),Bmat_list.begin()+i,Bmat_list.begin()+i+stepNum);
        images3.insert(images3.begin(),Gmat_list.begin()+i,Gmat_list.begin()+i+stepNum);
        images2.insert(images2.begin(),Rmat_list.begin()+i,Rmat_list.begin()+i+stepNum);

        pattern_class->decode_sinusoidal_LookUpTable(maskImage,images1,sinResult,stepNum);
        pattern_class->decode_sinusoidal_LookUpTable(maskImage,images2,codeResult,stepNum);
        pattern_class->decode_sinusoidal_LookUpTable(maskImage,images3,assistCodeResult,stepNum);
//        pattern_class->decode_multi_step_phaseShift(images1,sinResult,stepNum,2,maskImage);
//        pattern_class->decode_multi_step_phaseShift(images2,codeResult,stepNum,2,maskImage);
//        pattern_class->decode_multi_step_phaseShift(images3,assistCodeResult,stepNum,2,maskImage);

        wrapPhase.push_back(sinResult);
        wrapPhase.push_back(codeResult);
        wrapPhase.push_back(assistCodeResult);

//        paint_pattern.push_back(codeResult);
//        paint_pattern.push_back(assistCodeResult);
    }
    wrapPhaseX.insert(wrapPhaseX.begin(),wrapPhase.begin(),wrapPhase.begin()+3);
    wrapPhaseY.insert(wrapPhaseY.begin(),wrapPhase.begin()+3,wrapPhase.begin()+6);
//    wrapPhaseX.insert(wrapPhaseX.begin(),wrapPhase.begin(),wrapPhase.begin()+2);
//    wrapPhaseY.insert(wrapPhaseY.begin(),wrapPhase.begin()+2,wrapPhase.begin()+4);


    pattern_class->image_paint.clear();
    pattern_class->decode_phaseCode_sin_pattern(wrapPhaseX,unwrapPhaseX,maskImage,Vertical);
    pattern_class->decode_phaseCode_sin_pattern(wrapPhaseY,unwrapPhaseY,maskImage,Horizontal);

    paint_pattern.insert(paint_pattern.end(),pattern_class->image_paint.begin(),pattern_class->image_paint.end());

    //绘图
    cv::Mat unwrapX = unwrapPhaseX.clone();
    cv::Mat unwrapY = unwrapPhaseY.clone();
    cv::normalize(unwrapPhaseX,unwrapX,1,0,cv::NORM_MINMAX);
    cv::normalize(unwrapPhaseY,unwrapY,1,0,cv::NORM_MINMAX);
    unwrapX.convertTo(unwrapX,CV_8UC1,255,0);
    unwrapY.convertTo(unwrapY,CV_8UC1,255,0);
    cv::imwrite("../ScanData/1.bmp",unwrapX);
    cv::imwrite("../ScanData/2.bmp",unwrapY);
}

void Widget::composed_sin_reconstruction()
{
    pattern_class->parameter_setting(imageHeight,imageWidth);
    pattern_class->image_paint.clear();

    int stepNum=5;

    std::vector<cv::Mat> pattern_sequence;
    if(gamma_box->isChecked() && gamma_flag)
    {
        pattern_class->generate_composed_sin_pattern(pattern_sequence,80,stepNum,2);
    }
    else{
        pattern_class->generate_composed_sin_pattern(pattern_sequence,80,stepNum);
    }

//    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
//    if(!project_capture_pattern(pattern_sequence))
//    {
//        textbrowser->append("\nError : Failed to capture the picture ... ");
//        return;
//    }

    //simulation
    mat_list.swap(pattern_sequence);
    maskImage=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));

    std::vector<cv::Mat> Rmat_list, Bmat_list;
    for(int n=0;n<mat_list.size();n++)
    {
        std::vector<cv::Mat> mat;
        cv::split(mat_list[n],mat);
        Bmat_list.push_back(mat[0]);
        Rmat_list.push_back(mat[2]);
    }

    std::vector<cv::Mat> wrapPhaseX,wrapPhaseY;
    cv::Mat wrapSinX,wrapSinY;
    cv::Mat phaseCodeX,phaseCodeY;

    wrapSinX=cv::Mat::zeros(imageHeight,imageWidth,CV_64FC1);
    wrapSinY=cv::Mat::zeros(imageHeight,imageWidth,CV_64FC1);
    phaseCodeX=cv::Mat::zeros(imageHeight,imageWidth,CV_64FC1);
    phaseCodeY=cv::Mat::zeros(imageHeight,imageWidth,CV_64FC1);

    pattern_class->decode_multi_step_phaseShift(Bmat_list,wrapSinX,stepNum,2,maskImage);
    pattern_class->decode_multi_step_phaseShift(Bmat_list,phaseCodeX,stepNum,4,maskImage);
    pattern_class->decode_multi_step_phaseShift(Rmat_list,wrapSinY,stepNum,2,maskImage);
    pattern_class->decode_multi_step_phaseShift(Rmat_list,phaseCodeY,stepNum,4,maskImage);

    wrapPhaseX.push_back(wrapSinX);
    wrapPhaseX.push_back(phaseCodeX);
    wrapPhaseY.push_back(wrapSinY);
    wrapPhaseY.push_back(phaseCodeY);

    paint_pattern.insert(paint_pattern.end(),wrapPhaseX.begin(),wrapPhaseX.end());
    paint_pattern.insert(paint_pattern.end(),wrapPhaseY.begin(),wrapPhaseY.end());

}

void Widget::Fourier_sin_reconstruction()
{
    imageHeight=1080;
    imageWidth=1920;
    pattern_class->parameter_setting(imageHeight,imageWidth);
    pattern_class->image_paint.clear();

    int stepNum=4;

    std::vector<cv::Mat> pattern_sequence;
    if(gamma_box->isChecked() && gamma_flag)
    {
        pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,30,stepNum,Blue,Vertical,gammaValue);
    }
    else{
        pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,40,stepNum,Blue,Vertical);
    }

//    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
//    if(!project_capture_pattern(pattern_sequence))
//    {
//        textbrowser->append("\nError : Failed to capture the picture ... ");
//        return;
//    }

    //simulation
    mat_list.swap(pattern_sequence);
    maskImage=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));

    std::vector<cv::Mat> Bmat_list;
    for(int n=0;n<mat_list.size();n++)
    {
        std::vector<cv::Mat> mat;
        cv::split(mat_list[n],mat);
        Bmat_list.push_back(mat[0]);
    }

    cv::Mat wrapPhase;
    pattern_class->decode_sinusoidal(maskImage,Bmat_list,wrapPhase,4);

    cv::Mat fourierMat;
    cv::Mat wrapPhase1=cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_64FC1);
    pattern_class->wrap_fourier_transform(Bmat_list[0],fourierMat,Circle,12,5);

//    for(unsigned int i=0;i<Bmat_list[0].rows;++i)
//    {
//        double* matPtr=fourierMat.ptr<double>(i);
//        uchar* maskPtr=maskImage.ptr<uchar>(i);
//        double* wrapPhasePtr=wrapPhase1.ptr<double>(i);

//        for(unsigned int j=0;j<Bmat_list[0].cols;++j)
//        {
//            if(maskPtr[j]!=0)
//            {
//                double real=matPtr[j*2];
//                double img=matPtr[j*2+1];
//                wrapPhasePtr[j]=atan2(img,real)+myPI;
//            }
//        }
//    }
//    //cv::normalize(wrapPhase1,wrapPhase1,1.0,0.0,cv::NORM_MINMAX);

//    paint_pattern.push_back(wrapPhase);
//    paint_pattern.push_back(wrapPhase1);
    paint_pattern.insert(paint_pattern.end(), pattern_class->image_paint.begin(),pattern_class->image_paint.end());
}

void Widget::phaseCode_grayCode_sin_reconstruction()
{

    pattern_class->parameter_setting(imageHeight,imageWidth);
    pattern_class->image_paint.clear();

    int stepNum=4;

    std::vector<cv::Mat> pattern_sequence;
    if(gamma_box->isChecked())
    {
        pattern_class->generate_gamma_phaseCode_grayCode_sin_pattern(pattern_sequence,40,Vertical,stepNum
                                                                     ,blue_gamma_Aconst,blue_gamma_Bcoefficient);
        pattern_class->generate_gamma_phaseCode_grayCode_sin_pattern(pattern_sequence,40,Horizontal,stepNum
                                                                     ,blue_gamma_Aconst,blue_gamma_Bcoefficient);
    }
    else{
        pattern_class->generate_phaseCode_grayCode_sin_pattern(pattern_sequence,40,Vertical,stepNum);
        pattern_class->generate_phaseCode_grayCode_sin_pattern(pattern_sequence,40,Horizontal,stepNum);
    }

    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
    if(!project_capture_pattern(pattern_sequence))
    {
        textbrowser->append("\nError : Failed to capture the picture ... ");
        return;
    }

    //simulation
//    mat_list.swap(pattern_sequence);
//    maskImage=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));

    ////////////// save the capture pattern
    // saveCapturePattern();

    std::vector<cv::Mat> Rmat_list, Gmat_list, Bmat_list;
    std::cout<<"mat_list size : "<<mat_list.size()<<std::endl;
    for(int n=0;n<mat_list.size();n++)
    {
        std::vector<cv::Mat> channels;
        //串扰前
        cv::Mat beforeMat=mat_list[n].clone();
        //paint_pattern.push_back(beforeMat);

        if(crosstalk_flag)
        {
            cv::Mat correctMat1 = cv::Mat::zeros(mat_list[n].rows,mat_list[n].cols,CV_8UC3);
            cv::Mat correctMat2 = cv::Mat::zeros(mat_list[n].rows,mat_list[n].cols,CV_8UC3);

//            correctMat1 = mat_list[n].clone();
            //pattern_class->color_correction(maskImage,mat_list[n],mat_list[n],crosstalk_mat,reflect_mat);
             pattern_class->correct_crosstalk_by_LookUpTable(crosstalk_table,mat_list[n],mat_list[n],maskImage);
//            correctMat2 = mat_list[n].clone();
//             paint_pattern.push_back(correctMat1);
//             paint_pattern.push_back(correctMat2);
        }

//        cv::imwrite("../ScanData/measure_image/"+QString::number(n).toStdString()+".bmp",mat_list[n]);

        cv::split(mat_list[n],channels);
        Bmat_list.push_back(channels[0]);
        Gmat_list.push_back(channels[1]);
        Rmat_list.push_back(channels[2]);

    }

    std::vector<cv::Mat> wrapPhaseX,wrapPhaseY,wrapPhase;

    for(int i=0;i<Bmat_list.size();i+=stepNum)
    {
        std::vector<cv::Mat> images1,images2,images3;
        images1.insert(images1.begin(),Bmat_list.begin()+i,Bmat_list.begin()+i+stepNum);
        images3.insert(images3.begin(),Gmat_list.begin()+i,Gmat_list.begin()+i+4);
        images2.insert(images2.begin(),Rmat_list.begin()+i,Rmat_list.begin()+i+stepNum);

        std::cout<<"images1 size : "<<images1.size()<<std::endl;
        std::cout<<"images2 size : "<<images2.size()<<std::endl;
        std::cout<<"images3 size : "<<images3.size()<<std::endl;

//        if(i==0)
//        {
//            cv::imwrite("../ScanData/measure_image/sin_1.bmp",Bmat_list[i]);
//            cv::imwrite("../ScanData/measure_image/sin_2.bmp",Bmat_list[i+1]);
//            cv::imwrite("../ScanData/measure_image/sin_3.bmp",Bmat_list[i+2]);
//            cv::imwrite("../ScanData/measure_image/sin_4.bmp",Bmat_list[i+3]);
//            cv::imwrite("../ScanData/measure_image/phase_1.bmp",Rmat_list[i]);
//            cv::imwrite("../ScanData/measure_image/phase_2.bmp",Rmat_list[i+1]);
//            cv::imwrite("../ScanData/measure_image/phase_3.bmp",Rmat_list[i+2]);
//            cv::imwrite("../ScanData/measure_image/phase_4.bmp",Rmat_list[i+3]);
//            cv::imwrite("../ScanData/measure_image/gray_1.bmp",Gmat_list[i]);
//            cv::imwrite("../ScanData/measure_image/gray_2.bmp",Gmat_list[i+1]);
//            cv::imwrite("../ScanData/measure_image/gray_3.bmp",Gmat_list[i+2]);
//            cv::imwrite("../ScanData/measure_image/gray_4.bmp",Gmat_list[i+3]);
//        }

        cv::Mat sinResult=cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_64FC1);
        cv::Mat phaseCodeResult=cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_64FC1);
        cv::Mat grayCodeResult = cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_8UC1);

//        pattern_class->decode_sinusoidal(maskImage,images1,sinResult,stepNum);
//        pattern_class->decode_sinusoidal(maskImage,images2,phaseCodeResult,stepNum);

        pattern_class->decode_sinusoidal_LookUpTable(maskImage,images1,sinResult,stepNum);
        pattern_class->decode_sinusoidal_LookUpTable(maskImage,images2,phaseCodeResult,stepNum);

//        pattern_class->decode_multi_step_phaseShift(images1,sinResult,stepNum,2,maskImage);
//        pattern_class->decode_multi_step_phaseShift(images2,phaseCodeResult,stepNum,2,maskImage);
        pattern_class->decode_grayCode(images3,grayCodeResult,maskImage);

        //phaseCode
        if(i==0){
            //paint_pattern.insert(paint_pattern.end(),images2.begin(),images2.end());
        }
        cv::Mat phasecodepaint = phaseCodeResult.clone();
        paint_pattern.push_back(phasecodepaint);

        wrapPhase.push_back(sinResult);
        wrapPhase.push_back(phaseCodeResult);
        wrapPhase.push_back(grayCodeResult);


//        cv::Mat wrapPhasexxx = sinResult.clone();
//        cv::normalize(wrapPhasexxx,wrapPhasexxx,1.0,0.0,cv::NORM_MINMAX);
//        wrapPhasexxx.convertTo(wrapPhasexxx,CV_8UC1,255,0);
//        cv::imwrite("../ScanData/3.bmp",wrapPhasexxx);

    }
    wrapPhaseX.insert(wrapPhaseX.begin(),wrapPhase.begin(),wrapPhase.begin()+3);
    wrapPhaseY.insert(wrapPhaseY.begin(),wrapPhase.begin()+3,wrapPhase.begin()+6);

    pattern_class->image_paint.clear();
    unwrapPhaseX=cv::Mat::zeros(wrapPhaseX[0].rows,wrapPhaseX[0].cols,CV_64FC1);
    unwrapPhaseY=cv::Mat::zeros(wrapPhaseX[0].rows,wrapPhaseX[0].cols,CV_64FC1);
    pattern_class->decode_phaseCode_grayCode_sin_pattern(wrapPhaseX,unwrapPhaseX,maskImage,Vertical);
    pattern_class->decode_phaseCode_grayCode_sin_pattern(wrapPhaseY,unwrapPhaseY,maskImage,Horizontal);

    paint_pattern.insert(paint_pattern.end(),pattern_class->image_paint.begin(),pattern_class->image_paint.end());

    cv::Mat wrapx,wrapy;
    cv::normalize(unwrapPhaseX,wrapx,1.0,0.0,cv::NORM_MINMAX);
    cv::normalize(unwrapPhaseY,wrapy,1.0,0.0,cv::NORM_MINMAX);
    wrapx.convertTo(wrapx,CV_8UC1,255,0);
    wrapy.convertTo(wrapy,CV_8UC1,255,0);

    cv::imwrite("../ScanData/1.bmp",wrapx);
    cv::imwrite("../ScanData/2.bmp",wrapy);

}

void Widget::multiFrequencyHeterodyne_reconstruction()
{
    std::vector<cv::Mat> pattern_sequence;
    pattern_class->parameter_setting(imageHeight,imageWidth);
    int stepNum = 4;

    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,40,stepNum,Blue,Vertical);
    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,46,stepNum,Blue,Vertical);
    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,350,stepNum,Blue,Vertical);

    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,40,stepNum,Blue,Horizontal);
    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,46,stepNum,Blue,Horizontal);
    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,350,stepNum,Blue,Horizontal);

   /*********************************************************************/

//    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
//    if(!project_capture_pattern(pattern_sequence))
//    {
//        textbrowser->append("\nError : Failed to capture the picture ... ");
//        return;
//    }

    //*******************simulation****************//    
    maskImage=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));
    mat_list.swap(pattern_sequence);

//    for(int n = 0; n < mat_list.size(); n++){

//        cv::imwrite("../ScanData/measure_image/"+QString::number(n).toStdString()+".bmp",mat_list[n]);
//    }

    std::vector<cv::Mat> Bmat_list;
    for(int n=0;n<mat_list.size();n++)
    {    
//        if(crosstalk_flag){
//            pattern_class->correct_crosstalk_by_LookUpTable(crosstalk_table,mat_list[n],mat_list[n],maskImage);
//        }
        std::vector<cv::Mat> Bmat;
        cv::split(mat_list[n],Bmat);
        Bmat_list.push_back(Bmat[0]);
    }

    std::vector<cv::Mat> after_wrap_list;

    std::vector<cv::Mat> before_wrap_list,wrapPhaseX,wrapPhaseY;

    std::vector<cv::Mat> images;
    images.reserve(20);

    for(int i= 0;i< Bmat_list.size();i+= stepNum)
    {
        images.clear();
        images.insert(images.begin(),Bmat_list.begin()+i,Bmat_list.begin()+i+stepNum);

        cv::Mat result=cv::Mat::zeros(images[0].rows,images[0].cols,CV_64FC1);
        pattern_class->decode_sinusoidal_LookUpTable(maskImage,images,result,stepNum);
        //pattern_class->decode_multi_step_phaseShift(images,result,stepNum,2,maskImage);

        //显示某种频率
//        if(i==0 ){
//        paint_pattern.push_back(result);
////        cv::Mat dst = result.clone();
////        cv::normalize(dst,dst,1.0,0.0,cv::NORM_MINMAX);
////        dst.convertTo(dst,CV_8UC1,255,0);
////        cv::imwrite("../ScanData/3.bmp",dst);
//        }

        cv::Mat before_correct = result.clone();
        paint_pattern.push_back(before_correct);
        before_wrap_list.push_back(before_correct);

        //Look up table compention
//        if(phaseErrorTable_flag)
//        {
//            pattern_class->correct_phase(result,result,maskImage,1);
//            wrap_list.push_back(result);
//            //paint_pattern.push_back(result);
//        }

    }

    wrapPhaseX.insert(wrapPhaseX.begin(),before_wrap_list.begin(),before_wrap_list.begin()+3);
    wrapPhaseY.insert(wrapPhaseY.begin(),before_wrap_list.begin()+3,before_wrap_list.begin()+6);

    cv::Mat before_unwrapMap_x = cv::Mat::zeros(wrapPhaseX[0].rows,wrapPhaseX[0].cols,CV_64FC1);
    cv::Mat before_unwrapMap_y = cv::Mat::zeros(wrapPhaseX[0].rows,wrapPhaseX[0].cols,CV_64FC1);
    pattern_class->decode_threefrequency(maskImage,wrapPhaseX,before_unwrapMap_x,stepNum);
    pattern_class->decode_threefrequency(maskImage,wrapPhaseY,before_unwrapMap_y,stepNum);

    paint_pattern.push_back(before_unwrapMap_x);
    paint_pattern.push_back(before_unwrapMap_y);


//    cv::Mat unwrapMap_x;
//    cv::Mat unwrapMap_y;
//    if(phaseErrorTable_flag)
//    {
//        pattern_class->decode_threefrequency(maskImage,wrap_list_x,unwrapMap_x,stepNum);
//        pattern_class->decode_threefrequency(maskImage,wrap_list_y,unwrapMap_y,stepNum);
//        //paint_pattern.push_back(unwrapMap_x);
//        //paint_pattern.push_back(unwrapMap_y);
//    }

    unwrapPhaseX = before_unwrapMap_x.clone();
    unwrapPhaseY = before_unwrapMap_y.clone();

    cv::Mat wrapx,wrapy;
    cv::normalize(unwrapPhaseX,wrapx,1.0,0.0,cv::NORM_MINMAX);
    cv::normalize(unwrapPhaseY,wrapy,1.0,0.0,cv::NORM_MINMAX);
    wrapx.convertTo(wrapx,CV_8UC1,255,0);
    wrapy.convertTo(wrapy,CV_8UC1,255,0);

    cv::imwrite("../ScanData/1.bmp",wrapx);
    cv::imwrite("../ScanData/2.bmp",wrapy);
}

void Widget::binary_grayCode_sin_reconstruction()
{
    imageHeight=800;
    imageWidth=1280;
    pattern_class->parameter_setting(imageHeight,imageWidth);
    pattern_class->image_paint.clear();

    int stepNum=4;

    std::vector<cv::Mat> pattern_sequence;
    if(gamma_box->isChecked())
    {
        pattern_class->generate_binary_grayCode_sin_pattern(pattern_sequence,80,Vertical,stepNum,1.8);
        pattern_class->generate_binary_grayCode_sin_pattern(pattern_sequence,80,Horizontal,stepNum,1.8);
    }
    else{
        pattern_class->generate_binary_grayCode_sin_pattern(pattern_sequence,80,Vertical,stepNum);
        pattern_class->generate_binary_grayCode_sin_pattern(pattern_sequence,80,Horizontal,stepNum);
    }

//    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
//    if(!project_capture_pattern(pattern_sequence))
//    {
//        textbrowser->append("\nError : Failed to capture the picture ... ");
//        return;
//    }

    //simulation
    mat_list.swap(pattern_sequence);
    maskImage=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));

    ////////////// save the capture pattern
    //saveCapturePattern();

    std::vector<cv::Mat> Rmat_list, Gmat_list, Bmat_list;
    for(int n=0;n<mat_list.size();n++)
    {
        std::vector<cv::Mat> mat;
        //串扰前
        cv::Mat beforeMat=mat_list[n].clone();
        //paint_pattern.push_back(beforeMat);

        if(crosstalk_flag)
        {
            pattern_class->color_correction(maskImage,mat_list[n],mat_list[n],crosstalk_mat,reflect_mat);
            //解耦后
            //paint_pattern.push_back(mat_list[n]);
        }

        cv::split(mat_list[n],mat);
        Bmat_list.push_back(mat[0]);
        Gmat_list.push_back(mat[1]);
        Rmat_list.push_back(mat[2]);
    }

    std::vector<cv::Mat> wrapPhaseX,wrapPhaseY,wrapPhase;

    for(int i=0;i<Bmat_list.size();i+=stepNum)
    {
        cv::Mat sinResult,grayCodeResult;

        std::vector<cv::Mat> images1,images2;
        images1.push_back(Bmat_list[i]);
        images1.push_back(Bmat_list[i+1]);
        images1.push_back(Bmat_list[i+2]);
        if(stepNum==4) images1.push_back(Bmat_list[i+3]);

        images2.push_back(Gmat_list[i]);
        images2.push_back(Gmat_list[i+1]);
        images2.push_back(Gmat_list[i+2]);
        if(stepNum==4) images2.push_back(Gmat_list[i+3]);

        images2.push_back(Rmat_list[i]);
        images2.push_back(Rmat_list[i+1]);
        images2.push_back(Rmat_list[i+2]);
        if(stepNum==4) images2.push_back(Rmat_list[i+3]);

       // paint_pattern.insert(paint_pattern.end(),Gmat_list.begin()+i,Gmat_list.begin()+i+4);

        pattern_class->decode_sinusoidal(maskImage,images1,sinResult,stepNum);

        pattern_class->decode_multi_grayCode(images2,grayCodeResult,maskImage);

        if(phaseErrorTable_flag && gamma_box->isChecked())
        {
            pattern_class->correct_phase(sinResult,sinResult,maskImage,1);
        }

        wrapPhase.push_back(sinResult);
        wrapPhase.push_back(grayCodeResult);

    }
    wrapPhaseX.insert(wrapPhaseX.begin(),wrapPhase.begin(),wrapPhase.begin()+2);
    wrapPhaseY.insert(wrapPhaseY.begin(),wrapPhase.begin()+2,wrapPhase.begin()+4);

    pattern_class->image_paint.clear();
    pattern_class->decode_grayCode_sin_pattern(wrapPhaseX,unwrapPhaseX,maskImage,Vertical);
    pattern_class->decode_grayCode_sin_pattern(wrapPhaseY,unwrapPhaseY,maskImage,Horizontal);

    paint_pattern.insert(paint_pattern.end(),pattern_class->image_paint.begin(),pattern_class->image_paint.end());

    cv::normalize(unwrapPhaseX,unwrapPhaseX,1,0,cv::NORM_MINMAX);
    cv::normalize(unwrapPhaseY,unwrapPhaseY,1,0,cv::NORM_MINMAX);
    unwrapPhaseX.convertTo(unwrapPhaseX,CV_8UC1,255,0);
    unwrapPhaseY.convertTo(unwrapPhaseY,CV_8UC1,255,0);
    cv::imwrite("../ScanData/measure_image/unwrapX.bmp",unwrapPhaseX);
    cv::imwrite("../ScanData/measure_image/unwrapY.bmp",unwrapPhaseY);
}

void Widget::color_sin_phaseCode_reconstruction()
{
    pattern_class->parameter_setting(imageHeight,imageWidth);
    pattern_class->image_paint.clear();

    std::vector<cv::Mat> pattern_sequence;
    pattern_class->generate_color_sin_phaseCode_phaseshift_pattern(pattern_sequence,40,Vertical);
    pattern_class->generate_color_sin_phaseCode_phaseshift_pattern(pattern_sequence,40,Horizontal);


//    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
//    if(!project_capture_pattern(pattern_sequence))
//    {
//        textbrowser->append("\nError : Failed to capture the picture ... ");
//        return;
//    }

    //simulation
    mat_list.swap(pattern_sequence);
    maskImage=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));

    std::vector<cv::Mat> wrapPhaseX,wrapPhaseY,wrapPhase;
    for(int n=0;n<mat_list.size();n++)
    {
        std::vector<cv::Mat> mat;
        //串扰前
        cv::Mat beforeMat=mat_list[n].clone();
        //paint_pattern.push_back(beforeMat);

        if(crosstalk_flag)
        {
            pattern_class->color_correction(maskImage,mat_list[n],mat_list[n],crosstalk_mat,reflect_mat);
            //解耦后
            //paint_pattern.push_back(mat_list[n]);
        }

        cv::split(mat_list[n],mat);

        cv::Mat Result = cv::Mat::zeros(mat_list[0].rows,mat_list[0].cols,CV_64FC1);

        pattern_class->decode_sinusoidal_LookUpTable(maskImage,mat,Result,3);

        wrapPhase.push_back(Result);

    }

    wrapPhaseX.insert(wrapPhaseX.begin(),wrapPhase.begin(),wrapPhase.begin()+2);
    wrapPhaseY.insert(wrapPhaseY.begin(),wrapPhase.begin()+2,wrapPhase.begin()+4);

    pattern_class->image_paint.clear();
    pattern_class->decode_color_sin_phaseCode_phaseshift_pattern(wrapPhaseX,unwrapPhaseX,maskImage,Vertical);
    pattern_class->decode_color_sin_phaseCode_phaseshift_pattern(wrapPhaseY,unwrapPhaseY,maskImage,Horizontal);

    paint_pattern.insert(paint_pattern.end(),pattern_class->image_paint.begin(),pattern_class->image_paint.end());

    cv::Mat unwrapX = unwrapPhaseX.clone();
    cv::Mat unwrapY = unwrapPhaseY.clone();
    cv::normalize(unwrapPhaseX,unwrapX,1,0,cv::NORM_MINMAX);
    cv::normalize(unwrapPhaseY,unwrapY,1,0,cv::NORM_MINMAX);
    unwrapX.convertTo(unwrapX,CV_8UC1,255,0);
    unwrapY.convertTo(unwrapY,CV_8UC1,255,0);
    cv::imwrite("../ScanData/1.bmp",unwrapX);
    cv::imwrite("../ScanData/2.bmp",unwrapY);

}

void Widget::pure_sin_phaseCode_reconstruction()
{
    pattern_class->parameter_setting(imageHeight,imageWidth);
    pattern_class->image_paint.clear();

    int stepNum=4;
    std::vector<cv::Mat> pattern_sequence;

    pattern_class->generate_pure_phaseCode_pattern(pattern_sequence,40,stepNum,Vertical);
    pattern_class->generate_pure_phaseCode_pattern(pattern_sequence,40,stepNum,Horizontal);


//    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
//    if(!project_capture_pattern(pattern_sequence))
//    {
//        textbrowser->append("\nError : Failed to capture the picture ... ");
//        return;
//    }

    //simulation
    mat_list.swap(pattern_sequence);
    maskImage=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));

    std::vector<cv::Mat> Bmat_list;
    for(int n=0;n<mat_list.size();n++)
    {
        std::vector<cv::Mat> mat;
        //串扰前
        cv::Mat beforeMat=mat_list[n].clone();
        //paint_pattern.push_back(beforeMat);

        if(crosstalk_flag)
        {
            //pattern_class->color_correction(maskImage,mat_list[n],mat_list[n],crosstalk_mat,reflect_mat);
            pattern_class->correct_crosstalk_by_LookUpTable(crosstalk_table,mat_list[n],mat_list[n],maskImage);
            //解耦后
            //paint_pattern.push_back(mat_list[n]);
        }
        cv::split(mat_list[n],mat);
        Bmat_list.push_back(mat[0]);
    }

    for(int i=0;i<Bmat_list.size();i++ )
    {
        cv::imwrite("../ScanData/measure_image/"+QString::number(i).toStdString()+".bmp",Bmat_list[i]);
    }

    std::vector<cv::Mat> wrapPhaseX,wrapPhaseY,wrapPhase;

    int num = 3;
    for(int i=0;i<Bmat_list.size();i+=stepNum*num )
    {

        cv::Mat sinResult=cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_64FC1);
        cv::Mat codeResult=cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_64FC1);
        cv::Mat assistCodeResult = cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_64FC1);

        std::vector<cv::Mat> images1,images2,images3;
        images1.insert(images1.begin(),Bmat_list.begin()+i,Bmat_list.begin()+i+stepNum);
        images2.insert(images2.begin(),Bmat_list.begin()+i+stepNum,Bmat_list.begin()+i+stepNum*2);
        if(num==3)images3.insert(images3.begin(),Bmat_list.begin()+i+stepNum*2,Bmat_list.begin()+i+stepNum*3);

        pattern_class->decode_sinusoidal_LookUpTable(maskImage,images1,sinResult,stepNum);
        pattern_class->decode_sinusoidal_LookUpTable(maskImage,images2,codeResult,stepNum);
        if(num==3)pattern_class->decode_sinusoidal_LookUpTable(maskImage,images3,assistCodeResult,stepNum);
//        pattern_class->decode_multi_step_phaseShift(images1,sinResult,stepNum,2,maskImage);
//        pattern_class->decode_multi_step_phaseShift(images2,codeResult,stepNum,2,maskImage);
//        pattern_class->decode_multi_step_phaseShift(images3,assistCodeResult,stepNum,2,maskImage);

        wrapPhase.push_back(sinResult);
        wrapPhase.push_back(codeResult);
        wrapPhase.push_back(assistCodeResult);

//        paint_pattern.push_back(codeResult);
//        paint_pattern.push_back(assistCodeResult);
    }
    wrapPhaseX.insert(wrapPhaseX.begin(),wrapPhase.begin(),wrapPhase.begin()+3);
    wrapPhaseY.insert(wrapPhaseY.begin(),wrapPhase.begin()+3,wrapPhase.begin()+6);
//    wrapPhaseX.insert(wrapPhaseX.begin(),wrapPhase.begin(),wrapPhase.begin()+2);
//    wrapPhaseY.insert(wrapPhaseY.begin(),wrapPhase.begin()+2,wrapPhase.begin()+4);

    pattern_class->image_paint.clear();
    pattern_class->decode_phaseCode_sin_pattern(wrapPhaseX,unwrapPhaseX,maskImage,Vertical);
    pattern_class->decode_phaseCode_sin_pattern(wrapPhaseY,unwrapPhaseY,maskImage,Horizontal);

    paint_pattern.insert(paint_pattern.end(),pattern_class->image_paint.begin(),pattern_class->image_paint.end());

    //绘图
    cv::Mat unwrapX = unwrapPhaseX.clone();
    cv::Mat unwrapY = unwrapPhaseY.clone();
    cv::normalize(unwrapPhaseX,unwrapX,1,0,cv::NORM_MINMAX);
    cv::normalize(unwrapPhaseY,unwrapY,1,0,cv::NORM_MINMAX);
    unwrapX.convertTo(unwrapX,CV_8UC1,255,0);
    unwrapY.convertTo(unwrapY,CV_8UC1,255,0);
    cv::imwrite("../ScanData/1.bmp",unwrapX);
    cv::imwrite("../ScanData/2.bmp",unwrapY);
}

void Widget::composite_color_code_reconstruction()
{
        pattern_class->parameter_setting(imageHeight,imageWidth);
        pattern_class->image_paint.clear();

        int stepNum = 5;
        std::vector<cv::Mat> pattern_sequence;
        pattern_sequence.reserve(20);

        pattern_class->generate_color_composite_code_pattern(pattern_sequence,90,95,stepNum,Vertical);
        pattern_class->generate_color_composite_code_pattern(pattern_sequence,80,85,stepNum,Horizontal);

//        textbrowser->append("\nthe number of pattern is" + QString::number(pattern_sequence.size()));
//        if(!project_capture_pattern(pattern_sequence))
//        {
//            textbrowser->append("\nError: Failed to capture the pattern ... ");
//            return;
//        }

        //simulation
        mat_list.swap(pattern_sequence);
        maskImage = cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));

        ///////save the capture pattern
        //saveCapturePattern();

        std::vector<cv::Mat> Rmat_list,Gmat_list,Bmat_list;
        std::cout<<"mat_list size : "<<mat_list.size()<<std::endl;
        for(int n = 0 ; n < mat_list.size(); n++){
            std::vector<cv::Mat> channels;
//            if(n==0){
//                paint_pattern.push_back(mat_list[n]);
//            }
            if(crosstalk_flag){
                pattern_class->correct_crosstalk_by_LookUpTable(crosstalk_table,mat_list[n],mat_list[n],maskImage);
            }
//            if(n==0){
//                paint_pattern.push_back(mat_list[n]);
//            }
            cv::split(mat_list[n],channels);

            cv::GaussianBlur(channels[0],channels[0],cv::Size(15,15),5,5);
            cv::GaussianBlur(channels[2],channels[2],cv::Size(15,15),5,5);

            Bmat_list.push_back(channels[0]);
           // Gmat_list.push_back(channels[1]);
            Rmat_list.push_back(channels[2]);

//            if(n==0 || n==5){
//            paint_pattern.push_back(channels[0]);
//            paint_pattern.push_back(channels[2]);
//            }

        }
        int imageRows = Bmat_list[0].rows;
        int imageCols = Bmat_list[0].cols;
        std::vector<cv::Mat> wrapPhaseX,wrapPhaseY;
        for(int i=0; i<Bmat_list.size(); i+=stepNum ){

            std::vector<cv::Mat> images1,images3;
            images1.insert(images1.begin(),Bmat_list.begin()+i,Bmat_list.begin()+i+stepNum);

            images3.insert(images3.begin(),Rmat_list.begin()+i,Rmat_list.begin()+i+stepNum);

            cv::Mat sinResult_f1 = cv::Mat::zeros(imageRows,imageCols,CV_64FC1);
            cv::Mat sinResult_f11 = cv::Mat::zeros(imageRows,imageCols,CV_64FC1);
            cv::Mat sinResult_f2 = cv::Mat::zeros(imageRows,imageCols,CV_64FC1);
            cv::Mat sinResult_f22 = cv::Mat::zeros(imageRows,imageCols,CV_64FC1);

            pattern_class->decode_multi_step_phaseShift(images1,sinResult_f1,stepNum,2,maskImage);
            pattern_class->decode_multi_step_phaseShift(images3,sinResult_f11,stepNum,2,maskImage);
            pattern_class->decode_multi_step_phaseShift(images1,sinResult_f2,stepNum,4,maskImage);
            pattern_class->decode_multi_step_phaseShift(images3,sinResult_f22,stepNum,4,maskImage);

//            paint_pattern.push_back(sinResult_f1);
//            paint_pattern.push_back(sinResult_f11);
//            paint_pattern.push_back(sinResult_f2);
//            paint_pattern.push_back(sinResult_f22);

    //        cv::normalize(sinResult,sinResult,1.0,0.0,cv::NORM_MINMAX);
    //        cv::normalize(phaseCodeResult,phaseCodeResult,1.0,0.0,cv::NORM_MINMAX);
    //        sinResult.convertTo(sinResult,CV_8UC1,255,0);
    //        phaseCodeResult.convertTo(phaseCodeResult,CV_8UC1,255,0);
    //        cv::imwrite("../ScanData/sinResult"+QString::number(i).toStdString()+".bmp",sinResult);
    //        cv::imwrite("../ScanData/phaseCodeResult"+QString::number(i).toStdString()+".bmp",phaseCodeResult);

            if(i==0){
                wrapPhaseX.push_back(sinResult_f1);
                wrapPhaseX.push_back(sinResult_f2);
                wrapPhaseX.push_back(sinResult_f11);
                wrapPhaseX.push_back(sinResult_f22);

            }else{
                wrapPhaseY.push_back(sinResult_f1);
                wrapPhaseY.push_back(sinResult_f2);
                wrapPhaseY.push_back(sinResult_f11);
                wrapPhaseY.push_back(sinResult_f22);
            }
        }
        unwrapPhaseX=cv::Mat::zeros(imageRows,imageCols,CV_64FC1);
        unwrapPhaseY=cv::Mat::zeros(imageRows,imageCols,CV_64FC1);
        pattern_class->decode_doublefrequencyHeterodyne(maskImage,wrapPhaseX,90,95,unwrapPhaseX,Vertical);
        pattern_class->decode_doublefrequencyHeterodyne(maskImage,wrapPhaseY,80,85,unwrapPhaseY,Horizontal);

        paint_pattern.insert(paint_pattern.end(),pattern_class->image_paint.begin(),pattern_class->image_paint.end());

        cv::Mat wrapx,wrapy;
        cv::normalize(unwrapPhaseX,wrapx,1.0,0.0,cv::NORM_MINMAX);
        cv::normalize(unwrapPhaseY,wrapy,1.0,0.0,cv::NORM_MINMAX);
        wrapx.convertTo(wrapx,CV_8UC1,255,0);
        wrapy.convertTo(wrapy,CV_8UC1,255,0);

        cv::imwrite("../ScanData/1.bmp",wrapx);
        cv::imwrite("../ScanData/2.bmp",wrapy);
}

void Widget::color_double_frequency_reconstruction()
{
    pattern_class->parameter_setting(imageHeight,imageWidth);
    pattern_class->image_paint.clear();

    int stepNum = 3;
    int pixelperperiodf1_V = 80;
    int pixelperperiodf2_V = 85;
    int pixelperperiodf1_H = 70;
    int pixelperperiodf2_H = 75;
    std::vector<cv::Mat> pattern_sequence;
    pattern_sequence.reserve(20);

    pattern_class->generate_color_three_step_sin_pattern(pattern_sequence,pixelperperiodf1_V,pixelperperiodf2_V,stepNum,Vertical,0);
    pattern_class->generate_color_three_step_sin_pattern(pattern_sequence,pixelperperiodf1_V,pixelperperiodf2_V,stepNum,Vertical,myPI/stepNum);
    pattern_class->generate_color_three_step_sin_pattern(pattern_sequence,pixelperperiodf1_H,pixelperperiodf2_H,stepNum,Horizontal,0);
    pattern_class->generate_color_three_step_sin_pattern(pattern_sequence,pixelperperiodf1_H,pixelperperiodf2_H,stepNum,Horizontal,myPI/stepNum);

    textbrowser->append("\nthe number of pattern is" + QString::number(pattern_sequence.size()));
    if(!project_capture_pattern(pattern_sequence))
    {
        textbrowser->append("\nError: Failed to capture the pattern ... ");
        return;
    }

    //simulation
//    mat_list.swap(pattern_sequence);
//    maskImage = cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));

    ///////save the capture pattern
    //saveCapturePattern();

    std::vector<cv::Mat> Rmat_list,Gmat_list,Bmat_list;
    std::cout<<"mat_list size : "<<mat_list.size()<<std::endl;
    for(int n = 0 ; n < mat_list.size(); n++){
        std::vector<cv::Mat> channels;
//            if(n==0){
//                paint_pattern.push_back(mat_list[n]);
//            }
        if(crosstalk_flag){
            pattern_class->correct_crosstalk_by_LookUpTable(crosstalk_table,mat_list[n],mat_list[n],maskImage);
        }
//            if(n==0){
//                paint_pattern.push_back(mat_list[n]);
//            }
        cv::split(mat_list[n],channels);
        Bmat_list.push_back(channels[0]);
        Gmat_list.push_back(channels[1]);
        Rmat_list.push_back(channels[2]);

    }
    int imageRows = Bmat_list[0].rows;
    int imageCols = Bmat_list[0].cols;
    std::vector<cv::Mat> wrapPhaseX,wrapPhaseY;
    for(int i=0; i<Bmat_list.size(); i+=stepNum*2 ){

        std::vector<cv::Mat> imagesf1,imagesf2,imagesf11,imagesf22;
        imagesf1.insert(imagesf1.begin(),Bmat_list.begin()+i,Bmat_list.begin()+i+stepNum);
        imagesf11.insert(imagesf11.begin(),Bmat_list.begin()+i+stepNum,Bmat_list.begin()+i+stepNum*2);
        imagesf2.insert(imagesf2.begin(),Rmat_list.begin()+i,Rmat_list.begin()+i+stepNum);
        imagesf22.insert(imagesf22.begin(),Rmat_list.begin()+i+stepNum,Rmat_list.begin()+i+stepNum*2);

        cv::Mat sinResult_f1 = cv::Mat::zeros(imageRows,imageCols,CV_64FC1);
        cv::Mat sinResult_f11 = cv::Mat::zeros(imageRows,imageCols,CV_64FC1);
        cv::Mat sinResult_f2 = cv::Mat::zeros(imageRows,imageCols,CV_64FC1);
        cv::Mat sinResult_f22 = cv::Mat::zeros(imageRows,imageCols,CV_64FC1);

//        pattern_class->decode_sinusoidal_LookUpTable(maskImage,imagesf1,sinResult_f1,stepNum);
//        pattern_class->decode_sinusoidal_LookUpTable(maskImage,imagesf11,sinResult_f11,stepNum);
//        pattern_class->decode_sinusoidal_LookUpTable(maskImage,imagesf2,sinResult_f2,stepNum);
//        pattern_class->decode_sinusoidal_LookUpTable(maskImage,imagesf22,sinResult_f22,stepNum);

        pattern_class->decode_multi_step_phaseShift(imagesf1,sinResult_f1,stepNum,4,maskImage);
        pattern_class->decode_multi_step_phaseShift(imagesf11,sinResult_f11,stepNum,4,maskImage);
        pattern_class->decode_multi_step_phaseShift(imagesf2,sinResult_f2,stepNum,4,maskImage);
        pattern_class->decode_multi_step_phaseShift(imagesf22,sinResult_f22,stepNum,4,maskImage);

//        paint_pattern.push_back(sinResult_f1);
//        paint_pattern.push_back(sinResult_f11);
//        paint_pattern.push_back(sinResult_f2);
//        paint_pattern.push_back(sinResult_f22);

//        cv::normalize(sinResult,sinResult,1.0,0.0,cv::NORM_MINMAX);
//        cv::normalize(phaseCodeResult,phaseCodeResult,1.0,0.0,cv::NORM_MINMAX);
//        sinResult.convertTo(sinResult,CV_8UC1,255,0);
//        phaseCodeResult.convertTo(phaseCodeResult,CV_8UC1,255,0);
//        cv::imwrite("../ScanData/sinResult"+QString::number(i).toStdString()+".bmp",sinResult);
//        cv::imwrite("../ScanData/phaseCodeResult"+QString::number(i).toStdString()+".bmp",phaseCodeResult);

        if(i==0){
            wrapPhaseX.push_back(sinResult_f1);
            wrapPhaseX.push_back(sinResult_f2);
            wrapPhaseX.push_back(sinResult_f11);
            wrapPhaseX.push_back(sinResult_f22);

        }else{
            wrapPhaseY.push_back(sinResult_f1);
            wrapPhaseY.push_back(sinResult_f2);
            wrapPhaseY.push_back(sinResult_f11);
            wrapPhaseY.push_back(sinResult_f22);
        }
    }
    unwrapPhaseX=cv::Mat::zeros(imageRows,imageCols,CV_64FC1);
    unwrapPhaseY=cv::Mat::zeros(imageRows,imageCols,CV_64FC1);
    pattern_class->decode_doublefrequencyHeterodyne(maskImage,wrapPhaseX,pixelperperiodf1_V,pixelperperiodf2_V,unwrapPhaseX,Vertical);
    pattern_class->decode_doublefrequencyHeterodyne(maskImage,wrapPhaseY,pixelperperiodf1_H,pixelperperiodf2_H,unwrapPhaseY,Horizontal);

    paint_pattern.insert(paint_pattern.end(),pattern_class->image_paint.begin(),pattern_class->image_paint.end());

    cv::Mat wrapx,wrapy;
    cv::normalize(unwrapPhaseX,wrapx,1.0,0.0,cv::NORM_MINMAX);
    cv::normalize(unwrapPhaseY,wrapy,1.0,0.0,cv::NORM_MINMAX);
    wrapx.convertTo(wrapx,CV_8UC1,255,0);
    wrapy.convertTo(wrapy,CV_8UC1,255,0);

    cv::imwrite("../ScanData/1.bmp",wrapx);
    cv::imwrite("../ScanData/2.bmp",wrapy);
}

void Widget::color_three_frequency_reconstruction()
{
    std::vector<cv::Mat> pattern_sequence;
    pattern_class->parameter_setting(imageHeight,imageWidth);
    int stepNum = 4;

    pattern_class->generate_color_three_sinusoidal_pattern(pattern_sequence,stepNum,Vertical);
    pattern_class->generate_color_three_sinusoidal_pattern(pattern_sequence,stepNum,Horizontal);

   /*********************************************************************/

//    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
//    if(!project_capture_pattern(pattern_sequence))
//    {
//        textbrowser->append("\nError : Failed to capture the picture ... ");
//        return;
//    }

    //*******************simulation****************//
    maskImage=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));
    mat_list.swap(pattern_sequence);

//    for(unsigned int n = 0; n < mat_list.size(); n++){

//        cv::imwrite("../ScanData/measure_image/"+QString::number(n).toStdString()+".bmp",mat_list[n]);
//    }

    std::vector<cv::Mat> P1mat_list,P2mat_list,P3mat_list;
    for(int n=0;n<mat_list.size();n++)
    {
        if(crosstalk_flag){
            pattern_class->correct_crosstalk_by_LookUpTable(crosstalk_table,mat_list[n],mat_list[n],maskImage);
        }
        std::vector<cv::Mat> mat;
        cv::split(mat_list[n],mat);
        P1mat_list.push_back(mat[0]);
        P2mat_list.push_back(mat[1]);
        P3mat_list.push_back(mat[2]);
//        cv::imwrite("../ScanData/measure_image/"+QString::number(n).toStdString()+"_p1.bmp",mat[0]);
//        cv::imwrite("../ScanData/measure_image/"+QString::number(n).toStdString()+"_p2.bmp",mat[1]);
//        cv::imwrite("../ScanData/measure_image/"+QString::number(n).toStdString()+"_p3.bmp",mat[2]);
    }

    std::vector<cv::Mat> wrapPhaseX,wrapPhaseY;

    for(int i= 0;i< P1mat_list.size();i+= stepNum)
    {

        cv::Mat P1result=cv::Mat::zeros(P1mat_list[0].rows,P1mat_list[0].cols,CV_64FC1);
        cv::Mat P2result=cv::Mat::zeros(P1mat_list[0].rows,P1mat_list[0].cols,CV_64FC1);
        cv::Mat P3result=cv::Mat::zeros(P1mat_list[0].rows,P1mat_list[0].cols,CV_64FC1);

        std::vector<cv::Mat> P1_list,P2_list,P3_list;

        P1_list.insert(P1_list.begin(),P1mat_list.begin()+i,P1mat_list.begin()+i+stepNum);
        P2_list.insert(P2_list.begin(),P2mat_list.begin()+i,P2mat_list.begin()+i+stepNum);
        P3_list.insert(P3_list.begin(),P3mat_list.begin()+i,P3mat_list.begin()+i+stepNum);

        pattern_class->decode_sinusoidal_LookUpTable(maskImage,P1_list,P1result,stepNum);
        pattern_class->decode_sinusoidal_LookUpTable(maskImage,P2_list,P2result,stepNum);
        pattern_class->decode_sinusoidal_LookUpTable(maskImage,P3_list,P3result,stepNum);

        if(i==0){
            wrapPhaseX.push_back(P1result);
            wrapPhaseX.push_back(P2result);
            wrapPhaseX.push_back(P3result);
        }else{
            wrapPhaseY.push_back(P1result);
            wrapPhaseY.push_back(P2result);
            wrapPhaseY.push_back(P3result);
        }

    }

    cv::Mat unwrapMap_x = cv::Mat::zeros(wrapPhaseX[0].rows,wrapPhaseX[0].cols,CV_64FC1);
    cv::Mat unwrapMap_y = cv::Mat::zeros(wrapPhaseX[0].rows,wrapPhaseX[0].cols,CV_64FC1);

    pattern_class->decode_threefrequency(maskImage,wrapPhaseX,unwrapMap_x,stepNum);
    pattern_class->decode_threefrequency(maskImage,wrapPhaseY,unwrapMap_y,stepNum);

    paint_pattern.push_back(unwrapMap_x);
    paint_pattern.push_back(unwrapMap_y);

    unwrapPhaseX = unwrapMap_x.clone();
    unwrapPhaseY = unwrapMap_y.clone();

    cv::Mat wrapx,wrapy;
    cv::normalize(unwrapPhaseX,wrapx,1.0,0.0,cv::NORM_MINMAX);
    cv::normalize(unwrapPhaseY,wrapy,1.0,0.0,cv::NORM_MINMAX);
    wrapx.convertTo(wrapx,CV_8UC1,255,0);
    wrapy.convertTo(wrapy,CV_8UC1,255,0);

    cv::imwrite("../ScanData/1.bmp",wrapx);
    cv::imwrite("../ScanData/2.bmp",wrapy);
}

void Widget::on_other_button_slot()
{
    QTime time1;
    time1.start();

    /////////gammma compensation
    if(!gamma_flag)
        gamma_compensation();
    else{
        std::cout<<"gamma was compensated successfully ... \n";
    }

   // std::cout<<"time consumed : "<<time1.elapsed()<<" ms"<<std::endl;

}

void Widget::gamma_compensation()
{
    QTime t1;
    t1.start();

    pattern_class->parameter_setting(imageHeight,imageWidth);
    std::vector< cv::Mat > pattern_sequence;

///////generate_phaseCode_sin_pattern
//    pattern_class->generate_phaseCode_sin_pattern(pattern_sequence,40,Vertical,4);
//    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,40,16,Blue,Vertical);
//    pattern_class->generate_pure_phaseCode_pattern(pattern_sequence,40,16,Vertical);

///////generate_phaseCode_grayCode_sin_pattern
//    pattern_class->generate_phaseCode_grayCode_sin_pattern(pattern_sequence,40,Vertical,4);
//    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,40,16,Blue,Vertical);
//    pattern_class->generate_pure_phaseCode_pattern(pattern_sequence,40,16,Vertical);

///////generate_grayCode_sin_pattern
//    pattern_class->generate_binary_grayCode_sin_pattern(pattern_sequence,80,Vertical,4);
//    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,80,16,Blue,Vertical);

    //渐变纯蓝图案
    pattern_class->generate_gradual_change_gray_pattern(pattern_sequence,Blue);

    ///////generate_sin_pattern
    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,400,3,Blue,Vertical);
    pattern_class->generate_pure_sinusoidal_pattern(pattern_sequence,400,15,Blue,Vertical);

    /******************************capture********************************/
    if(maskImage.empty())
    {
        std::cout<<"the maskImage is empty ... "<<std::endl;
        return ;
    }
    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
    if(!project_capture_pattern(pattern_sequence) || mat_list.empty())
    {
        std::cout<<"Error : Failed to get the picture ... "<<std::endl;
        return;
    }

    /******************************simulation********************************/
//    maskImage=cv::Mat(imageHeight,imageWidth,CV_8UC1,cv::Scalar(255));
//    mat_list.clear();
//    mat_list.swap(pattern_sequence);

    std::vector<cv::Mat> pattern_list;
    std::vector<cv::Mat> phase_list;

    for(int n= 26; n<mat_list.size(); n++)
    {
        std::vector<cv::Mat> channels;
        cv::split(mat_list[n],channels);
        pattern_list.push_back(channels[0]);
    }

    std::vector<cv::Mat> images4,images16;
    images4.insert(images4.begin(),pattern_list.begin(),pattern_list.begin()+3);
    images16.insert(images16.begin(),pattern_list.begin()+3,pattern_list.begin()+18);

    cv::Mat result16= cv::Mat::zeros(images4[0].rows,images4[0].cols,CV_64FC1);;
    cv::Mat result4 = cv::Mat::zeros(images4[0].rows,images4[0].cols,CV_64FC1);
    pattern_class->decode_sinusoidal_LookUpTable(maskImage,images4,result4,3);
    pattern_class->decode_multi_step_phaseShift(images16,result16,15,2,maskImage);
    paint_pattern.push_back(result4);
    paint_pattern.push_back(result16);

//    //the actual and the real
//    phase_list.push_back(result4);
//    phase_list.push_back(result16);

//    //paint
//    paint_pattern.clear();
//    paint_pattern.insert(paint_pattern.end(),phase_list.begin(),phase_list.end());

////    ////active compensation ,to calibrate the gamma value

////    std::vector<double> difference;

////    gammaValue = pattern_class->gamma_active_calibration(phase_list,difference,500);
////    std::cout<<"the gamma value is "<<gammaValue<<std::endl;
////    std::cout<<"the difference size is "<<difference.size()<<std::endl;
////    //value_paint.push_back(difference);
////    gamma_flag=true;

//    /*************************************************************/

//    ////create the look up table
//    hash_table phaseErrorTable_sin,phaseErrorTable_pCode;
//    phaseErrorTable_sin=pattern_class->create_LookUpTable(sin_list,maskImage);
//    phaseErrorTable_pCode=pattern_class->create_LookUpTable(phaseCode_list,maskImage);

//    std::vector<pointXY> tempPointBefore;
//    tempPointBefore.swap(pattern_class->point_paint);
//    std::cout<<"the paintPoint size : "<<tempPointBefore.size()<<std::endl;
//    pointXY_paint.push_back(tempPointBefore);

//    std::vector<pointXY> tempPointAfter;
//    pointXY temp;
//    for(unsigned int j=0;j<256;++j)
//    {
//        double tempPhase = j*2*myPI/256;
//        temp.x = tempPhase;
//        temp.y = phaseErrorTable_pCode.find(j);
//        tempPointAfter.push_back(temp);
//    }
//    pointXY_paint.push_back(tempPointAfter);


////    value_paint.clear();
////    value_paint.insert(value_paint.end(),pattern_class->value_paint.begin(),pattern_class->value_paint.end());

//    pattern_class->hash_table_set(phaseErrorTable_sin,1);
//    pattern_class->hash_table_set(phaseErrorTable_pCode,2);

//    //校正
//    cv::Mat result;
//    pattern_class->correct_phase(phaseCode_list[0],result,maskImage,2);
//    paint_pattern.push_back(result);

//    std::vector<double> beforeDifference,afterDifference;

    //    pattern_class->difference_phase(phaseCode_list[0],phaseCode_list[1],500,beforeDifference);
    //    pattern_class->difference_phase(result,phaseCode_list[1],500,afterDifference);

    //    value_paint.clear();
    //    value_paint.push_back(beforeDifference);
    //    value_paint.push_back(afterDifference);



    //crosstalk
    std::vector<double> gray_meansBlue,gray_realBlue;
    gray_meansBlue.reserve(mat_list.size()+5);
    gray_realBlue.reserve(mat_list.size()+5);

    std::vector<cv::Mat> blue_matlist;
    blue_matlist.reserve(mat_list.size()+5);

    for(int n = 0;n < 26;n++)
    {
        std::vector<cv::Mat> channels;
        if(crosstalk_flag)
        {
            pattern_class->correct_crosstalk_by_LookUpTable(crosstalk_table,mat_list[n],mat_list[n],maskImage);
            //pattern_class->color_correction(maskImage,mat_list[n],mat_list[n],crosstalk_mat,reflect_mat);
        }
        cv::split(mat_list[n],channels);
        blue_matlist.push_back(channels[0]);

    }
    pattern_class->meanValue_gradual_gray(blue_matlist,gray_meansBlue);

    for(unsigned int n=0;n<256;n+=10)
    {
        gray_realBlue.push_back(n);
    }
    //七阶多项式拟合
    if( !pattern_class->multipleLinearRegression( gray_meansBlue,gray_realBlue, 7 , blue_gamma_Aconst,blue_gamma_Bcoefficient))
    {
        std::cout<<"\nError: Failed to finish the Linear Regression ...\n";
    }

    pattern_class->build_gamma_Table( blue_gamma_Aconst,blue_gamma_Bcoefficient,7 );
    //绘图
    //拟合前
    std::vector<pointXY> tempPointBlue;
    for(unsigned int j=0;j<gray_realBlue.size();j++)
    {
        pointXY tempBlue;
        tempBlue.x = gray_realBlue[j];
        tempBlue.y = gray_meansBlue[j];
        tempPointBlue.push_back(tempBlue);
        //输出
        textbrowser->append(QString::number(tempBlue.x)+" "+QString::number(tempBlue.y));
    }
    pointXY_paint.push_back(tempPointBlue);

    textbrowser->append(" ********** ");
    textbrowser->append(" ********** ");
    textbrowser->append(" ********** ");

    //拟合后
    double blueGrayValue = 0,blueRealValue = 0;

    std::vector<pointXY> tempPointBlue1;
    for(unsigned int n=0;n<gray_realBlue.size();n++)
    {
        pointXY tempBlue;
        blueRealValue = gray_realBlue[n];
        blueGrayValue = pattern_class->gammaTable[(int)blueRealValue];

        tempBlue.x = blueRealValue;
        tempBlue.y = blueGrayValue;
        tempPointBlue1.push_back(tempBlue);
        //输出
        textbrowser->append(QString::number(tempBlue.x)+" "+QString::number(tempBlue.y));
    }
    pointXY_paint.push_back(tempPointBlue1);

    //测试效果
    pattern_sequence.clear();
    pattern_class->generate_gamma_pure_sinusoidal_pattern(pattern_sequence,400,3,Blue,Vertical,blue_gamma_Aconst,blue_gamma_Bcoefficient);

    //拍摄
    textbrowser->append("\nthe number of pattern is "+QString::number(pattern_sequence.size()));
    if(!project_capture_pattern(pattern_sequence) || mat_list.empty())
    {
        std::cout<<"Error : Failed to get the picture ... "<<std::endl;
        return;
    }
    //仿真
    //mat_list.swap(pattern_sequence);

    std::vector<cv::Mat> Bmat_list;
    for(int n=0;n<mat_list.size();n++)
    {
        std::vector<cv::Mat> Bmat;
        cv::split(mat_list[n],Bmat);
        Bmat_list.push_back(Bmat[0]);
    }
    cv::Mat result=cv::Mat::zeros(Bmat_list[0].rows,Bmat_list[0].cols,CV_64FC1);
    pattern_class->decode_sinusoidal_LookUpTable(maskImage,Bmat_list,result,3);
    paint_pattern.push_back(result);


    std::vector< double > beforeDifference;
    std::vector< double > afterDifference;
    beforeDifference.reserve(2000);
    afterDifference.reserve(2000);

    pattern_class->difference_phase(result4,result16,700,beforeDifference,maskImage);
    pattern_class->difference_phase(result,result16,700,afterDifference,maskImage);

    for(unsigned int n = 0; n < beforeDifference.size(); ++n){
        textbrowser->append(QString::number(beforeDifference[n]));
    }

    textbrowser->append(" after compensation ");
    textbrowser->append(" after compensation ");
    textbrowser->append(" after compensation ");

    for(unsigned int n = 0; n < beforeDifference.size(); ++n){
        textbrowser->append(QString::number(afterDifference[n]));
    }

    std::cout<<"beforeDifference size: "<<beforeDifference.size()<<std::endl;
    std::cout<<"afterDifference size: "<<afterDifference.size()<<std::endl;
    for(int i = 0;i < beforeDifference.size();i+=50){
        std::cout<<beforeDifference[i]<<" , "<<afterDifference[i]<<std::endl;
    }

    value_paint.clear();
    value_paint.push_back(beforeDifference);
    value_paint.push_back(afterDifference);

    phaseErrorTable_flag=true;

    std::cout<<"the time of gamma_compensation is "<<t1.elapsed()<<" ms \n";

}
