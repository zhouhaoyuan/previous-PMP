#include "reconstruct3d.h"

Reconstruct3D::Reconstruct3D()
{
    cameraA= cv::Mat::zeros(3,3,CV_64FC1);
    dlpA= cv::Mat::zeros(3,3,CV_64FC1);
    cameraM= cv::Mat::zeros(3,4,CV_64FC1);
    dlpM= cv::Mat::zeros(3,4,CV_64FC1);
    cameraKc= cv::Mat::zeros(1,5,CV_64FC1);
    dlpKc= cv::Mat::zeros(1,5,CV_64FC1);

    R_Mat= cv::Mat::zeros(3,3,CV_64FC1);
    T_Mat= cv::Mat::zeros(3,1,CV_64FC1);
}

Point_3D::Point_3D()
{

}

void Reconstruct3D::cvShowMat(cv::Mat img, std::string name)
{


    cv::Mat showMat= img.clone();

    float scale= 0.5;
    cv::Size dsize;

    dsize = cv::Size(showMat.cols*scale,showMat.rows*scale);

    cv::resize(showMat,showMat,dsize);

    cv::namedWindow(name);
    cv::imshow(name,showMat);

//    cv::waitKey(0);
}

void Reconstruct3D::cvShowMatFloat(cv::Mat img, std::string name,int period)
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

//showMat.at<uchar>(r,c)= 255*img.at<float>(r,c)/(PI*pow(2.0,period));
    float scale= 0.25;
    cv::Size dsize;

    dsize = cv::Size(showMat.cols*scale,showMat.rows*scale);

    cv::resize(showMat,showMat,dsize);

    cv::namedWindow(name);
    cv::imshow(name,showMat);


}


void Reconstruct3D::readCalibPara()
 {
    QFile calib_file("../ScanData/calibResult.txt");
    if(!calib_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Open failed!";
        return;
    }

    QTextStream calib_input(&calib_file);

    QString txt= calib_input.readAll();

    calib_file.close();



    QStringList str_list= txt.split("[");

    QStringList matrix_list;

    for(int i= 1;i< str_list.size();i++)
    {
        QStringList list= str_list.at(i).split("]");
        matrix_list.push_back(list.at(0));
//            qDebug()<<i<<": "<<str_list.at(i);
    }



    std::vector<std::vector<double>> martix_elments;

    for(int m=0;m< matrix_list.size();m++)
    {
        QString str_matrix= matrix_list.at(m);
        QStringList str_num_list= str_matrix.split(QRegExp("[\,\;]"));
        std::vector<double> elments;

        for(int i= 0;i< str_num_list.size();i++)
        {
            QString str= str_num_list.at(i);
            elments.push_back(str.toDouble());
        }

        martix_elments.push_back(elments);
    }


    double ca[3][3];

    std::vector<double> ca_num_list= martix_elments.at(0);

    for(int i= 0;i< ca_num_list.size();i++)
    {
        ca[i/3][i%3]= ca_num_list.at(i);
    }

    cameraA= cv::Mat(3,3,CV_64FC1,ca);


    double ckc[1][5];

    std::vector<double> ckc_num_list= martix_elments.at(1);

    for(int i= 0;i< ckc_num_list.size();i++)
    {
        ckc[0][i]= ckc_num_list.at(i);
    }

    cameraKc= cv::Mat(1,5,CV_64FC1,ckc);



    double da[3][3];

    std::vector<double> da_num_list= martix_elments.at(2);

    for(int i= 0;i< da_num_list.size();i++)
    {
        da[i/3][i%3]= da_num_list.at(i);
    }

    dlpA= cv::Mat(3,3,CV_64FC1,da);

    double dkc[1][5];

    std::vector<double> dkc_num_list= martix_elments.at(3);

    for(int i= 0;i< dkc_num_list.size();i++)
    {
        dkc[0][i]= dkc_num_list.at(i);
    }

    dlpKc= cv::Mat(1,5,CV_64FC1,dkc);


    double cm[3][4]= {1, 0, 0,0,
                      0, 1, 0,0,
                      0, 0, 1,0};
    cameraM= cv::Mat(3,4,CV_64FC1,cm);



    double dm[3][4];
    double dR[3][3];

    std::vector<double> r_num_list= martix_elments.at(4);

    for(int i= 0;i< r_num_list.size();i++)
    {
        dm[i/3][i%3]= r_num_list.at(i);
        dR[i/3][i%3]= r_num_list.at(i);
    }

    R_Mat=  cv::Mat(3,3,CV_64FC1,dR);




    double dT[3][1];
    std::vector<double> t_num_list= martix_elments.at(5);

    for(int i= 0;i<3;i++)
    {
        dm[i][3]= t_num_list.at(i);
        dT[i][0]= t_num_list.at(i);
    }

    T_Mat= cv::Mat(3,1,CV_64FC1,dT);


    dlpM= cv::Mat(3,4,CV_64FC1,dm);





    M_1= cv::Mat::zeros(3,4,CV_64FC1);
    M_2= cv::Mat::zeros(3,4,CV_64FC1);


    M_1= cameraA*cameraM;
    M_2= dlpA*dlpM;


//    std::cout<<"cameraA: "<<"\n"<<cameraA<<"\n";

//    std::cout<<"cameraM: "<<"\n"<<cameraM<<"\n";

//    std::cout<<"cameraKc: "<<"\n"<<cameraKc<<"\n";

//    std::cout<<"M_1: "<<"\n"<<M_1<<"\n";

//    std::cout<<"dlpA: "<<"\n"<<dlpA<<"\n";

//    std::cout<<"dlpM: "<<"\n"<<dlpM<<"\n";

//    std::cout<<"dlpKc: "<<"\n"<<dlpKc<<"\n";

//    std::cout<<"M_2: "<<"\n"<<M_2<<"\n";

//    std::cout<<"R_Mat: "<<"\n"<<R_Mat<<"\n";

//    std::cout<<"T_Mat: "<<"\n"<<T_Mat<<"\n";

//    std::cout.flush();

//    qDebug("%.16f",M_1.at<double>(0,0));
 }


