#include "calibrate.h"

Calibrate::Calibrate()
{
    objectPoints.clear();
    imagePoints.clear();

    boardSize.width = 11;
    boardSize.height = 13;
}

Calibrate::~Calibrate()
{

}


//找到标定板的圆心，传入全局变量objectPoints ，imagePoints
int Calibrate::addCirclebordPoints(QStringList filelist, cv::Size &boardSize)
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
                addPoints(imageCorners,objectCorners);
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


//找出圆形标定板的圆心
bool Calibrate::findCircleBoardCorners(cv::Mat image, cv::Size boardSize, std::vector<cv::Point2f> &realCorners)
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

    threshold(img,img,120,255,cv::THRESH_BINARY);

    cv::findContours(img,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);


    //移除过长和过短的轮廓
    int cmin= 50;
    int cmax= 300;

    std::vector<std::vector<cv::Point>>:: const_iterator itl= contours.begin();

    while(itl!= contours.end()){
        if(itl->size()< cmin || itl->size()> cmax)
            itl= contours.erase(itl);
        else
            ++itl;
    }



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


    if(5> contours.size())
    {
        qDebug()<<"error: can not find mark circle"<< contours.size();
        return false;
    }


    //半径容器、圆心容器
    std::vector<float> radius_v;
    std::vector<cv::Point2f> centers_v;

    cv::Point2f center;
    float radius;

    std::vector<std::vector<cv::Point>>:: const_iterator itr= contours.begin();

    for(int i= 0;itr!= contours.end();i++)
    {
        cv::minEnclosingCircle(cv::Mat(contours[i]),center,radius);

        radius_v.push_back(radius);
        centers_v.push_back(center);

            ++itr;
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

    //五个圆心相互的距离
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

    //找出最大圆心值
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

    //找出最小圆心值
    for(int i=0;i<10;i++)
    {
        if(minD> distance.at(i))
        {
            minD= distance.at(i);
            minN= i;
        }

    }

    cv::Point2f longP_1,longP_2;
    cv::Point2f shortP_1,shortP_2;
    cv::Point2f otherP,nearP,farP;


    //选出最大距离的两个圆和最小距离的两个圆剩下一个方向圆
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


    cv::Point2f temp= otherP;

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


    cv::Point2f srcTriangle[3];
    cv::Point2f dstTriangle[3];

    cv::Mat tosrcWarpMat(2,3,CV_32FC1);
    cv::Mat todstWarpMat(2,3,CV_32FC1);

    cv::Mat dstImage= cv::Mat::zeros(img.rows,img.cols,img.type());

    srcTriangle[0]= farP;
    srcTriangle[1]= nearP;
    srcTriangle[2]= otherP;


    dstTriangle[0]= cv::Point2f(static_cast<float>(image.cols*0.5),static_cast<float>(image.rows*0.5-2*minD));
    dstTriangle[1]= cv::Point2f(static_cast<float>(image.cols*0.5),static_cast<float>(image.rows*0.5+2*minD));
    dstTriangle[2]= cv::Point2f(static_cast<float>(image.cols*0.5+minD),static_cast<float>(image.rows*0.5+2*minD));


    todstWarpMat= getAffineTransform(srcTriangle,dstTriangle);

    tosrcWarpMat= getAffineTransform(dstTriangle,srcTriangle);


    cv::Mat orgImg= image.clone();


    warpAffine(orgImg,dstImage,todstWarpMat,dstImage.size());


    /****************************************************************************************************************/
    //确定有效范围


    getValidScope(dstImage,boardSize,dstImage);



    /****************************************************************************************************************/

    std::vector<cv::Point2f> imageCorners;

    for(int c= 0;c< dstImage.cols;c++)
    {
        for(int r= 0;r< dstImage.rows;r++)
        {
            dstImage.at<uchar>(r,c)= 255-dstImage.at<uchar>(r,c);
        }

    }

    bool found = cv::findCirclesGrid(dstImage, boardSize, imageCorners, CALIB_CB_SYMMETRIC_GRID | CALIB_CB_CLUSTERING);


    for(int i= 0;i< imageCorners.size();i++)
    {
        float x= imageCorners.at(i).x;
        float y= imageCorners.at(i).y;

        float realX= x*tosrcWarpMat.at<double>(0,0)+ y*tosrcWarpMat.at<double>(0,1) +tosrcWarpMat.at<double>(0,2);
        float realY= x*tosrcWarpMat.at<double>(1,0)+ y*tosrcWarpMat.at<double>(1,1) +tosrcWarpMat.at<double>(1,2);


        realCorners.push_back(cv::Point2f(realX,realY));
    }



    return found;


}

void Calibrate::run()
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

        addPoints(imageCorners,objectCorners);
    }

    qDebug()<<"found: "<<found;

    send_found_circle(found);


}
//set完单幅标定板图像就开始线程，单幅送入点
void Calibrate::setCalibrateBoardImage(cv::Mat img)
{
    calibrateBoardImg= img.clone();

    start();
}
//确定有效范围，找圆心函数有调用此函数
void Calibrate::getValidScope(cv::Mat imgIn,cv::Size boardSize,cv::Mat &imgOut)
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
    float h= (boardSize.width-1)/2+2;
    float w= (boardSize.height-1)/2+2;

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


void Calibrate::addPoints(const std::vector<cv::Point2f> &imageCorners, const std::vector<cv::Point3f> &objectCorners)
{
    imagePoints.push_back(imageCorners);
    objectPoints.push_back(objectCorners);
}


double Calibrate::CalibrateCamera(cv::Size &imageSize)
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
