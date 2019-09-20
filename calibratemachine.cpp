#include "calibratemachine.h"
#include "stdio.h"
#include "time.h"
CalibrateMachine::CalibrateMachine()
{
        objectPoints.clear();
        imagePoints.clear();

        boardSize.width = 11;
        boardSize.height = 13;
}

CalibrateMachine::~CalibrateMachine()
{

}


//输出找圆心的情况
int CalibrateMachine::addCirclebordPoints(QStringList filelist, cv::Size &boardSize)
{

    std::vector<cv::Point3f> objectCorners;


    for(int i= 0;i< boardSize.height;i++){
        for(int j=0;j<boardSize.width;j++){
            objectCorners.push_back(cv::Point3f(i,j,0.0f));
        }
    }

    cv::Mat image;
    int successes= 0;

    int j= 0;

    qDebug()<<"start";

    for(int i= 0; i< filelist.count(); i++)
    {
        std::vector<cv::Point2f> imageCorners;

        image= cv::imread(filelist.at(i).toStdString(),0);

        bool found= findCircleBoardCorners(image,boardSize,imageCorners);

        if(found)
        {
            j++;

            if(imageCorners.size() == boardSize.area())
            {
//                addPoints(imageCorners,objectCorners);
                successes++;
            }else{

                qDebug()<<"imageCorners.size(): "<<imageCorners.size()<<"  boardSize.area(): "<< boardSize.area();
            }
//            qDebug()<<"i: "<<i<<"successes: "<<successes<<" j: "<<j;
        }

        qDebug()<<"i: "<<i<<"successes: "<<successes;


    }

    return successes;

}

//求dlp对应点
void CalibrateMachine::pointsCameraToDlp(cv::Mat phaseX, cv::Mat phaseY, int period, std::vector<cv::Point2f> cameraPoints, std::vector<Point2f> &dlpPoints)
{
    int image_row= boardSize.height;
    int image_col= boardSize.width;

    for(int i=0; i< image_row*image_col; i++)
    {

            cv::Point2f point= cameraPoints.at(i);

//            int x= 0.5+ point.x;
//            int y= 0.5+ point.y;

//            double xValue= phaseX.at<double>(y,x);
//            double yValue= phaseY.at<double>(y,x);


//            double temp_x= 912*xValue;
//            double temp_y= 570*yValue;

//            double dlp_x_value= temp_x/(CV_PI*std::pow(2.0,period-1));
//            double dlp_y_value= temp_y/(CV_PI*std::pow(2.0,period-1));


            int x=  point.x;
            int y=  point.y;

            double xValue= phaseX.at<double>(y,x);
            double yValue= phaseY.at<double>(y,x);

            double dlp_x_value= xValue*period/(2*CV_PI);
            double dlp_y_value= yValue*period/(2*CV_PI);

            dlpPoints.push_back(cv::Point2f(dlp_x_value,dlp_y_value));

    }
}