//void Reconstruct3D::generateHieraryPointCloud(std::vector<cv::Mat> in_mat)
//{
//    std::vector<cv::Mat> unwrap_x_list,unwrap_y_list;

////    decodeHierarchyPatterns(in_mat,unwrap_x_list,unwrap_y_list);

//    for(int i= 0;i< unwrap_x_list.size();i++)
//    {
//        cv::Mat unwrap_x= unwrap_x_list.at(i);
//        cv::Mat unwrap_y= unwrap_y_list.at(i);

//        QTime time;
//        time.start();


//        std::vector<cv::Point2f> left_p,right_p;
//        std::vector<Point3f> pCloud;

//        time.restart();

//        createCorrectPoints(unwrap_x,unwrap_y ,left_p,right_p);


//        qDebug()<<i<<" :  "<<"createCorrectPoints: "<<time.elapsed();

//        pCloud.clear();
//        time.restart();

//        restruct3D(left_p, right_p, pCloud);
//        qDebug()<<i<<" :  "<<"points: "<<pCloud.size();
//        qDebug()<<i<<" :  "<<"restruct3D time: "<<time.elapsed()<<" ms";
//        savePointCloud(pCloud,"../DlpConfig/output/pCloudOneWay_all_"  +QString::number(i)+  ".xyz");

//    }
//}

void Reconstruct3D::generateMultiplyWavelengthPointCloud(std::vector<cv::Mat> in_mat)
{
    cv::Mat unwrap_x,unwrap_y;



    cv::Mat mask_mat= m_mask.clone();

    QTime time;
    time.start();


    std::vector<cv::Point2f> left_p,right_p;
    std::vector<Point3f> pCloud;

    time.restart();

    createCorrectPoints(unwrap_x,unwrap_y ,left_p,right_p,mask_mat);



    time.restart();




    pCloud.clear();
    time.restart();

//    restruct3D(left_p, right_p, pCloud);
//    qDebug()<<"points: "<<pCloud.size();
//    qDebug()<<"restruct3D time: "<<time.elapsed()<<" ms";
//    savePointCloud(pCloud,"../pCloudOneWay_all_m_p.xyz");


    restruct3D(left_p, right_p, pCloud);
    qDebug()<<"points: "<<pCloud.size();
    qDebug()<<"restruct3D time: "<<time.elapsed()<<" ms";
    savePointCloud(pCloud,output_path+ "_pCloud.ply");


    QString open_file= output_path+ "_pCloud.ply";

    QProcess *process = new QProcess;
    QStringList str;
    str << open_file;
    process->start("D:/Program Files/VCG/MeshLab/meshlab.exe",str);

    //    QProcess *process = new QProcess;
    //    QStringList str;
    //    str << savePoints;
    //    process->start("D:/Program Files/VCG/MeshLab/meshlab.exe",str);
}


