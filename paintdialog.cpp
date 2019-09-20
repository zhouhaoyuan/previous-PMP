#include "paintdialog.h"
#include "ui_paintdialog.h"

paintDialog::paintDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::paintDialog)
{
    ui->setupUi(this);

    directionGroup=new QButtonGroup(this);
    directionGroup->addButton(ui->vertical_radioBtn,0);//x
    directionGroup->addButton(ui->Horizontal_rationBtn,1);//y

    ui->color_cBox->addItem(tr("red"));
    ui->color_cBox->addItem(tr("green"));
    ui->color_cBox->addItem(tr("blue"));

    ui->dot_line_cBox->addItem(tr("SolidLine"));
    ui->dot_line_cBox->addItem(tr("DashLine"));
    ui->dot_line_cBox->addItem(tr("DotLine"));
    ui->dot_line_cBox->addItem(tr("Dot"));

    ui->vertical_radioBtn->setCheckable(true);
    ui->vertical_radioBtn->setChecked(true);

    ui->YvalueEdit->setValue(15);
    ui->NumSpinbox->setValue(500);

    pixmap=new QPixmap(1200,800);
    pixmap->fill(Qt::white);
}

paintDialog::~paintDialog()
{
    delete ui;
}


void paintDialog::crosstalk_Paint(cv::Mat &srcData,channel channel_select,cv::Mat &mask)
{
    if(srcData.channels()!=3)
    {
        std::cout<<"the number of channel is not enough!"<<std::endl;
        return;
    }

    std::vector<cv::Mat> channels;
    cv::split(srcData, channels);             //利用vector对象分离

    cv::Mat Data;

    QPen penPhase;

    switch (channel_select) {
    case red:
    {
        penPhase.setColor(Qt::red);
        penPhase.setWidth(1);
        Data=channels[2].clone();
    }
        break;
    case green:
    {
        penPhase.setColor(Qt::green);
        penPhase.setWidth(1);
        Data=channels[1].clone();
    }
        break;
    case blue:
    {
        penPhase.setColor(Qt::blue);
        penPhase.setWidth(1);
        Data=channels[0].clone();
    }
        break;
    default:
        break;
    }

    QPainter painter(pixmap);
    painter.setRenderHint(QPainter::Antialiasing,true);

    int width=450,height=300; //坐标轴宽度、高度
    int axis_X=50+5,axis_Y=height+50+5;  //坐标轴原点坐标
    painter.drawRect(axis_X,axis_Y-height,width,height);

    int axisSize=0;
    std::vector<unsigned int> temp;

    switch (directionGroup->checkedId()) {
    case 1://y
    {
         axisSize=srcData.rows;
         for(int i=0;i<axisSize;i++)
         {
             uchar* ptr = mask.ptr<uchar>(i);
             if(ptr[srcData.cols/2]!=0)
             {
                temp.push_back(Data.at<uchar>(i,srcData.cols/2));
             }
         }
    }
        break;
    case 0://x
    {
         axisSize=srcData.cols;
         for(int i=0;i<axisSize;i++)
         {
             uchar* ptr = mask.ptr<uchar>(srcData.rows/2);
             if(ptr[i]!=0)
             {
                temp.push_back(Data.at<uchar>(srcData.rows/2,i));
             }
         }
    }
        break;

    }

    double minvalue=1000.0,maxvalue=0.0;

    for(int i=0;i<temp.size();i++)
    {
        if(minvalue>temp[i])
            minvalue=temp[i];
        if(maxvalue<temp[i])
            maxvalue=temp[i];
    }

    std::cout<<"the vector size is : "<<temp.size()<<" ; The min value is : "<<minvalue<<" ; The max value is : "<<maxvalue<<std::endl;

    //字体
    QFont font;
    font.setFamily("Microsoft YaHei");
    // 大小
    font.setPointSize(13);
    font.setBold(true);
//    // 斜体
//    font.setItalic(true);
//    // 设置下划线
//    font.setUnderline(true);
//    // 设置上划线
//    font.setOverline(true);
    // 设置字母大小写
//    font.setCapitalization(QFont::SmallCaps);
//    // 设置字符间距
//    font.setLetterSpacing(QFont::AbsoluteSpacing, 20);

    // 使用字体
    painter.setFont(font);

    //绘制刻度线
    QPen penScale;
    penScale.setColor(Qt::black);
    penScale.setWidth(2);
    painter.setPen(penScale);

    double X_step=(double)temp.size()/10;
    painter.drawText(axis_X-10,axis_Y+15,QString::number(0));
    for(int i=0;i<10;i++)
    {
        painter.drawLine(axis_X+(i+1)*width/10,axis_Y,axis_X+(i+1)*width/10,axis_Y-5);
        painter.drawLine(axis_X+(i+1)*width/10,axis_Y-height,axis_X+(i+1)*width/10,axis_Y-height+5);
        painter.drawText(axis_X+(i+0.4)*width/10,axis_Y+15,QString::number((int)((i+1)*X_step)));
        painter.drawText(axis_X+width/2-50,axis_Y+40,tr("Position/pixel"));
    }

    int Yaxle_value=0;
    if(maxvalue>6.3)
    {
        Yaxle_value=300;
    }
    else if(maxvalue <= 1)
    {
        Yaxle_value=1;
    }else{
        Yaxle_value=10;
    }

    double Y_step=(double)Yaxle_value/10;

    penScale.setColor(Qt::black);
    penScale.setStyle(Qt::SolidLine);
    painter.setPen(penScale);

    QPen penHorizontal(Qt::black,1,Qt::DotLine,Qt::RoundCap);

    QTransform transform;
    transform.rotate(-90);
    painter.setWorldTransform(transform);
    painter.drawText(-(50+5+height/2+30),15,tr("Intensity"));
    transform.rotate(-270);
    painter.setWorldTransform(transform);
    painter.setPen(penScale);
    for(int j=0;j<10;j++)
    {              
        painter.drawLine(axis_X,axis_Y-(j+1)*height/10,axis_X+5,axis_Y-(j+1)*height/10);
        painter.drawLine(axis_X+width,axis_Y-(j+1)*height/10,axis_X+width-5,axis_Y-(j+1)*height/10);
        painter.drawText(axis_X-35,axis_Y-(j+0.8)*height/10,QString::number((Y_step*(j+1))));
        painter.setPen(penHorizontal);
        //painter.drawLine(axis_X,axis_Y-(j+1)*height/10,axis_X+width,axis_Y-(j+1)*height/10);
    }

    //绘制图案

//    penScale.setColor(Qt::red);
//    penScale.setStyle(Qt::SolidLine);
//    painter.setPen(penScale);
//    painter.drawLine(axis_X+width-130,axis_Y-height/10*9.5,axis_X+width-120,axis_Y-height/10*9.5);
//    painter.drawText(axis_X+width-110,axis_Y-height/10*9.2,tr("Sin Code"));
//    penScale.setColor(Qt::green);
//    penScale.setStyle(Qt::SolidLine);
//    painter.setPen(penScale);
//    painter.drawLine(axis_X+width-240,axis_Y-height/10*9.5,axis_X+width-230,axis_Y-height/10*9.5);
//    painter.drawText(axis_X+width-220,axis_Y-height/10*9.2,tr("Gray Code"));
//    penScale.setColor(Qt::blue);
//    penScale.setStyle(Qt::SolidLine);
//    painter.setPen(penScale);
//    painter.drawLine(axis_X+width-350,axis_Y-height/10*9.5,axis_X+width-340,axis_Y-height/10*9.5);
//    painter.drawText(axis_X+width-330,axis_Y-height/10*9.2,tr("Stair Code"));

    QPen penPoint;
    penPoint.setColor(Qt::red);
    penPoint.setWidth(2);
    double kx=(double)width/temp.size();
    double ky=(double)height/Yaxle_value;
    int n=0;
    while(n!=(temp.size()-1))
    {
        painter.setPen(penPhase);
        painter.drawLine(axis_X+n*kx,axis_Y-temp[n]*ky,axis_X+(n+1)*kx,axis_Y-temp[n+1]*ky);
        //painter.setPen(penPoint);
        //painter.drawPoint(axis_X+n*kx,50+temp[n]*ky);
        n++;
    }

    update();
}