//送入的是拍摄的图片序列，解相位，再把相机和dlp的像点、物点坐标传入全局变量
void CalibrateMachine::decodeFourStepMultiplyHierarchical( std::vector<cv::Mat> mat_list)
{
    int flag= mat_list.size()%4;

    if(1 != flag)
    {
        qDebug()<<"Error: Image number is not time of 4!";
        return;
    }

    cv::Mat cameraImage= mat_list.at(0);

    if(1!= cameraImage.channels())
    {
        qDebug()<<"Error: The type of image is not mono!";
        return ;
    }

    std::vector<cv::Point2f> cameraCirclePoints;

    bool found= findCircleBoardCorners(cameraImage,boardSize,cameraCirclePoints);

//    showMat(cameraImage,"img");

    if(!found)
    {
        qDebug()<<"Error: Can not find the circle points!";
        return;
    }

//    cv::Mat show_camera_image= mat_list.at(0);
//    drawChessboardCorners(show_camera_image,boardSize,cameraCirclePoints,found);


      /************************************************************************************************************/


      //周期相位

          PhaseShift PS_machine;
          std::vector<cv::Mat> wrap_list_x;
          std::vector<cv::Mat> wrap_list_y;



          for(int i= 1;i< 1+ (mat_list.size()-1)/2;i+= 4)
          {

              std::vector<cv::Mat> images;
              images.push_back(mat_list.at(i+ 0));
              images.push_back(mat_list.at(i+ 1));
              images.push_back(mat_list.at(i+ 2));
              images.push_back(mat_list.at(i+ 3));

              cv::Mat result;
              PS_machine.fourStepPhaseShift(images,result);

              wrap_list_x.push_back(result);


          }

//          for(int i= 0;i< wrap_list_x.size();i++)
//          {
//              QString name= "x_wrap"+ QString::number(i);
//              cvShowMatFloat(wrap_list_x.at(i),name.toStdString(),0);

//          }

          for(int i= 1+ (mat_list.size()-1)/2; i< mat_list.size() ;i+= 4)
          {

              std::vector<cv::Mat> images;
              images.push_back(mat_list.at(i+ 0));
              images.push_back(mat_list.at(i+ 1));
              images.push_back(mat_list.at(i+ 2));
              images.push_back(mat_list.at(i+ 3));

              cv::Mat result;
              PS_machine.fourStepPhaseShift(images,result);

              wrap_list_y.push_back(result);


          }

//          for(int i= 0;i< wrap_list_y.size();i++)
//          {
//              QString name= "y_wrap"+ QString::number(i);
//              cvShowMatFloat(wrap_list_y.at(i),name.toStdString(),0);

//          }


          cv::Mat unwrapMap_x;
          cv::Mat unwrapMap_y;

          std::vector<cv::Mat> unwrap_Mat_list_x;
          std::vector<cv::Mat> unwrap_Mat_list_y;

          PS_machine.unwarpMultiplyHierarchical(2.0,wrap_list_x,unwrapMap_x,unwrap_Mat_list_x);
          PS_machine.unwarpMultiplyHierarchical(2.0,wrap_list_y,unwrapMap_y,unwrap_Mat_list_y);

//          PS_machine.unwrapMultiplyWavelength(wrap_list_x,unwrapMap_x,unwrap_Mat_list_x);
//          PS_machine.unwrapMultiplyWavelength(wrap_list_y,unwrapMap_y,unwrap_Mat_list_y);

          qDebug()<<"wrap_list_x: "<<wrap_list_x.size();
          qDebug()<<"wrap_list_y: "<<wrap_list_y.size();


//          PS_machine.multipleWavelengthUnwrapFloat(wrap_list_x,unwrapMap_x);
//          PS_machine.multipleWavelengthUnwrapFloat(wrap_list_y,unwrapMap_y);

//          QString name= "x_unwrap";
//          cvShowMatFloat(unwrapMap_x,name.toStdString(),wrap_list_x.size()-1);

//          name= "y_unwrap";
//          cvShowMatFloat(unwrapMap_y,name.toStdString(),wrap_list_x.size()-1);


//          qDebug()<<"unwrap_Mat_list_x num: "<<unwrap_Mat_list_x.size();

//          for(int i= 0;i< unwrap_Mat_list_x.size();i++)
//          {
//              QString name= "x_unwrap"+ QString::number(i);
//              cvShowMatFloat(unwrap_Mat_list_x.at(i),name.toStdString(),i);

//          }

//          for(int i= 0;i< unwrap_Mat_list_y.size();i++)
//          {
//              QString name= "y_unwrap"+ QString::number(i);
//              cvShowMatFloat(unwrap_Mat_list_y.at(i),name.toStdString(),i);

//          }

          /****************************************************************************************************************************/
           //坐标转换

           std::vector<cv::Point2f> dlpCirclePoints;

           int period= (mat_list.size()-1)/8;

           qDebug()<<"period: "<<period;

       //    phaseToCV_PIxel(unwrapMap_x,unwrapMap_y,period,cameraCirclePoints,dlpCirclePoints);

           pointsCameraToDlp(unwrapMap_x,unwrapMap_y,period,cameraCirclePoints,dlpCirclePoints);




       //    qDebug()<<"num: "<<dlpCirclePoints.size();


       //    cvShowUnwrapMap(unwrapMap_x,period,"unwrapMap_x");

       //    cvShowUnwrapMap(unwrapMap_y,period,"unwrapMap_y");


//           cvShowMatFloat(unwrapMap_x,"unwrapMap_x",period-1);
//           cvShowMatFloat(unwrapMap_y,"unwrapMap_y",period-1);

       //    cvShowUnwrapMapFloat(unwrapMap_x,period,"unwrapMap_x");

       //    cvShowUnwrapMapFloat(unwrapMap_y,period,"unwrapMap_y");

       //    pointsToTxt(cameraCirclePoints,"cameraCirclePoints");

       //    pointsToTxt(dlpCirclePoints,"dlpCirclePoints");



           /****************************************************************************************************************************/

           //添加点

           std::vector<cv::Point3f> objectCorners;


           for(int i= 0;i< boardSize.height;i++){
               for(int j=0;j<boardSize.width;j++){
                   objectCorners.push_back(cv::Point3f(2.3*i,2.3*j,0.0f));
               }
           }


           addPoints(cameraCirclePoints,dlpCirclePoints,objectCorners);










}


//标定并保存
double CalibrateMachine::structuredLightStereoCalibrate()
{
    //标定
    mustInitUndistort= true;


    cv::Mat left_intrinsic, right_intrinsic;
    cv::Mat left_distortion, right_distortion;

    std::vector<cv::Mat> left_rvecs,left_tvecs;
    std::vector<cv::Mat> right_rvecs,right_tvecs;

    cv::Mat _R,  _T,  _E,  _F;

//    double cameraError= cv::calibrateCamera(objectPoints,
//                               cameraPoints,
//                               boardSize,
//                               left_intrinsic,
//                               left_distortion,
//                               left_rvecs,left_tvecs,
//                               flag,cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 50, DBL_EPSILON));

    cv::Size cameraSize = cv::Size(1928,1448);
    double cameraError= cv::calibrateCamera(objectPoints,
                               cameraPoints,
                               cameraSize,
                               left_intrinsic,
                               left_distortion,
                               left_rvecs,left_tvecs);

    qDebug()<<"\nthe num of groups : "<<cameraPoints.size();

    qDebug()<<"cameraError: "<<cameraError;

//    double dlpError= cv::calibrateCamera(objectPoints,
//                               dlpPoints,
//                               boardSize,
//                               right_intrinsic,
//                               right_distortion,
//                               right_rvecs,right_tvecs,
//                               flag,cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 50, DBL_EPSILON));

    double dlpError= cv::calibrateCamera(objectPoints,
                               dlpPoints,
                               cameraSize,
                               right_intrinsic,
                               right_distortion,
                               right_rvecs,right_tvecs);

     qDebug()<<"dlpError: "<<dlpError;

//     createReprojectErrorTable(cameraPoints, objectPoints,"camera",left_rvecs,left_tvecs,left_intrinsic,left_distortion);
//     createReprojectErrorTable(dlpPoints, objectPoints,"dlp",right_rvecs,right_tvecs,right_intrinsic,right_distortion);

     double stereoError =cv::stereoCalibrate(objectPoints, cameraPoints, dlpPoints, left_intrinsic, left_distortion,
            right_intrinsic, right_distortion, cameraSize, _R, _T, _E, _F,
                                             cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 500, DBL_EPSILON)
                                             ,/*cv::CALIB_FIX_INTRINSIC*/ cv::CALIB_USE_INTRINSIC_GUESS /*+ cal_flags*/);

     qDebug()<<"stereoError: "<<stereoError;

     CalibrationEvaluate("camera",cameraPoints,left_rvecs,left_tvecs,left_intrinsic,left_distortion);
     CalibrationEvaluate("projector",dlpPoints,right_rvecs,right_tvecs,right_intrinsic,right_distortion);
     /******************************************************************************************************************/


     cv::Mat E_1= cv::Mat::eye(3,4,CV_64FC1);
     cv::Mat E_2= cv::Mat::zeros(3,4,CV_64FC1);

     cv::Mat M_1= cv::Mat::zeros(3,4,CV_64FC1);
     cv::Mat M_2= cv::Mat::zeros(3,4,CV_64FC1);

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

     saveCalibrateResult(cameraError,left_intrinsic,left_distortion,dlpError,right_intrinsic,right_distortion,stereoError,_R,_T);