void Reconstruct3D::saveMask(std::vector<cv::Mat> m_list)
{
    cv::Mat mat_0= m_list.at(0);
    cv::Mat mat_1= m_list.at(1);
    cv::Mat mat_2= m_list.at(2);
    cv::Mat mat_3= m_list.at(3);

    int nr= mat_0.rows;
    int nc= mat_0.cols;

    cv::Mat compute_Mat(nr,nc,CV_8U);


    for(int r= 0;r< nr;r++)
    {
        uchar* ptr_0= mat_0.ptr<uchar>(r);
        uchar* ptr_1= mat_1.ptr<uchar>(r);
        uchar* ptr_2= mat_2.ptr<uchar>(r);
        uchar* ptr_3= mat_3.ptr<uchar>(r);

        uchar* ptr_compute= compute_Mat.ptr<uchar>(r);

        for(int c= 0;c< nc;c++)
        {
            ptr_compute[c]=  (ptr_0[c]+ ptr_1[c]+ptr_2[c]+ptr_3[c])/4.0;

        }
    }


    cv::imwrite("../mask.bmp",compute_Mat);
}

void Reconstruct3D::computeMaskImage(std::vector<cv::Mat> m_list,cv::Mat& mask_mat)
{
    cv::Mat mat_0= m_list.at(0);
    cv::Mat mat_1= m_list.at(1);
    cv::Mat mat_2= m_list.at(2);
    cv::Mat mat_3= m_list.at(3);

    int nr= mat_0.rows;
    int nc= mat_0.cols;

    cv::Mat compute_Mat(nr,nc,CV_8U);


    for(int r= 0;r< nr;r++)
    {
        uchar* ptr_0= mat_0.ptr<uchar>(r);
        uchar* ptr_1= mat_1.ptr<uchar>(r);
        uchar* ptr_2= mat_2.ptr<uchar>(r);
        uchar* ptr_3= mat_3.ptr<uchar>(r);

        uchar* ptr_compute= compute_Mat.ptr<uchar>(r);

        for(int c= 0;c< nc;c++)
        {
            ptr_compute[c]=  (ptr_0[c]+ ptr_1[c]+ptr_2[c]+ptr_3[c])/4;

        }
    }


    mask_mat= compute_Mat.clone();



}

void Reconstruct3D::generatePointCloud(std::vector<cv::Mat> in_mat)
{
    readCalibPara();

    generateMultiplyWavelengthPointCloud(in_mat);


}



void Reconstruct3D::loadImages(std::vector<cv::Mat> mats, Mat mask, QString c_path, QString out_path)
{
    ps_mat_list= mats;

    m_mask= mask.clone();
    calib_path= c_path;
    output_path= out_path;
}


void Reconstruct3D::IntersectionRestruct3D(std::vector<cv::Point2f> leftPoints, std::vector<cv::Point2f> rightPoints, std::vector<Point3f> &pCloud)
{
    int points_num= leftPoints.size();

    float col_disparity_value = 1.0;
    float row_disparity_value = 1.0;
    Point3f point;
    double distance=0;


//    std::cout<<"cameraA: "<<"\n"<<cameraA<<"\n";

//    std::cout<<"cameraKc: "<<"\n"<<cameraKc<<"\n";

//    std::cout<<"dlpA: "<<"\n"<<dlpA<<"\n";

//    std::cout<<"dlpKc: "<<"\n"<<dlpKc<<"\n";

//    std::cout<<"R_Mat: "<<"\n"<<R_Mat<<"\n";

//    std::cout<<"T_Mat: "<<"\n"<<T_Mat<<"\n";

//    std::cout.flush();

    for(int i= 0;i< points_num;i++)
    {
//        GeneratePoint(cam_K, cam_kc, proj_K, proj_kc, Rt, T,
//                                      xCol, yRow, col_disparity_value, row_disparity_value, &point, &distance);
        GeneratePoint(cameraA, cameraKc, dlpA, dlpKc, R_Mat, T_Mat,
                      leftPoints.at(i).x, leftPoints.at(i).y, rightPoints.at(i).x, rightPoints.at(i).y, &point, &distance);

//        if(distance< 100)
//        {

            pCloud.push_back(point);
//        }

    }




}


void Reconstruct3D::savePointCloud(std::vector<Point3f> pCloud,QString path)
{
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


    for(int i= 0;i< pCloud.size();i++)
    {
        Point3f resultP= pCloud.at(i);

        p_out<<resultP.x<<" "<<resultP.y<<" "<<resultP.z;
        p_out<<"\r\n";
    }





    p_file.close();

}