void paintDialog::phase_Paint(cv::Mat &srcData,direction _direction,cv::Mat &mask)
{
    if(srcData.channels()!=1)
    {
        std::cout<<"the number of srcData's channels is not correct ... "<<std::endl;
        return;
    }
    QPainter painter(pixmap);
    painter.setRenderHint(QPainter::Antialiasing,true);
    int width=450,height=300; //坐标轴宽度、高度
    int axis_X=50+5,axis_Y=height+50+5;  //坐标轴原点坐标
    painter.drawRect(axis_X,axis_Y-height,width,height);

    int axisSize=0;
    std::vector<double> temp;

    //cv::normalize(srcData,srcData,1.0,0.0,cv::NORM_MINMAX);

    switch (_direction) {
    case vertical://y
    {
        axisSize=srcData.rows;
        for(int i=0;i<axisSize;i++)
        {
            uchar *ptr=mask.ptr<uchar>(i);
            if(ptr[srcData.cols/2]!=0)
            {
                temp.push_back(srcData.at<double>(i,ui->NumSpinbox->value()));
            }
        }
    }
        break;
    case horizontal://x
    {
        axisSize=srcData.cols;
        for(int i=0;i<axisSize;i++)
        {
            uchar *ptr=mask.ptr<uchar>(srcData.rows/2);
            if(ptr[i]!=0)
            {
                temp.push_back(srcData.at<double>(ui->NumSpinbox->value(),i));
            }
        }
    }
        break;
    default:
        break;
    }


    double minvalue=1000.0,maxvalue=0.0;
    for(int i=0;i<temp.size();i++)
    {
        if(minvalue>temp[i])
            minvalue=temp[i];
        if(maxvalue<temp[i])
            maxvalue=temp[i];
    }
    double demominator = maxvalue - minvalue;
//    for(int i=0;i<temp.size();i++)
//    {
//        temp[i] = (temp[i]-minvalue)*ui->YvalueEdit->value()/demominator;
//    }

    std::cout<<"the vector size is : "<<temp.size()<<" ; The min value is : "<<minvalue<<" ; The max value is : "<<maxvalue<<std::endl;

    QFont font;
    font.setFamily("Microsoft YaHei");
    // 大小
    font.setPointSize(13);
    font.setBold(true);
//    // 斜体
//    font.setItalic(true);
//    // 设置下划线
//    font.setUnderline(true);
//    // 设置上划线
//    font.setOverline(true);
    // 设置字母大小写
//    font.setCapitalization(QFont::SmallCaps);
//    // 设置字符间距
//    font.setLetterSpacing(QFont::AbsoluteSpacing, 20);

    // 使用字体
    painter.setFont(font);

    //绘制刻度线
    QPen penScale;
    penScale.setColor(Qt::black);
    penScale.setWidth(2);
    painter.setPen(penScale);

    double X_step=(double)temp.size()/10;
    painter.drawText(axis_X-10,axis_Y+15,QString::number(0));
    for(int i=0;i<10;i++)
    {
        painter.drawLine(axis_X+(i+1)*width/10,axis_Y,axis_X+(i+1)*width/10,axis_Y-5);
        painter.drawLine(axis_X+(i+1)*width/10,axis_Y-height,axis_X+(i+1)*width/10,axis_Y-height+5);
        painter.drawText(axis_X+(i+0.4)*width/10,axis_Y+15,QString::number((int)((i+1)*X_step)));

    }
    int Yaxle_value=0;
    if(maxvalue>10 && maxvalue<50)
    {
        Yaxle_value=35;
    }
    else if(maxvalue>50)
    {
        Yaxle_value=300;
    }
    else if(maxvalue<=1)
    {
        Yaxle_value=1;
    }
    else
    {
        Yaxle_value=10;
    }
    //Yaxle_value=ui->YvalueEdit->value();
    double Y_num= 10;
    //double Y_step=(double)(20)/Y_num;
    Yaxle_value = ceil(maxvalue);
    double Y_step=(double)(Yaxle_value)/Y_num;

    painter.drawText(axis_X+width/2-50,axis_Y+40,tr("Position/pixel"));
    QTransform transform;
    transform.rotate(-90);
    painter.setWorldTransform(transform);
    painter.drawText(-(50+5+height/2+30),20,tr("Phase/rad"));
    transform.rotate(-270);
    painter.setWorldTransform(transform);

//    penScale.setColor(Qt::red);
//    penScale.setStyle(Qt::DotLine);
//    painter.setPen(penScale);
//    painter.drawLine(axis_X+width-150,axis_Y-height/Y_num*9.5,axis_X+width-120,axis_Y-height/Y_num*9.5);
//    painter.drawText(axis_X+width-110,axis_Y-height/Y_num*9.2,tr("Sin"));
//    penScale.setColor(Qt::green);
//    penScale.setStyle(Qt::SolidLine);
//    painter.setPen(penScale);
//    painter.drawLine(axis_X+width-150,axis_Y-height/Y_num*8.5,axis_X+width-120,axis_Y-height/Y_num*8.5);
//    painter.drawText(axis_X+width-110,axis_Y-height/Y_num*8.2,tr("Main_Stair"));
//    penScale.setColor(Qt::blue);
//    penScale.setStyle(Qt::DashLine);
//    painter.setPen(penScale);
//    painter.drawLine(axis_X+width-150,axis_Y-height/Y_num*7.5,axis_X+width-120,axis_Y-height/Y_num*7.5);
//    painter.drawText(axis_X+width-110,axis_Y-height/Y_num*7.2,tr("Sub_Stair"));

    penScale.setColor(Qt::black);
    penScale.setStyle(Qt::SolidLine);
    painter.setPen(penScale);

    QPen penHorizontal(Qt::black,1,Qt::DotLine,Qt::RoundCap);
    for(int j=0;j<Y_num;j++)
    {
        painter.setPen(penScale);
        painter.drawLine(axis_X,axis_Y-(j+1)*height/Y_num,axis_X+5,axis_Y-(j+1)*height/Y_num);
        painter.drawLine(axis_X+width,axis_Y-(j+1)*height/Y_num,axis_X+width-5,axis_Y-(j+1)*height/Y_num);
        painter.drawText(axis_X-30,axis_Y-(j+0.8)*height/Y_num,QString::number((Y_step*(j+1))));
        painter.setPen(penHorizontal);
        //painter.drawLine(axis_X,axis_Y-(j+1)*height/Y_num,axis_X+width,axis_Y-(j+1)*height/Y_num);
    }

    //绘制图案

    QPen penPoint;
    QPen penPhase;
    if(ui->color_cBox->currentIndex()==0){
        penPhase.setColor(Qt::red);
         penPhase.setWidth(2);
    }
    if(ui->color_cBox->currentIndex()==1){
        penPhase.setColor(Qt::green);
        penPhase.setWidth(3);
    }
    if(ui->color_cBox->currentIndex()==2){
        penPhase.setColor(Qt::blue);
        penPhase.setWidth(3);
        }

    if(ui->dot_line_cBox->currentIndex()==0)penPhase.setStyle(Qt::SolidLine);
    if(ui->dot_line_cBox->currentIndex()==1)penPhase.setStyle(Qt::DashLine);
    if(ui->dot_line_cBox->currentIndex()==2)penPhase.setStyle(Qt::DotLine);


    penPoint.setColor(Qt::red);
    penPoint.setWidth(2);
    double kx=(double)width/temp.size();
    //double ky=(double)height/20;
    double ky=(double)height/Yaxle_value;
    int n=0;
    while(n!=(temp.size()-1))
    {
        if(ui->dot_line_cBox->currentIndex()!=3)
        {
            painter.setPen(penPhase);
            painter.drawLine(axis_X+n*kx,axis_Y-temp[n]*ky,axis_X+(n+1)*kx,axis_Y-temp[n+1]*ky);
        }else{
            painter.setPen(penPoint);
            painter.drawPoint(axis_X+n*kx,axis_Y-temp[n]*ky);
        }
        n++;
    }

    update();

}