//     std::cout<<"E_1";
//     std::cout<<"\n";
//     std::cout<<E_1;
//     std::cout<<"\n";

//     std::cout<<"E_2";
//     std::cout<<"\n";
//     std::cout<<E_2;
//     std::cout<<"\n";

//     std::cout<<"M_1";
//     std::cout<<"\n";
//     std::cout<<M_1;
//     std::cout<<"\n";

//     std::cout<<"M_2";
//     std::cout<<"\n";
//     std::cout<<M_2;
//     std::cout<<"\n";

//     std::cout<<"left_intrinsic";
//     std::cout<<"\n";
//     std::cout<<left_intrinsic;
//     std::cout<<"\n";

//     std::cout<<"left_distortion";
//     std::cout<<"\n";
//     std::cout<<left_distortion;
//     std::cout<<"\n";


//     std::cout<<"right_intrinsic";
//     std::cout<<"\n";
//     std::cout<<right_intrinsic;
//     std::cout<<"\n";

//     std::cout<<"right_distortion";
//     std::cout<<"\n";
//     std::cout<<right_distortion;
//     std::cout<<"\n";

//     std::cout<<"_R";
//     std::cout<<"\n";
//     std::cout<<_R;
//     std::cout<<"\n";

//     std::cout<<"_T";
//     std::cout<<"\n";
//     std::cout<<_T;
//     std::cout<<"\n";

//     std::cout<<_E;
//     std::cout<<"\n";

//     std::cout<<_F;
//     std::cout<<"\n";

//     std::cout.flush();


     /********************************************************************************************************************/



//    createCalibrateTxt();


//    readCalibPara();


     return stereoError;
}

void CalibrateMachine::printMat(cv::Mat mat)
{
    int row= mat.rows;
    int col= mat.cols;

    qDebug()<<"[";
    for(int i=0;i< row;i++)
    {
        QString showRow="";
        for(int j=0;j<col;j++)
        {
            showRow += QString::number(mat.at<uchar>(i,j));
            showRow +=",";
        }
        qDebug()<<showRow;
    }
    qDebug()<<"]";
}



void CalibrateMachine::saveCalibrateResult(double cameraError,cv::Mat cameraM,cv::Mat cameraKc,double proError,cv::Mat proM,cv::Mat proKc,
                         double stereoError,cv::Mat R,cv::Mat T)
{


//    std::ofstream result_file;
//    result_file.open("../ScanData/calibResult.txt");


//    result_file<<"Camera Calib:"<<"\n";
//    result_file<<"-reprojection error: "<<cameraError<<"\n";
//    result_file<<"-K:"<<"\n";
//    result_file<<cameraM<<"\n";
//    result_file<<"-kc:"<<"\n";
//    result_file<<cameraKc<<"\n"<<"\n";


//    result_file<<"Project Calib:"<<"\n";
//    result_file<<"-reprojection error: "<<proError<<"\n";
//    result_file<<"-K:"<<"\n";
//    result_file<<proM<<"\n";
//    result_file<<"-kc:"<<"\n";
//    result_file<<proKc<<"\n"<<"\n";


//    result_file<<"Stereo Calib:"<<"\n";
//    result_file<<"-reprojection error: "<<stereoError<<"\n";
//    result_file<<"-R:"<<"\n";
//    result_file<<R<<"\n";
//    result_file<<"-T:"<<"\n";
//    result_file<<T<<"\n"<<"\n";


//    result_file.close();

    FileStorage fs("../ScanData/calibResult.yml", FileStorage::WRITE); //填入写操作
    time_t rawtime;
    time(&rawtime);

    fs<<"calibrationDate"<<asctime(localtime(&rawtime));

    fs<<"Camera Calib - reprojection error"<<cameraError;
    fs<<"cameraM"<<cameraM;
    fs<<"cameraKc"<<cameraKc;
    fs<<"Project Calib - reprojection error"<<proError;
    fs<<"proM"<<proM;
    fs<<"proKc"<<proKc;
    fs<<"R"<<R;
    fs<<"T"<<T;
    fs<<"Stereo Calib - reprojection error"<<stereoError;
    fs.release();



}