void Reconstruct3D::restruct3D(std::vector<cv::Point2f> leftPoints, std::vector<cv::Point2f> rightPoints, std::vector<cv::Point3f> &pCloud)
{



    /*****************************************************************************************************************************/
    cv::Mat leftM= cv::Mat::zeros(4,3,CV_64FC1);
    cv::Mat rightM= cv::Mat::zeros(4,1,CV_64FC1);
    cv::Mat point= cv::Mat::zeros(3,1,CV_64FC1);

    int pointsNum= leftPoints.size();

    cv::Point3f resultP;

    pCloud.clear();


    /*******************************************************************************************************************************/
    //.ply头


//    p_out<<"ply";
//    p_out<<"\r\n";
//    p_out<<"format ascii 1.0";
//    p_out<<"\r\n";
//    p_out<<"comment made by scan";
//    p_out<<"\r\n";

//    p_out<<"element vertex "<<pointsNum;
//    p_out<<"\r\n";

//    p_out<<"property float x ";
//    p_out<<"\r\n";


//    p_out<<"property float y ";
//    p_out<<"\r\n";

//    p_out<<"property float z ";
//    p_out<<"\r\n";

//    p_out<<"end_header";
//    p_out<<"\r\n";



    //to image camera coordinates

    /*********************************************************************************************************************************/

    int badPoints= 0;
//    std::cout<<"cameraA: "<<cameraA<<std::endl;
//    std::cout<<"cameraKc: "<<cameraKc<<std::endl;
//    std::cout.flush();

    for(int i= 0;i< pointsNum;i++)
    {
        cv::Point2f cameraP= leftPoints.at(i);
        cv::Point2f dlpP= rightPoints.at(i);

        if( dlpP.x> 912 |  dlpP.x< 0 | dlpP.y > 570 | dlpP.y< 0)
        {
            badPoints++;
            continue;
        }




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

//        out<<resultP.x<<" "<<resultP.y<<" "<<resultP.z;
//        out<<"\r\n";

//        p_out<<resultP.x<<" "<<resultP.y<<" "<<resultP.z;
//        p_out<<"\r\n";

        pCloud.push_back(resultP);




    }
//    file.close();
//    p_file.close();

//    qDebug()<<"badPoints: "<<badPoints;

//    std::cout<<"cameraA: "<<cameraA<<std::endl;
//    std::cout<<"cameraKc: "<<cameraKc<<std::endl;
//    std::cout.flush();

    //"C:\Program Files\VCG\MeshLab\meshlab.exe"

//    QProcess *process = new QProcess;
//    QStringList str;
//    str << savePoints;
//    process->start("D:/Program Files/VCG/MeshLab/meshlab.exe",str);



}


void Reconstruct3D::phaseToPixelFloat(cv::Mat phaseX, cv::Mat phaseY, int period, QString name)
{
    //写入txt文件

    QString file_name, file_path;
    QFileInfo fi;

//    fi = QFileInfo(readPath);
//    file_name = fi.fileName();
//    file_path = fi.absolutePath();
//    QString saveFile= QFileDialog::getSaveFileName(this,"Set Save FileName: ",name);

    QString saveFile= "../"+ name;

    if(saveFile.isEmpty())
    {
        return ;
    }

    saveFile += ".txt";



    QFile file(saveFile);
    if(!file.open(QIODevice::WriteOnly)){
        qDebug()<<"Open Error!!!";
        return ;
    }

    QTextStream out(&file);
    for(int r= 0;r< phaseX.rows; r++)
    {
        for(int c= 0;c< phaseX.cols; c++)
        {
//            if(0!= wrapRoi.at<uchar>(r,c))
//            {
                float xValue= phaseX.at<float>(r,c);
                float yValue= phaseY.at<float>(r,c);

                if(abs(xValue) > 0.001 && abs(yValue)   > 0.001)
                {

                    float temp_x= 1140*xValue;
                    float temp_y= 912*yValue;

                    float dlp_x_value= temp_x/(PI*std::pow(2.0,period-1));
                    float dlp_y_value= temp_y/(PI*std::pow(2.0,period-1));

                    out<<c<<","<<r<<","<<dlp_x_value<<","<<dlp_y_value;
                    out<<"\r\n";

                }
//            }
        }
    }

    file.close();
}