void paintDialog::value_Paint(std::vector<double> &src)
{

    if(src.empty())
    {
        std::cout<<"\nthe vector filled with 'double' value is empty ... \n";
        return;
    }
    QPainter painter(pixmap);
    painter.setRenderHint(QPainter::Antialiasing,true);
    int width=450,height=300; //坐标轴宽度、高度
    int axis_X=50+5,axis_Y=height+50+5;  //坐标轴原点坐标
    painter.drawRect(axis_X,axis_Y-height,width,height);

    double minvalue=1000.0,maxvalue=0.0;

    for(int i=0;i<src.size();++i)
    {
        if(minvalue>abs(src[i]))
            minvalue=abs(src[i]);
        if(maxvalue<abs(src[i]))
            maxvalue=abs(src[i]);
    }
    std::cout<<"the vector size is : "<<src.size()<<" ; The min value is : "<<minvalue<<" ; The max value is : "<<maxvalue<<std::endl;

    QFont font;
    font.setFamily("Microsoft YaHei");
    // 大小
    font.setPointSize(12);
    font.setBold(true);
    // 使用字体
    painter.setFont(font);
    //绘制刻度线
    QPen penScale;
    penScale.setColor(Qt::black);
    penScale.setWidth(2);
    painter.setPen(penScale);


    double X_step=(double)2000/10;
    for(int i=0;i<10;i++)
    {
        painter.drawLine(axis_X+(i+1)*width/10,axis_Y,axis_X+(i+1)*width/10,axis_Y-5);
        painter.drawLine(axis_X+(i+1)*width/10,axis_Y-height,axis_X+(i+1)*width/10,axis_Y-height+5);
        painter.drawText(axis_X+(i+0.4)*width/10,axis_Y+15,QString::number((int)((i+1)*X_step)));

    }


    double Yaxle_value=1;

    if(maxvalue>10 && maxvalue<50)
    {
        Yaxle_value=50;
    }
    else if(maxvalue>50)
    {
        Yaxle_value=300;
    }
    else if(maxvalue<=1)
    {
        Yaxle_value=0.5;
    }
    else if(maxvalue<3 && maxvalue>1)
    {
        Yaxle_value=3;
    }else{
        Yaxle_value=10;
    }

    double Y_step=(double)(Yaxle_value)/5;
    QPen penHorizontal(Qt::black,1,Qt::DotLine,Qt::RoundCap);

    //设置坐标轴名
    painter.drawText(axis_X+width/2-20,axis_Y+30,tr("X (pixel)"));
    QTransform transform;
    transform.rotate(-90);
    painter.setWorldTransform(transform);
    painter.drawText(-(50+5+height/2+30),15,tr("Intensity"));
    transform.rotate(-270);
    painter.setWorldTransform(transform);

    for(int j=0;j<5;j++)
    {      
        painter.drawLine(axis_X,axis_Y-(j+1)*height/10,axis_X+5,axis_Y-(j+1)*height/10);
        painter.drawText(axis_X-35,axis_Y-(j+1)*height/10,QString::number(Y_step*(-(4-j) ) ) );
        painter.setPen(penHorizontal);
        painter.drawLine(axis_X,axis_Y-(j+1)*height/10,axis_X+width,axis_Y-(j+1)*height/10);
    }
    for(int j=5;j<10;j++)
    {
        painter.drawLine(axis_X,axis_Y-(j+1)*height/10,axis_X+5,axis_Y-(j+1)*height/10);
        painter.drawText(axis_X-35,axis_Y-(j+1)*height/10,QString::number(Y_step*(j-4 ) ) );
        painter.setPen(penHorizontal);
        painter.drawLine(axis_X,axis_Y-(j+1)*height/10,axis_X+width,axis_Y-(j+1)*height/10);
    }

    //绘制图案

    QPen penPoint;
    QPen penPhase;
    if(ui->color_cBox->currentIndex()==0)penPhase.setColor(Qt::red);
    if(ui->color_cBox->currentIndex()==1)penPhase.setColor(Qt::green);
    if(ui->color_cBox->currentIndex()==2)penPhase.setColor(Qt::blue);

    if(ui->color_cBox->currentIndex()==0)penPoint.setColor(Qt::red);
    if(ui->color_cBox->currentIndex()==1)penPoint.setColor(Qt::green);
    if(ui->color_cBox->currentIndex()==2)penPoint.setColor(Qt::blue);

    penPhase.setWidth(1);
    penPoint.setWidth(2);
    double kx=(double)width/src.size();
    double ky=(double)height/(2*Yaxle_value);
    int n=0;
    while(n!=(src.size()-1))
    {
        if(ui->dot_line_cBox->currentIndex()==0)
        {
            painter.setPen(penPhase);
            painter.drawLine(axis_X+n*kx,axis_Y-height/2-src[n]*ky,axis_X+(n+1)*kx,axis_Y-height/2-src[n+1]*ky);
        }else{
            painter.setPen(penPoint);
            painter.drawPoint(axis_X+n*kx,axis_Y-height/2-src[n]*ky);
        }
        n++;
    }

    update();


}