//找圆心
bool CalibrateMachine::findCircleBoardCorners(cv::Mat image, cv::Size boardSize, std::vector<cv::Point2f> &realCorners)
{

    cv::Mat img= image.clone();


    std::vector<std::vector<cv::Point>> contours;

    for(int c= 0;c< img.cols;c++)
   {
       for(int r= 0;r< img.rows;r++)
       {
           img.at<uchar>(r,c)= 255-img.at<uchar>(r,c);
       }

   }
    cv::medianBlur(img,img,5);

    threshold(img,img,100,255,cv::THRESH_BINARY);


    cv::findContours(img,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);

    cv::Mat findContoursMat(img.size(),CV_8U,cv::Scalar(0));

    cv::drawContours(findContoursMat,contours,-1,cv::Scalar(255),2);

    //移除过长和过短的轮廓
    int cmin= 10;
    int cmax= 300;

     std::vector<std::vector<cv::Point>>:: const_iterator itTemp;

     std::vector<std::vector<cv::Point>>:: const_iterator itl= contours.begin();

     while(itl!= contours.end()){
         if(itl->size()< cmin || itl->size()> cmax){
             itTemp= itl;
             itl= contours.erase(itTemp);
         }
         else
             ++itl;
     }




    cv::Mat meddleContours(img.size(),CV_8U,cv::Scalar(0));

    cv::drawContours(meddleContours,contours,-1,cv::Scalar(255),2);

    std::vector<std::vector<cv::Point>>:: const_iterator it_rect= contours.begin();

    while(it_rect!= contours.end()){
        cv::Rect r0= cv::boundingRect(cv::Mat(*it_rect));

        float scale;

        if(r0.width> r0.height)
        {
            scale= r0.width/ r0.height;
        }else{
            scale= r0.height/ r0.width;
        }
        if(scale> 1.1 || r0.x< 0.1*img.cols || r0.y< 0.1*img.rows || r0.x> 0.9*img.cols ||r0.y> 0.9*img.rows)
        {
            it_rect= contours.erase(it_rect);
        } else
            ++it_rect;
    }


    cv::Mat showContours(img.size(),CV_8U,cv::Scalar(0));

    cv::drawContours(showContours,contours,-1,cv::Scalar(255),2);


    //半径容器、圆心容器
    std::vector<float> radius_v;
    std::vector<cv::Point2f> centers_v;

    cv::Point2f center;
    float radius;

    std::vector<std::vector<cv::Point>>:: const_iterator itsort= contours.begin();

    for(int i= 0;itsort!= contours.end();i++)
    {
        cv::minEnclosingCircle(cv::Mat(contours[i]),center,radius);

        radius_v.push_back(radius);
        centers_v.push_back(center);

            ++itsort;
    }



    //根据半径排序
    for(int i= 0;i< centers_v.size();i++)
    {
        for(int j= i+1;j< centers_v.size();j++)
        {


            if(radius_v.at(i)< radius_v.at(j))
            {

                std::swap(radius_v[i],radius_v[j]);
                std::swap(centers_v[i],centers_v[j]);

            }
        }
    }

    cv::Mat mark(img.size(),CV_8U,cv::Scalar(255));

    std::vector<cv::Point2f> mark_point;

    for(int i= 0; i<5; i++)
    {
        mark_point.push_back(centers_v[i]);
        circle(mark,centers_v[i],radius_v[i],cv::Scalar(0),3);
    }


    std::vector<float> distance;

    for(int i=0; i<5; i++)
    {
        for(int j=i+1;j<5;j++)
        {
            float d= sqrt(pow(centers_v[i].x-centers_v[j].x,2) + pow(centers_v[i].y-centers_v[j].y,2));
            distance.push_back(d);
        }
    }


    float maxD=0;
    int maxN;

    //找出最大值
    for(int i=0;i<10;i++)
    {
        if(maxD< distance.at(i))
        {
            maxD= distance.at(i);
            maxN= i;
        }

    }


    float minD=maxD;
    int minN;

    //找出最小值
    for(int i=0;i<10;i++)
    {
        if(minD> distance.at(i))
        {
            minD= distance.at(i);
            minN= i;
        }

    }


    Point2f longP_1,longP_2;
    Point2f shortP_1,shortP_2;
    Point2f otherP,nearP,farP;


    int flag= 10;


    switch(maxN)
    {
        case 0:
        {
            longP_1= centers_v.at(0);
            longP_2= centers_v.at(1);

            flag -=0;
            flag -=1;

        }

            break;
        case 1:
        {
            longP_1= centers_v.at(0);
            longP_2= centers_v.at(2);

            flag -=0;
            flag -=2;
        }

            break;
        case 2:
        {
            longP_1= centers_v.at(0);
            longP_2= centers_v.at(3);

            flag -=0;
            flag -=3;
        }

            break;
        case 3:
        {
            longP_1= centers_v.at(0);
            longP_2= centers_v.at(4);

            flag -=0;
            flag -=4;
        }

            break;

        case 4:
        {
            longP_1= centers_v.at(1);
            longP_2= centers_v.at(2);

            flag -=1;
            flag -=2;
        }

            break;
        case 5:
        {
            longP_1= centers_v.at(1);
            longP_2= centers_v.at(3);

            flag -=1;
            flag -=3;
        }

            break;
        case 6:
        {
            longP_1= centers_v.at(1);
            longP_2= centers_v.at(4);

            flag -=1;
            flag -=4;
        }

            break;
        case 7:
        {
            longP_1= centers_v.at(2);
            longP_2= centers_v.at(3);

            flag -=2;
            flag -=3;
        }

            break;

        case 8:
        {
            longP_1= centers_v.at(2);
            longP_2= centers_v.at(4);

            flag -=2;
            flag -=4;
        }

            break;
        case 9:
        {
            longP_1= centers_v.at(3);
            longP_2= centers_v.at(4);

            flag -=3;
            flag -=4;
        }

            break;
    }

    switch(minN)
    {
        case 0:
        {
            shortP_1= centers_v.at(0);
            shortP_2= centers_v.at(1);

            flag -=0;
            flag -=1;

        }

            break;
        case 1:
        {
            shortP_1= centers_v.at(0);
            shortP_2= centers_v.at(2);

            flag -=0;
            flag -=2;
        }

            break;
        case 2:
        {
            shortP_1= centers_v.at(0);
            shortP_2= centers_v.at(3);

            flag -=0;
            flag -=3;
        }

            break;
        case 3:
        {
            shortP_1= centers_v.at(0);
            shortP_2= centers_v.at(4);

            flag -=0;
            flag -=4;
        }

            break;

        case 4:
        {
            shortP_1= centers_v.at(1);
            shortP_2= centers_v.at(2);

            flag -=1;
            flag -=2;
        }

            break;
        case 5:
        {
            shortP_1= centers_v.at(1);
            shortP_2= centers_v.at(3);

            flag -=1;
            flag -=3;
        }

            break;
        case 6:
        {
            shortP_1= centers_v.at(1);
            shortP_2= centers_v.at(4);

            flag -=1;
            flag -=4;
        }

            break;
        case 7:
        {
            shortP_1= centers_v.at(2);
            shortP_2= centers_v.at(3);

            flag -=2;
            flag -=3;
        }

            break;

        case 8:
        {
            shortP_1= centers_v.at(2);
            shortP_2= centers_v.at(4);

            flag -=2;
            flag -=4;
        }

            break;
        case 9:
        {
            shortP_1= centers_v.at(3);
            shortP_2= centers_v.at(4);

            flag -=3;
            flag -=4;
        }

            break;
    }


    otherP= centers_v.at(flag);
    farP= otherP;


    float d_1= abs(sqrt(pow(longP_1.x-shortP_1.x,2) + pow(longP_1.y-shortP_1.y,2))-
                   sqrt(pow(longP_2.x-shortP_1.x,2) + pow(longP_2.y-shortP_1.y,2)));

    float d_2= abs(sqrt(pow(longP_1.x-shortP_2.x,2) + pow(longP_1.y-shortP_2.y,2))-
                   sqrt(pow(longP_2.x-shortP_2.x,2) + pow(longP_2.y-shortP_2.y,2)));


    Point2f temp= otherP;

    if(d_1> d_2)
    {
        otherP= shortP_1;
        shortP_1= temp;

        nearP= shortP_2;
    }else{
        otherP= shortP_2;
        shortP_2= temp;

        nearP= shortP_1;
    }


    Point2f srcTriangle[3];
    Point2f dstTriangle[3];

    Mat tosrcWarpMat(2,3,CV_32FC1);
    Mat todstWarpMat(2,3,CV_32FC1);

    Mat dstImage= Mat::zeros(img.rows,img.cols,img.type());

//    Mat testImage= Mat::zeros(img.rows,img.cols,img.type());
    cv::Mat testImage(img.size(),CV_8U,cv::Scalar(255));

    circle(testImage,farP,radius_v[0],cv::Scalar(0),2);
    circle(testImage,nearP,radius_v[0],cv::Scalar(0),4);
    circle(testImage,otherP,radius_v[0],cv::Scalar(0),6);


    srcTriangle[0]= farP;
    srcTriangle[1]= nearP;
    srcTriangle[2]= otherP;


    dstTriangle[0]= Point2f(static_cast<float>(mark.cols*0.5),static_cast<float>(mark.rows*0.5-2*minD));
    dstTriangle[1]= Point2f(static_cast<float>(mark.cols*0.5),static_cast<float>(mark.rows*0.5+2*minD));
    dstTriangle[2]= Point2f(static_cast<float>(mark.cols*0.5+minD),static_cast<float>(mark.rows*0.5+2*minD));


    todstWarpMat= getAffineTransform(srcTriangle,dstTriangle);

    tosrcWarpMat= getAffineTransform(dstTriangle,srcTriangle);


    cv::Mat showMat= image.clone();

     //摆正标定板
    warpAffine(showMat,dstImage,todstWarpMat,dstImage.size());




    /****************************************************************************************************************/
    //查找中心圆

//    cv::Size boardSize;


//    boardSize.width = 9;
//    boardSize.height = 11;

    cv::Mat vScope;

//    getValidScope(dstImage,board_Size,dstImage);



    /****************************************************************************************************************/



    cv::Mat showSCV_PIn= dstImage.clone();

//    cvShowMat(dstImage,"result");

//     findCircles(dstImage);

//     circle(mark,otherP,radius_v[0],cv::Scalar(0),10);

//    qDebug()<<"flag: "<<flag;

    std::vector<cv::Point2f> imageCorners;
//    std::vector<cv::Point2f> realCorners;

    for(int c= 0;c< dstImage.cols;c++)
    {
        for(int r= 0;r< dstImage.rows;r++)
        {
            dstImage.at<uchar>(r,c)= 255-dstImage.at<uchar>(r,c);
        }

    }

    cv::Size board;

    board.width = 9;
    board.height = 11;

    bool found = cv::findCirclesGrid(dstImage, boardSize, imageCorners, CALIB_CB_SYMMETRIC_GRID | CALIB_CB_CLUSTERING);

    if(!found)
    {
        return found;
    }


    for(int i= 0;i< imageCorners.size();i++)
    {
        float x= imageCorners.at(i).x;
        float y= imageCorners.at(i).y;

        float realX= x*tosrcWarpMat.at<double>(0,0)+ y*tosrcWarpMat.at<double>(0,1) +tosrcWarpMat.at<double>(0,2);
        float realY= x*tosrcWarpMat.at<double>(1,0)+ y*tosrcWarpMat.at<double>(1,1) +tosrcWarpMat.at<double>(1,2);

//        qDebug()<<realX<<","<<realY;

        realCorners.push_back(cv::Point2f(realX,realY));
    }







    return found;

}