void Reconstruct3D::createCorrectPoints(cv::Mat phaseX, cv::Mat phaseY, std::vector<cv::Point2f> &l_points, std::vector<cv::Point2f> &r_points,cv::Mat mask)
{
    l_points.clear();
    r_points.clear();

    int nr= phaseX.rows;
    int nc= phaseX.cols;




//    std::vector<cv::Point2f> inputDistortedPoints;
//    std::vector<cv::Point2f> outputUndistortedPoints;

//    inputDistortedPoints.push_back(cv::Point2f(120,562));
//    inputDistortedPoints.push_back(cv::Point2f(220,562));
//    inputDistortedPoints.push_back(cv::Point2f(320,562));
//    inputDistortedPoints.push_back(cv::Point2f(420,562));
//    inputDistortedPoints.push_back(cv::Point2f(520,562));

//    cv::Mat cameraMatrix = cameraA.clone();
//    cv::Mat distCoeffs = cameraKc.clone();

//    cv::undistortPoints(inputDistortedPoints, outputUndistortedPoints, cameraMatrix, distCoeffs, cv::noArray(), cameraMatrix);

    ////        cv::undistortPoints(inputDistortedPoints, outputUndistortedPoints, cameraA, cameraKc,cv::noArray());   //Undistort points

//    for(int i= 0;i< outputUndistortedPoints.size();i++)
//    {
////        qDebug()<<"x: "<<outputUndistortedPoints.at(i).x<<" y: "<<outputUndistortedPoints.at(i).y;
//    }

    //        std::cout<<"cameraA: "<<std::endl<<cameraA<<std::endl;
    //        std::cout<<"cameraKc: "<<std::endl<<cameraKc<<std::endl;
    //std::cout.flush();

    //        float* ptr_r;
    //        ptr_r = dst.ptr<float>(0);            // Fill src with correct pixel x and y values

    //        qDebug()<<"ptr_r[0] "<<ptr_r[0];
    //        qDebug()<<"ptr_r[1] "<<ptr_r[1];

    for(int r= 0;r< nr; r++)
    {

        double* x_ptr= phaseX.ptr<double>(r);
        double* y_ptr= phaseY.ptr<double>(r);
        uchar*  m_ptr= mask.ptr<uchar>(r);

        for(int c= 0;c< nc; c++)
        {

            double xValue= x_ptr[c];
            double yValue= y_ptr[c];

            //                float xValue= phaseX.at<float>(r,c);
            //                float yValue= phaseY.at<float>(r,c);

            if(m_ptr[c]> 45)
            {
                if(abs(xValue) < PI &&abs(xValue) > 0.001 &&abs(yValue) < PI && abs(yValue)   > 0.001)
                {
                    double temp_x= 912*xValue;
                    double temp_y= 570*yValue;

                    double dlp_x_value= temp_x/PI;
                    double dlp_y_value= temp_y/PI;


                    l_points.push_back(cv::Point2f(c,r));
                    r_points.push_back(cv::Point2f(dlp_x_value,dlp_y_value));
                }

            }



        }
    }

//    qDebug()<<"num: "<< r_points.size();
}

void Reconstruct3D::createCorrectPoints(cv::Mat phaseX, cv::Mat phaseY, std::vector<Point2f> &l_points, std::vector<Point2f> &r_points)
{
    l_points.clear();
    r_points.clear();

    int nr= phaseX.rows;
    int nc= phaseX.cols;


    for(int r= 0;r< nr; r++)
    {

        double* x_ptr= phaseX.ptr<double>(r);
        double* y_ptr= phaseY.ptr<double>(r);

        for(int c= 0;c< nc; c++)
        {

            double xValue= x_ptr[c];
            double yValue= y_ptr[c];

            //                float xValue= phaseX.at<float>(r,c);
            //                float yValue= phaseY.at<float>(r,c);


            if(abs(xValue) > 0.001 && abs(yValue)   > 0.001)
            {
                double temp_x= 912*xValue;
                double temp_y= 570*yValue;

                double dlp_x_value= temp_x/PI;
                double dlp_y_value= temp_y/PI;


                l_points.push_back(cv::Point2f(c,r));
                r_points.push_back(cv::Point2f(dlp_x_value,dlp_y_value));
            }


        }
    }

    qDebug()<<"num: "<< r_points.size();
}

/*
 * point和distance是输出参数，其它都是输入参数
 */