void paintDialog::pointXY_Paint(std::vector<pointXY> &src)
{

    if(src.empty())
    {
        std::cout<<"\nthe vector filled with 'double' value is empty ... \n";
        return;
    }
    QPainter painter(pixmap);
    painter.setRenderHint(QPainter::Antialiasing,true);

    int width=450,height=300; //坐标轴宽度、高度
    int axis_X=50+5,axis_Y=height+50+5;  //坐标轴原点坐标
    painter.drawRect(axis_X,axis_Y-height,width,height);

    double x_minvalue=1000.0,x_maxvalue=0.0;
    double y_minvalue=1000.0,y_maxvalue=0.0;

    for(int i=0;i<src.size();++i)
    {
        if(x_minvalue>abs(src[i].x))
            x_minvalue=abs(src[i].x);
        if(x_maxvalue<abs(src[i].x))
            x_maxvalue=abs(src[i].x);

        if(y_minvalue>abs(src[i].y))
            y_minvalue=abs(src[i].y);
        if(y_maxvalue<abs(src[i].y))
            y_maxvalue=abs(src[i].y);
    }
    std::cout<<"the vector size is : "<<src.size()<<" ; The Xmin value is : "<<x_minvalue<<" ; The Xmax value is : "<<x_maxvalue<<std::endl;
    std::cout<<"the vector size is : "<<src.size()<<" ; The Ymin value is : "<<y_minvalue<<" ; The Ymax value is : "<<y_maxvalue<<std::endl;

    QFont font;
    font.setFamily("Microsoft YaHei");
    // 大小
    font.setPointSize(13);
    font.setBold(true);
    // 使用字体
    painter.setFont(font);
    //绘制刻度线
    QPen penScale;
    penScale.setColor(Qt::black);
    penScale.setWidth(2);
    painter.setPen(penScale);

    double Xaxle_value=300;
    double X_step=(double)Xaxle_value/10;
    for(int i=0;i<10;i++)
    {
        painter.drawLine(axis_X+(i+1)*width/10,axis_Y,axis_X+(i+1)*width/10,axis_Y-5);
        painter.drawLine(axis_X+(i+1)*width/10,axis_Y-height,axis_X+(i+1)*width/10,axis_Y-height+5);
        painter.drawText(axis_X+(i+0.4)*width/10,axis_Y+15,QString::number((int)((i+1)*X_step)));

    }


    double Yaxle_value=300;

//    if(y_maxvalue>10)
//    {
//        Yaxle_value=300;
//    }
//    else if(y_maxvalue<=1 && y_maxvalue>0.5)
//    {
//        Yaxle_value=1;
//    }
//    else if(y_maxvalue<=0.5)
//    {
//        Yaxle_value=y_maxvalue*2;
//    }else{
//        Yaxle_value=10;
//    }

    double Y_step=(double)(Yaxle_value)/10;
    QPen penHorizontal(Qt::black,1,Qt::DotLine,Qt::RoundCap);

    //设置坐标轴名
    painter.drawText(axis_X+width/2-30,axis_Y+35,tr("Actual Intensity"));
    QTransform transform;
    transform.rotate(-90);
    painter.setWorldTransform(transform);
    painter.drawText(-(50+5+height/2+30),15,tr("Real Intensity"));
    transform.rotate(-270);
    painter.setWorldTransform(transform);

//    for(int j=0;j<5;j++)
//    {
//        painter.drawLine(axis_X,axis_Y-(j+1)*height/10,axis_X+5,axis_Y-(j+1)*height/10);
//        painter.drawText(axis_X-35,axis_Y-(j+1)*height/10,QString::number(Y_step*(-(4-j) ) ) );
//        painter.setPen(penHorizontal);
//        painter.drawLine(axis_X,axis_Y-(j+1)*height/10,axis_X+width,axis_Y-(j+1)*height/10);
//    }
//    for(int j=5;j<10;j++)
//    {
//        painter.drawLine(axis_X,axis_Y-(j+1)*height/10,axis_X+5,axis_Y-(j+1)*height/10);
//        painter.drawText(axis_X-35,axis_Y-(j+1)*height/10,QString::number(Y_step*(j-4 ) ) );
//        painter.setPen(penHorizontal);
//        painter.drawLine(axis_X,axis_Y-(j+1)*height/10,axis_X+width,axis_Y-(j+1)*height/10);
//    }
    int Y_num = 10;
    for(int j=0;j<Y_num;j++)
    {
        painter.setPen(penScale);
        painter.drawLine(axis_X,axis_Y-(j+1)*height/Y_num,axis_X+5,axis_Y-(j+1)*height/Y_num);
        painter.drawLine(axis_X+width,axis_Y-(j+1)*height/Y_num,axis_X+width-5,axis_Y-(j+1)*height/Y_num);
        painter.drawText(axis_X-30,axis_Y-(j+0.8)*height/Y_num,QString::number((Y_step*(j+1))));
        painter.setPen(penHorizontal);
        //painter.drawLine(axis_X,axis_Y-(j+1)*height/Y_num,axis_X+width,axis_Y-(j+1)*height/Y_num);
    }

    penScale.setColor(Qt::red);
    penScale.setStyle(Qt::DotLine);
    painter.setPen(penScale);

    painter.drawLine(axis_X+width-150,axis_Y-height/Y_num*7+30,axis_X+width-120,axis_Y-height/Y_num*7+30);
    painter.drawText(axis_X+width-110,axis_Y-height/Y_num*6.8+30,tr("Data Point"));
    penScale.setColor(Qt::green);
    penScale.setStyle(Qt::SolidLine);
    painter.setPen(penScale);
    painter.drawLine(axis_X+width-150,axis_Y-height/Y_num*6+30,axis_X+width-120,axis_Y-height/Y_num*6+30);
    painter.drawText(axis_X+width-110,axis_Y-height/Y_num*5.8+30,tr("F"));
    penScale.setColor(Qt::blue);
    penScale.setStyle(Qt::SolidLine);
    painter.setPen(penScale);
    painter.drawLine(axis_X+width-150,axis_Y-height/Y_num*5+30,axis_X+width-120,axis_Y-height/Y_num*5+30);
    painter.drawText(axis_X+width-110,axis_Y-height/Y_num*4.8+30,tr("F"));
    font.setPointSize(6);
    painter.drawText(axis_X+width-102,axis_Y-height/Y_num*4.6+30,tr("inverse"));
    font.setPointSize(13);
    //绘制图案

    QPen penPoint;
    QPen penPhase;
    if(ui->color_cBox->currentIndex()==0)penPhase.setColor(Qt::red);
    if(ui->color_cBox->currentIndex()==1)penPhase.setColor(Qt::green);
    if(ui->color_cBox->currentIndex()==2)penPhase.setColor(Qt::blue);

    if(ui->dot_line_cBox->currentIndex()==0)penPhase.setStyle(Qt::SolidLine);
    if(ui->dot_line_cBox->currentIndex()==1)penPhase.setStyle(Qt::DashLine);
    if(ui->dot_line_cBox->currentIndex()==2)penPhase.setStyle(Qt::DotLine);

    penPhase.setWidth(2);
    penPoint.setColor(Qt::red);
    penPoint.setWidth(5);
    double kx=(double)width/(Xaxle_value);
    double ky=(double)height/(Yaxle_value);
    int n=0;
    while(n!=(src.size()-1))
    {
        if(ui->dot_line_cBox->currentIndex()!=3)
        {
            painter.setPen(penPhase);
            if(src[n].x - src[n+1].x < X_step )
            painter.drawLine(axis_X+src[n].x * kx,axis_Y-src[n].y*ky,axis_X+src[n+1].x *kx,axis_Y-src[n+1].y*ky);
        }else{
            painter.setPen(penPoint);
            painter.drawPoint(axis_X+src[n].x *kx,axis_Y-src[n].y *ky);

        }
        n++;
    }

    update();
}