void CalibrateMachine::run()
{
    std::vector<cv::Point2f> imageCorners;

    bool found= findCircleBoardCorners(calibrateBoardImg,boardSize,imageCorners);


    if(found)
    {
        std::vector<cv::Point3f> objectCorners;

        for(int i= 0;i< boardSize.height;i++){
            for(int j=0;j<boardSize.width;j++){
                objectCorners.push_back(cv::Point3f(i,j,0.0f));
            }
        }

//        addPoints(imageCorners,objectCorners);
    }

    send_found_circle(found);


}

void CalibrateMachine::setCalibrateBoardImage(cv::Mat img)
{
    calibrateBoardImg= img.clone();

    start();
}


void CalibrateMachine::addPoints(const std::vector<cv::Point2f> &cameraCorners,const std::vector<cv::Point2f> &dlpCorners,const std::vector<cv::Point3f>& objectCorners)
{
    cameraPoints.push_back(cameraCorners);
    dlpPoints.push_back(dlpCorners);
    objectPoints.push_back(objectCorners);

    qDebug()<<"cameraPoints: "<<cameraPoints.size();
    qDebug()<<"dlpPoints: "<<dlpPoints.size();
    qDebug()<<"objectPoints: "<<objectPoints.size();
}

void CalibrateMachine::getValidScope(cv::Mat imgIn,cv::Size boardSize,cv::Mat &imgOut)
{
    cv::Mat img= imgIn.clone();

    std::vector<std::vector<cv::Point>> contours;

    for(int c= 0;c< img.cols;c++)
   {
       for(int r= 0;r< img.rows;r++)
       {
           img.at<uchar>(r,c)= 255-img.at<uchar>(r,c);
       }

   }
    cv::medianBlur(img,img,5);

    threshold(img,img,120,255,cv::THRESH_BINARY);

//    cv::Mat showimg= img.clone();



//    cv::namedWindow("showimg");
//    cv::imshow("showimg",img);

    //cvShowMat(img,"threshold");




    cv::findContours(img,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);

    cv::Mat findContoursMat(img.size(),CV_8U,cv::Scalar(0));

    cv::drawContours(findContoursMat,contours,-1,cv::Scalar(255),2);

    //cvShowMat(findContoursMat,"findContours");



    //移除过长和过短的轮廓
    int cmin= 10;
    int cmax= 300;

     std::vector<std::vector<cv::Point>>:: const_iterator itTemp;

     std::vector<std::vector<cv::Point>>:: const_iterator itl= contours.begin();

     while(itl!= contours.end()){
         if(itl->size()< cmin || itl->size()> cmax){
             itTemp= itl;
             itl= contours.erase(itTemp);
         }
         else
             ++itl;
     }




    cv::Mat meddleContours(img.size(),CV_8U,cv::Scalar(0));

    cv::drawContours(meddleContours,contours,-1,cv::Scalar(255),2);

    //cvShowMat(meddleContours,"meddleContours");



    std::vector<std::vector<cv::Point>>:: const_iterator it_rect= contours.begin();

    while(it_rect!= contours.end()){
        cv::Rect r0= cv::boundingRect(cv::Mat(*it_rect));

        float scale;

        if(r0.width> r0.height)
        {
            scale= r0.width/ r0.height;
        }else{
            scale= r0.height/ r0.width;
        }
        if(scale> 1.1 || r0.x< 0.1*img.cols || r0.y< 0.1*img.rows || r0.x> 0.9*img.cols ||r0.y> 0.9*img.rows)
        {
            it_rect= contours.erase(it_rect);
        } else
            ++it_rect;
    }


//    qDebug()<<"error: can not find mark circle"<< contours.size();




    cv::Mat showContours(img.size(),CV_8U,cv::Scalar(0));

    cv::drawContours(showContours,contours,-1,cv::Scalar(255),2);

    //cvShowMat(showContours,"showContours");



    //半径容器、圆心容器
    std::vector<float> radius_v;
    std::vector<cv::Point2f> centers_v;

    cv::Point2f center;
    float radius;

    std::vector<std::vector<cv::Point>>:: const_iterator itsort= contours.begin();

    for(int i= 0;itsort!= contours.end();i++)
    {
        cv::minEnclosingCircle(cv::Mat(contours[i]),center,radius);

        radius_v.push_back(radius);
        centers_v.push_back(center);

            ++itsort;
    }

//    qDebug()<<"num: "<<centers_v.size();

//    for(int i= 0;i< centers_v.size();i++)
//    {
//        qDebug()<<"r: "<<radius_v[i];
//    }

//    sort(radius_v.begin(),radius_v.end());



    //根据半径排序
    for(int i= 0;i< centers_v.size();i++)
    {
        for(int j= i+1;j< centers_v.size();j++)
        {


            if(radius_v.at(i)< radius_v.at(j))
            {

                std::swap(radius_v[i],radius_v[j]);
                std::swap(centers_v[i],centers_v[j]);

            }
        }
    }

    cv::Mat mark(img.size(),CV_8U,cv::Scalar(255));


    //提取5个大圆
    std::vector<cv::Point2f> mark_point;

    for(int i= 0; i<5; i++)
    {
        mark_point.push_back(centers_v[i]);
        circle(mark,centers_v[i],radius_v[i],cv::Scalar(0),3);
    }

//    scale= 0.25;

//    dsize = Size(mark.cols*scale,mark.rows*scale);

//    cv::resize(mark,mark,dsize);

//    cv::namedWindow("mark");
//    cv::imshow("mark",mark);

    //cvShowMat(mark,"mark");

    std::vector<float> distance;

    for(int i=0; i<5; i++)
    {
        for(int j=i+1;j<5;j++)
        {
            float d= sqrt(pow(centers_v[i].x-centers_v[j].x,2) + pow(centers_v[i].y-centers_v[j].y,2));
            distance.push_back(d);
        }
    }

//    for(int i=0; i< 10;i++)
//    {
//         qDebug()<<distance.at(i);
//    }

    float maxD=0;
    int maxN;

    //找出最大值
    for(int i=0;i<10;i++)
    {
        if(maxD< distance.at(i))
        {
            maxD= distance.at(i);
            maxN= i;
        }

    }


    float minD=maxD;
    int minN;

    //找出最小值
    for(int i=0;i<10;i++)
    {
        if(minD> distance.at(i))
        {
            minD= distance.at(i);
            minN= i;
        }

    }


    qDebug()<<"i: "<<maxN<<"maxD: "<<maxD;
    qDebug()<<"i: "<<minN<<"minD: "<<minD;

    Point2f longP_1,longP_2;
    Point2f shortP_1,shortP_2;
    Point2f otherP,nearP,farP;


    int flag= 10;


    switch(maxN)
    {
        case 0:
        {
            longP_1= centers_v.at(0);
            longP_2= centers_v.at(1);

            flag -=0;
            flag -=1;

        }

            break;
        case 1:
        {
            longP_1= centers_v.at(0);
            longP_2= centers_v.at(2);

            flag -=0;
            flag -=2;
        }

            break;
        case 2:
        {
            longP_1= centers_v.at(0);
            longP_2= centers_v.at(3);

            flag -=0;
            flag -=3;
        }

            break;
        case 3:
        {
            longP_1= centers_v.at(0);
            longP_2= centers_v.at(4);

            flag -=0;
            flag -=4;
        }

            break;

        case 4:
        {
            longP_1= centers_v.at(1);
            longP_2= centers_v.at(2);

            flag -=1;
            flag -=2;
        }

            break;
        case 5:
        {
            longP_1= centers_v.at(1);
            longP_2= centers_v.at(3);

            flag -=1;
            flag -=3;
        }

            break;
        case 6:
        {
            longP_1= centers_v.at(1);
            longP_2= centers_v.at(4);

            flag -=1;
            flag -=4;
        }

            break;
        case 7:
        {
            longP_1= centers_v.at(2);
            longP_2= centers_v.at(3);

            flag -=2;
            flag -=3;
        }

            break;

        case 8:
        {
            longP_1= centers_v.at(2);
            longP_2= centers_v.at(4);

            flag -=2;
            flag -=4;
        }

            break;
        case 9:
        {
            longP_1= centers_v.at(3);
            longP_2= centers_v.at(4);

            flag -=3;
            flag -=4;
        }

            break;
    }

    switch(minN)
    {
        case 0:
        {
            shortP_1= centers_v.at(0);
            shortP_2= centers_v.at(1);

            flag -=0;
            flag -=1;

        }

            break;
        case 1:
        {
            shortP_1= centers_v.at(0);
            shortP_2= centers_v.at(2);

            flag -=0;
            flag -=2;
        }

            break;
        case 2:
        {
            shortP_1= centers_v.at(0);
            shortP_2= centers_v.at(3);

            flag -=0;
            flag -=3;
        }

            break;
        case 3:
        {
            shortP_1= centers_v.at(0);
            shortP_2= centers_v.at(4);

            flag -=0;
            flag -=4;
        }

            break;

        case 4:
        {
            shortP_1= centers_v.at(1);
            shortP_2= centers_v.at(2);

            flag -=1;
            flag -=2;
        }

            break;
        case 5:
        {
            shortP_1= centers_v.at(1);
            shortP_2= centers_v.at(3);

            flag -=1;
            flag -=3;
        }

            break;
        case 6:
        {
            shortP_1= centers_v.at(1);
            shortP_2= centers_v.at(4);

            flag -=1;
            flag -=4;
        }

            break;
        case 7:
        {
            shortP_1= centers_v.at(2);
            shortP_2= centers_v.at(3);

            flag -=2;
            flag -=3;
        }

            break;

        case 8:
        {
            shortP_1= centers_v.at(2);
            shortP_2= centers_v.at(4);

            flag -=2;
            flag -=4;
        }

            break;
        case 9:
        {
            shortP_1= centers_v.at(3);
            shortP_2= centers_v.at(4);

            flag -=3;
            flag -=4;
        }

            break;
    }

    qDebug()<<"shortP_1:　"<<shortP_1.x<<","<<shortP_1.y;
    qDebug()<<"shortP_1:　"<<shortP_2.x<<","<<shortP_2.y;

    qDebug()<<"longP_1:　"<<longP_1.x<<","<<longP_1.y;
    qDebug()<<"longP_2:　"<<longP_2.x<<","<<longP_2.y;

    otherP= centers_v.at(flag);
    farP= otherP;


    float d_1= abs(sqrt(pow(longP_1.x-shortP_1.x,2) + pow(longP_1.y-shortP_1.y,2))-
                   sqrt(pow(longP_2.x-shortP_1.x,2) + pow(longP_2.y-shortP_1.y,2)));

    float d_2= abs(sqrt(pow(longP_1.x-shortP_2.x,2) + pow(longP_1.y-shortP_2.y,2))-
                   sqrt(pow(longP_2.x-shortP_2.x,2) + pow(longP_2.y-shortP_2.y,2)));


    Point2f temp= otherP;

    if(d_1> d_2)
    {
        otherP= shortP_1;
        shortP_1= temp;

        nearP= shortP_2;
    }else{
        otherP= shortP_2;
        shortP_2= temp;

        nearP= shortP_1;
    }

    qDebug()<<"farP:　"<<farP.x<<","<<farP.y;
    qDebug()<<"nearP:　"<<nearP.x<<","<<nearP.y;

    Point2f midpointL,midpointS;

    midpointL.x= (longP_1.x+longP_2.x)/2.0;
    midpointL.y= (longP_1.y+longP_2.y)/2.0;

    midpointS.x= (farP.x+nearP.x)/2.0;
    midpointS.y= (farP.y+nearP.y)/2.0;

    qDebug()<<"midpointS:　"<<midpointS.x<<","<<midpointS.y;
    qDebug()<<"midpointL:　"<<midpointL.x<<","<<midpointL.y;


    Point2f midpoint;

    midpoint.x= (midpointS.x+midpointL.x)/2.0;
    midpoint.y= (midpointS.y+midpointL.y)/2.0;

    float dist= (maxD+ minD)/7;
    float h= (boardSize.width-1)/2+0.5;
    float w= (boardSize.height-1)/2+0.5;

    float lx= midpoint.x- w*dist;
    float ly= midpoint.y- h*dist;
    float rx= midpoint.x+ w*dist;
    float ry= midpoint.y+ h*dist;

    cv::Mat validScope= imgIn.clone();


    for(int r= 0;r< validScope.rows;r++)
    {
        for(int c= 0;c< validScope.cols;c++)
        {
            if(r<ly || c<lx)
            {
                validScope.at<uchar>(r,c)= 0;
            }

            if(r>ry || c>rx)
            {
                validScope.at<uchar>(r,c)= 0;
            }
        }
    }

//    validScope= imgIn(cv::Rect(ly,lx,dist*boardSize.width,dist*boardSize.height));

//    cvShowMat(imgIn,"imgIn");

//    cvShowMat(validScope,"validScope");

    imgOut= validScope.clone();


}