inline void Reconstruct3D::GeneratePoint(const cv::Mat &K1, const cv::Mat &kc1, const cv::Mat &K2, const cv::Mat &kc2,
                                          const cv::Mat &Rt, const cv::Mat &T, const unsigned p1_x, const unsigned p1_y,
                                          const float p2_x, const float p2_y, Point3f *point, double *distance)
{


    std::cout<<"K1: "<<"\n"<<K1<<"\n";

    std::cout<<"kc1: "<<"\n"<<kc1<<"\n";

    std::cout.flush();


    //to image camera coordinates
    cv::Mat inp1(1, 1, CV_64FC2), inp2(1, 1, CV_64FC2);
    inp1.at<cv::Vec2d>(0, 0) = cv::Vec2d(p1_x, p1_y);
    inp2.at<cv::Vec2d>(0, 0) = cv::Vec2d(p2_x, p2_y);
    cv::Mat outp1, outp2;
    cv::undistortPoints(inp1, outp1, K1, kc1);
    cv::undistortPoints(inp2, outp2, K2, kc2);


//    std::cout<<"inp1: "<<"\n"<<inp1<<"\n";
//    std::cout<<"inp2: "<<"\n"<<inp2<<"\n";


//    std::cout.flush();

//    assert(outp1.type()==CV_64FC2 && outp1.rows==1 && outp1.cols==1);
//    assert(outp2.type()==CV_64FC2 && outp2.rows==1 && outp2.cols==1);
//    const cv::Vec2d & outvec1 = outp1.at<cv::Vec2d>(0,0);
//    const cv::Vec2d & outvec2 = outp2.at<cv::Vec2d>(0,0);

    const cv::Vec2d & outvec1 = inp1.at<cv::Vec2d>(0,0);
    const cv::Vec2d & outvec2 = inp2.at<cv::Vec2d>(0,0);


    cv::Point3d u1(outvec1[0], outvec1[1], 1.0);
    cv::Point3d u2(outvec2[0], outvec2[1], 1.0);

    //to world coordinates
    cv::Point3d w1 = u1;
    cv::Point3d w2 = cv::Point3d(cv::Mat(Rt*(cv::Mat(u2) - T)));

    //world rays
    cv::Point3d v1 = w1;
    cv::Point3d v2 = cv::Point3d(cv::Mat(Rt*cv::Mat(u2)));

    double out_l1,out_l2;



    ApproximateRayIntersection(v1, w1, v2, w2, point, distance, &out_l1, &out_l2);
}

inline void Reconstruct3D::ApproximateRayIntersection(const cv::Point3d &v1, const cv::Point3d &q1, const cv::Point3d &v2, const cv::Point3d &q2,
                                                       Point3f *point, double *distance, double *out_lambda1, double *out_lambda2)
{
    cv::Mat v1mat = cv::Mat(v1);
    cv::Mat v2mat = cv::Mat(v2);

    double v1tv1 = cv::Mat(v1mat.t()*v1mat).at<double>(0,0);
    double v2tv2 = cv::Mat(v2mat.t()*v2mat).at<double>(0,0);
    double v1tv2 = cv::Mat(v1mat.t()*v2mat).at<double>(0,0);
    double v2tv1 = cv::Mat(v2mat.t()*v1mat).at<double>(0,0);

    double detV = v1tv1*v2tv2 - v1tv2*v2tv1;

    cv::Point3d q2_q1 = q2 - q1;
    double Q1 = v1.x*q2_q1.x + v1.y*q2_q1.y + v1.z*q2_q1.z;
    double Q2 = -(v2.x*q2_q1.x + v2.y*q2_q1.y + v2.z*q2_q1.z);

    double lambda1 = (v2tv2 * Q1 + v1tv2 * Q2) /detV;
    double lambda2 = (v2tv1 * Q1 + v1tv1 * Q2) /detV;

    cv::Point3d p1 = lambda1*v1 + q1; //ray1
    cv::Point3d p2 = lambda2*v2 + q2; //ray2

    point->x = 0.5*(p1.x+p2.x);
    point->y = 0.5*(p1.y+p2.y);
    point->z = 0.5*(p1.z+p2.z);



    if (distance!=NULL){
        *distance = cv::norm(p2-p1);
    }
    if (out_lambda1){
        *out_lambda1 = lambda1;
    }
    if (out_lambda2){
        *out_lambda2 = lambda2;
    }

//    qDebug()<<"x: "<<point->x;
//    qDebug()<<"y: "<<point->y;
//    qDebug()<<"x: "<<point->x;
//    qDebug()<<"distance: "<<*distance;
}