void paintDialog::on_Red_Btn_clicked()
{
    int num=ui->numSelect_spinBox->value();

    std::vector<cv::Mat>::iterator pt =InputData.end()-1;
    cv::Mat maskimage=*pt;

    crosstalk_Paint(InputData[num],red,maskimage);
}

void paintDialog::on_Blue_Btn_clicked()
{
    int num=ui->numSelect_spinBox->value();

    std::vector<cv::Mat>::iterator pt =InputData.end()-1;
    cv::Mat maskimage=*pt;

    crosstalk_Paint(InputData[num],blue,maskimage);
}

void paintDialog::on_Green_Btn_clicked()
{
    int num=ui->numSelect_spinBox->value();

    std::vector<cv::Mat>::iterator pt =InputData.end()-1;
    cv::Mat maskimage=*pt;

    crosstalk_Paint(InputData[num],green,maskimage);
}

void paintDialog::on_clear_Btn_clicked()
{
    pixmap->fill(Qt::white);
    update();
}

void paintDialog::on_xPhase_Btn_clicked()
{
    int num=ui->numSelect_spinBox->value();

    std::vector<cv::Mat>::iterator pt =InputData.end()-1;
    cv::Mat maskimage=*pt;

    if(InputData[num].channels()!=1)
    {
        std::cout<<"\nError: the channel of phase pattern is not correct ... \n";
    }else{

        InputData[num].convertTo(InputData[num],CV_64FC1);
        phase_Paint(InputData[num],horizontal,maskimage);
    }

}

