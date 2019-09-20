#include "capturethread.h"

CaptureThread::CaptureThread()
{
    connectFlag= false;
}


void CaptureThread::run()
{
    captureImage();
}


void CaptureThread::startCameraCapture()
{
    if(!connectFlag)
    {
            connectCamera();
    }


    capture_flag= true;

    // Check that camera is NOT null
//     if(!camera) return;

     // Check if camera is connected
     if(!camera.isConnected()){
         dlp::CmdLine::Print("Camera NOT connected! \n");
         return;
     }


     // Get the camera frame rate (This assumes the camera triggers the projector!)
     float frame_rate;
     camera.GetFrameRate(&frame_rate);


     dlp::Image          camera_frame;
     dlp::Image::Window  camera_view;


     if(camera.Start().hasErrors()){
         dlp::CmdLine::Print("Could NOT start camera! \n");
         return;
     }

     camera.ControlStrobe(1);

     int num= 0;

        while(capture_flag){
            camera_frame.Clear();               // Clear the image object
            camera.GetFrame(&camera_frame);    // Grab the latest camera frame

            cv::Mat show_mat;
            camera_frame.GetOpenCVData(&show_mat);


            /*************************************************************************/

            QImage image;

            if(show_mat.channels()==3)
                {
                    //cvt Mat BGR 2 QImage RGB
                    cvtColor(show_mat,show_mat,CV_BGR2RGB);
                    image =QImage((const unsigned char*)(show_mat.data),
                                show_mat.cols,show_mat.rows,
                                show_mat.cols*show_mat.channels(),
                                QImage::Format_RGB888);
                }
                else
                {
                    image =QImage((const unsigned char*)(show_mat.data),
                                show_mat.cols,show_mat.rows,
                                show_mat.cols*show_mat.channels(),
                                QImage::Format_Grayscale8);

                }

            send_capture_mat(image);

            //qDebug()<<"i: "<<num++;

            /*************************************************************************/


            QTime dieTime = QTime::currentTime().addMSecs(100);
            while( QTime::currentTime() <dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);



        }


        camera.Stop();
//        camera.Disconnect();
}

void CaptureThread::captureImage()
{
    capture_flag= true;

    // Check that camera is NOT null
     if(!camera_point) return;

     // Check if camera is connected
     if(!camera_point->isConnected()){
         dlp::CmdLine::Print("Camera NOT connected! \n");
         return;

     }


     // Get the camera frame rate (This assumes the camera triggers the projector!)
     float frame_rate;
     camera_point->GetFrameRate(&frame_rate);


     dlp::Image          camera_frame;
     dlp::Image::Window  camera_view;


     if(camera_point->Start().hasErrors()){
         dlp::CmdLine::Print("Could NOT start camera! \n");
         return;
     }

     dlp::Time::Sleep::Microseconds(100000);
//     camera_point->ControlStrobe(1);

//     camera_frame.Clear();               // Clear the image object
//     camera_point->GetFrame(&camera_frame);    // Grab the latest camera frame

     int num= 0;

        while(capture_flag){

            camera_frame.Clear();               // Clear the image object
            camera_point->GetFrame(&camera_frame);    // Grab the latest camera frame

            cv::Mat show_mat;
            //camera_frame.ConvertToRGB();
            camera_frame.GetOpenCVData(&show_mat);


            /*************************************************************************/

            QImage image;


            if(show_mat.channels()==3)
                {
                    //cvt Mat BGR 2 QImage RGB
                    image =QImage((const unsigned char*)(show_mat.data),
                                show_mat.cols,show_mat.rows,
                                show_mat.cols*show_mat.channels(),
                                QImage::Format_RGB888);
                   // std::cout<<"the channel of QImage is 3 ..."<<std::endl;
                }
                else
                {
                    image =QImage((const unsigned char*)(show_mat.data),
                                show_mat.cols,show_mat.rows,
                                show_mat.cols*show_mat.channels(),
                                QImage::Format_Grayscale8);
                    //std::cout<<"the channel of QImage is 1 ..."<<std::endl;
                }

//            cv::cvtColor(show_mat,show_mat,CV_BGR2RGB);

//            cv::imshow("pattern",show_mat);
//            cv::waitKey(500);

            emit send_capture_mat(image);

//            qDebug()<<"i: "<<num++;

            /*************************************************************************/

            dlp::Time::Sleep::Microseconds(50000);

//            QTime dieTime = QTime::currentTime().addMSecs(100);
//            while( QTime::currentTime() <dieTime )
//            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);



        }


        camera_point->Stop();


        dlp::Time::Sleep::Microseconds(100000);
}