double CalibrateMachine::CalibrateCamera(cv::Size &imageSize)
{
    mustInitUndistort= true;

    std::vector<cv::Mat> rvecs,tvecs;

    return cv::calibrateCamera(objectPoints,
                               imagePoints,
                               imageSize,
                               cameraMatrix,
                               distCoeffs,
                               rvecs,tvecs,
                               flag,cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 50, DBL_EPSILON));
}

bool CalibrateMachine::decodeThreeFrequencyHeterodyne( cv::Mat mask, std::vector<cv::Mat> &mat_list,std::vector<cv::Mat> &result,Yuan_pattern *yuanClass)
{
    QTime t1;
    t1.start();

    if(1 != mat_list.size()%4)
    {
        std::cout<<"Error: Image number is not time of 4!";
        return false;
    }
    cv::Mat cameraImage= mat_list.at(0);//棋盘格图像

    if(1!= cameraImage.channels())
    {
        std::cout<<"Error: The type of image is not mono!";
        return false;
    }
    boardSize.width=12;
    boardSize.height=11;

    std::vector<cv::Point2f> cameraCornerPoints;

    bool found= cv::findChessboardCorners(cameraImage,boardSize,cameraCornerPoints);

//    showMat(cameraImage,"img");

    if(!found)
    {
        std::cout<<"Error: Can not find the corner points!"<<std::endl;
        return false;
    }else
    {
        /*亚像素精确化*/
        cv::find4QuadCornerSubpix(cameraImage,cameraCornerPoints,cv::Size(5,5));
    }

//    cv::Mat show_camera_image= mat_list.at(0);
//    drawChessboardCorners(show_camera_image,boardSize,cameraCirclePoints,found);


      /************************************************************************************************************/


      //周期相位

          std::vector<cv::Mat> wrap_list_x;
          std::vector<cv::Mat> wrap_list_y;

          for(int i= 1;i< 13;i+= 4)
          {

              std::vector<cv::Mat> images;
              images.push_back(mat_list.at(i+ 0));
              images.push_back(mat_list.at(i+ 1));
              images.push_back(mat_list.at(i+ 2));
              images.push_back(mat_list.at(i+ 3));

              cv::Mat result = cv::Mat::zeros(images[0].rows,images[0].cols,CV_64FC1);
              yuanClass->decode_sinusoidal_LookUpTable(mask,images,result,4);
              wrap_list_x.push_back(result);
          }
          for(int i= 13; i< 25 ;i+= 4)
          {

              std::vector<cv::Mat> images;
              images.push_back(mat_list.at(i+ 0));
              images.push_back(mat_list.at(i+ 1));
              images.push_back(mat_list.at(i+ 2));
              images.push_back(mat_list.at(i+ 3));

              cv::Mat result = cv::Mat::zeros(images[0].rows,images[0].cols,CV_64FC1);
              yuanClass->decode_sinusoidal_LookUpTable(mask,images,result,4);
              wrap_list_y.push_back(result);


          }
          cv::Mat unwrapMap_x = cv::Mat::zeros(wrap_list_x[0].rows,wrap_list_x[0].cols,CV_64FC1);
          cv::Mat unwrapMap_y = cv::Mat::zeros(wrap_list_x[0].rows,wrap_list_x[0].cols,CV_64FC1);

          yuanClass->decode_threefrequency(mask,wrap_list_x,unwrapMap_x);
          yuanClass->decode_threefrequency(mask,wrap_list_y,unwrapMap_y);

          result.push_back(unwrapMap_x);
          result.push_back(unwrapMap_y);
          /****************************************************************************************************************************/
           //坐标转换

           std::vector<cv::Point2f> dlpCirclePoints;

           int period= 40;

           pointsCameraToDlp(unwrapMap_x,unwrapMap_y,period,cameraCornerPoints,dlpCirclePoints);

           /****************************************************************************************************************************/

           //添加点

           std::vector<cv::Point3f> objectCorners;

           for(int i= 0;i< boardSize.height;i++){
               for(int j=0;j<boardSize.width;j++){
                   objectCorners.push_back(cv::Point3f(10.0*i,10.0*j,0.0f));
               }
           }

           addPoints(cameraCornerPoints,dlpCirclePoints,objectCorners);

           std::cout<<"decodeThreeFrequencyHeterodyne consumed : "<<t1.elapsed()<<" ms\n";
           return true;


}
void CalibrateMachine::CalibrationEvaluate(std::string name,std::vector< std::vector<cv::Point2f> > &points_Sequence,std::vector<cv::Mat> &rvecs,std::vector<cv::Mat> &tvecs,cv::Mat &intrinsic,cv::Mat distortion_coeff)
{
    double total_err=0.0;//所有图像的平均误差的总和
    double err=0.0;      //每幅图像的平均误差
    std::vector<cv::Point2f> reprojection_image_points; //计算得到的重投影点

    int image_count=points_Sequence.size();
    for(int i=0;i<image_count;i++)
    {
        std::vector<cv::Point3f> temp_obj_Point=objectPoints[i];

        cv::projectPoints(temp_obj_Point,rvecs[i],tvecs[i],intrinsic,distortion_coeff,reprojection_image_points);

        std::vector<cv::Point2f> temp_image_Point=points_Sequence[i];

        cv::Mat temCV_PImagePointMat=cv::Mat(1,temp_image_Point.size(),CV_32FC2);

        cv::Mat reprojectionImagePointsMat=cv::Mat(1,reprojection_image_points.size(),CV_32FC2);

        for(int j=0;j<temp_image_Point.size();j++)
        {
            temCV_PImagePointMat.at<cv::Vec2f>(0,j)=cv::Vec2f(temp_image_Point[j].x,temp_image_Point[j].y);
            reprojectionImagePointsMat.at<cv::Vec2f>(0,j)=cv::Vec2f(reprojection_image_points[j].x,reprojection_image_points[j].y);
        }
        err=cv::norm(reprojectionImagePointsMat,temCV_PImagePointMat,cv::NORM_L2);
        total_err+=err/=boardSize.width*boardSize.height;

        std::cout<<name<<" : "<<"groupNum "<< i+1 <<" calibration error: "<< err <<" CV_PIxel ..."<<std::endl;
    }

    std::cout<<name<<" : "<<"mean error is "<< total_err/image_count <<" CV_PIxel ..."<<std::endl;
    std::cout<<name<<" : "<<"the evaluation is finished ... "<<std::endl<<std::endl;
}