void paintDialog::on_yPhase_Btn_clicked()
{
    int num=ui->numSelect_spinBox->value();

    std::vector<cv::Mat>::iterator pt =InputData.end()-1;
    cv::Mat maskimage=*pt;

    if(InputData[num].channels()!=1)
    {
        std::cout<<"\nError: the channel of phase pattern is not correct ... \n";
    }else{

        InputData[num].convertTo(InputData[num],CV_64FC1);
        phase_Paint(InputData[num],vertical,maskimage);
    }
}

void paintDialog::on_valueBtn_clicked()
{
    int num=ui->numSelect_spinBox->value();

    if(num>=valueData.size())
    {
        std::cout<<"\nError: the number is over the size of valueData ... \n";
        return;
    }

    value_Paint(valueData[num]);
}


void paintDialog::getData(std::vector<cv::Mat> &Input)
{
    InputData=Input;
//    ui->numSelect_spinBox->setRange(0,Input.size()-2);
//    ui->numSelect_spinBox->setWrapping(true);
}

void paintDialog::on_XY_Btn_clicked()
{
    int num=ui->numSelect_spinBox->value();

    if(num>=pointXYData.size())
    {
        std::cout<<"\nError: the number is over the size of pointXYData ... \n";
        return;
    }

    pointXY_Paint(pointXYData[num]);
}