void CaptureThread::setCaptureFlag(bool flag)
{
    capture_flag= flag;
}


void CaptureThread::setCamera(dlp::PG_FlyCap2_C* camera)
{
    camera_point= camera;
}

void CaptureThread::connectCamera()
{
    DLP_NEW_PARAMETERS_ENTRY(ConnectIdProjector,           "CONNECT_ID_PROJECTOR",  std::string, "0");
    DLP_NEW_PARAMETERS_ENTRY(ConnectIdCamera,              "CONNECT_ID_CAMERA",     std::string, "0");

    DLP_NEW_PARAMETERS_ENTRY(ConfigFileProjector,           "CONFIG_FILE_PROJECTOR",                std::string, "config/config_projector.txt");
    DLP_NEW_PARAMETERS_ENTRY(ConfigFileCamera,              "CONFIG_FILE_CAMERA",                   std::string, "config/config_camera.txt");

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
//    ret = dlp::DLP_Platform::ConnectSetup(projector_4500,connect_id_projector.Get(),config_file_projector.Get(),true);
//    if(ret.hasErrors()) {
//        dlp::CmdLine::Print("\n\nPlease resolve the LightCrafter connection issue before proceeding to next step...\n");
//    }

    ret = dlp::Camera::ConnectSetup(camera,connect_id_camera.Get(),config_file_camera.Get(),true);
    if(ret.hasErrors()) {
        dlp::CmdLine::Print("\n\nPlease resolve the camera connection issue before proceeding to next step...\n");
    }else{
        connectFlag= true;
    }
}

void CaptureThread::capture_continuous_Image()
{
    connectCamera();

    capture_flag= true;

    // Check that camera is NOT null
     if(!camera_point) return;

     // Check if camera is connected
     if(!camera_point->isConnected()){
         dlp::CmdLine::Print("Camera NOT connected! \n");
         return;
     }


     // Get the camera frame rate (This assumes the camera triggers the projector!)
     float frame_rate;
     camera_point->GetFrameRate(&frame_rate);


     dlp::Image          camera_frame;
     dlp::Image::Window  camera_view;


     if(camera_point->Start().hasErrors()){
         dlp::CmdLine::Print("Could NOT start camera! \n");
         return;
     }

     dlp::Time::Sleep::Microseconds(100000);
     camera_point->ControlStrobe(1);

     camera_frame.Clear();               // Clear the image object
     camera_point->GetFrame(&camera_frame);    // Grab the latest camera frame

     int num= 0;

        while(capture_flag){
            camera_frame.Clear();               // Clear the image object
            camera_point->GetFrame(&camera_frame);    // Grab the latest camera frame

            cv::Mat show_mat;
            camera_frame.GetOpenCVData(&show_mat);


            /*************************************************************************/

            QImage image;


            image =QImage((const unsigned char*)(show_mat.data),
                          show_mat.cols,show_mat.rows,
                          show_mat.cols*show_mat.channels(),
                          QImage::Format_Grayscale8);


            send_capture_mat(image);

//            qDebug()<<"i: "<<num++;

            /*************************************************************************/

            dlp::Time::Sleep::Microseconds(50000);

//            QTime dieTime = QTime::currentTime().addMSecs(100);
//            while( QTime::currentTime() <dieTime )
//            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);



        }


//        camera_point->Stop();


//        dlp::Time::Sleep::Microseconds(100000);
}
