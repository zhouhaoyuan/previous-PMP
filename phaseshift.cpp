#include "phaseshift.h"

PhaseShift::PhaseShift()
{
     wavelength_P1= 52;
     wavelength_P2= 58;
     wavelength_P3= 620;
}


void PhaseShift::setThreeFrequenceWavelength(float p1,float p2,float p3)
{
    wavelength_P1= p1;
    wavelength_P2= p2;
    wavelength_P3= p3;
}


void PhaseShift::unwrapThreeFrequencyHeterodyne(std::vector<cv::Mat> wrap_list,cv::Mat& unwrap_phase)
{

    cv::Mat image_1= wrap_list.at(0);
    cv::Mat image_2= wrap_list.at(1);
    cv::Mat image_3= wrap_list.at(2);


    int nr= image_1.rows;
    int nc= image_1.cols;


    cv::Mat unwrap_comp_final_k1(nr,nc,CV_64F);

    //补偿界限
    float compensateBoundary= PI/2;

    float p1= wavelength_P1;
    float p2= wavelength_P2;
    float p3= wavelength_P3;

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

void PhaseShift::phaseNormalization(float rate, cv::Mat unwrap_phase, cv::Mat &norm_phase)
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

void PhaseShift::unwarpMultiplyHierarchical(float rate, std::vector<cv::Mat> wrap_list, cv::Mat& unwrap_phase, std::vector<cv::Mat> &unwrap_phase_list)
{
    std::vector<cv::Mat> unwrap_list;
    unwrap_list.push_back(wrap_list.at(0));



    for(int i= 0;i< wrap_list.size()-1;i++)
       {

           cv::Mat l_unwrap= unwrap_list.at(i);
           cv::Mat h_wrap= wrap_list.at(i+1);


           int nr= l_unwrap.rows;
           int nc= l_unwrap.cols;


           cv::Mat h_unwrap(nr,nc,CV_64F);
           cv::Mat k_Mat(nr,nc,CV_8U);


           unwarpHierarchical(rate,l_unwrap,h_wrap,h_unwrap,k_Mat);

           unwrap_list.push_back(h_unwrap);


       }

    unwrap_phase= unwrap_list.at(wrap_list.size()-1);
    unwrap_phase_list= unwrap_list;
}


void PhaseShift::unwrap_MP_MW(std::vector<cv::Mat> wrap_list, cv::Mat& unwrap_phase)
{
    cv::Mat start_wrap_x= wrap_list.at(0);

    cv::Mat m_wrap_x;
    cv::Mat f_wrap_x;
    cv::Mat n_mat;


    int nr= start_wrap_x.rows;
    int nc= start_wrap_x.cols;

    cv::Mat k_mat(nr,nc,CV_8U);

    unwarpHierarchical(std::pow(2.0,4),start_wrap_x,wrap_list.at(4),m_wrap_x,k_mat);


    unwarpHierarchical(4,m_wrap_x,wrap_list.at(6),f_wrap_x,k_mat);

    phaseNormalization(std::pow(2.0,6),f_wrap_x,n_mat);

    unwrap_phase= n_mat.clone();

}

void PhaseShift::unwarpMultiplyHierarchical(float rate, std::vector<cv::Mat> wrap_list, cv::Mat& unwrap_phase)
{
    std::vector<cv::Mat> unwrap_list;
    unwrap_list.push_back(wrap_list.at(0));



    for(int i= 0;i< wrap_list.size()-1;i++)
       {

           cv::Mat l_unwrap= unwrap_list.at(i);
           cv::Mat h_wrap= wrap_list.at(i+1);


           int nr= l_unwrap.rows;
           int nc= l_unwrap.cols;


           cv::Mat h_unwrap(nr,nc,CV_64F);
           cv::Mat k_Mat(nr,nc,CV_64F);


           unwarpHierarchical(rate,l_unwrap,h_wrap,h_unwrap,k_Mat);



           unwrap_list.push_back(h_unwrap);


       }

    unwrap_phase= unwrap_list.at(wrap_list.size()-1);
}


void PhaseShift::multipleWavelengthUnwrapFloat(std::vector<cv::Mat> setImage,cv::Mat& resultMat)
{
    std::vector<cv::Mat> unwrapImage;

    cv::Mat firstFringe= setImage.at(0);

    unwrapImage.push_back(firstFringe);

    for(int i=0; i<setImage.size()-1;i++)
    {
        cv::Mat unwrapMat(firstFringe.rows,firstFringe.cols,CV_64F,cv::Scalar(0));

        cv::Mat lastUnwrapMat= unwrapImage.at(i);

        cv::Mat wrapMat= setImage.at(i+1);

        cv::Mat k_value(firstFringe.rows,firstFringe.cols,CV_64F);

        double temp= 0;

        int k= 0;



//        for(int c= 0;c< firstFringe.cols;c++)
//        {
//            for(int r= 0;r< firstFringe.rows;r++)
//            {

//                temp= 0.5+ lastUnwrapMat.at<double>(r,c)/(0.5*PI) - wrapMat.at<double>(r,c)/(PI);

//                k= temp;


//                unwrapMat.at<double>(r,c)= k*PI + wrapMat.at<double>(r,c) ;


//                k_value.at<double>(r,c)= k;



//            }

//        }


        unwarpDualWavelength(lastUnwrapMat,wrapMat,unwrapMat,k_value);


        unwrapImage.push_back(unwrapMat);
    }

//    cv::Mat finalUnwrapMat(firstFringe.rows,firstFringe.cols,CV_32F,cv::Scalar(0));

//    finalUnwrapMat= unwrapImage.at(setImage.size()-1);

    resultMat= unwrapImage.at(setImage.size()-1);

}

void PhaseShift::unwrapMultiplyWavelength(std::vector<cv::Mat> wrap_list, cv::Mat& unwrap_phase,
                              std::vector<cv::Mat> &unwrap_phase_list)
{
    std::vector<cv::Mat> unwrap_list;

    cv::Mat first_unwrap= wrap_list.at(0);

    int nr= first_unwrap.rows;
    int nc= first_unwrap.cols;

    unwrap_list.push_back(first_unwrap);

    for(int i=0; i<wrap_list.size()-1;i++)
    {
        cv::Mat unwrapMat(nr,nc,CV_64F);

        cv::Mat lastUnwrapMat= unwrap_list.at(i);

        cv::Mat wrapMat= wrap_list.at(i+1);

        cv::Mat k_value(nr,nc,CV_64F);

//        unwarpDualWavelength(lastUnwrapMat,wrapMat,unwrapMat,k_value);

        unwarpHierarchical(2.0,lastUnwrapMat,wrapMat,unwrapMat,k_value);


        unwrap_list.push_back(unwrapMat);


    }

    unwrap_phase= unwrap_list.at(wrap_list.size()-1);
    unwrap_phase_list= unwrap_list;


}

void PhaseShift::unwarpDualWavelength(cv::Mat l_unwrap, cv::Mat h_wrap, cv::Mat &h_unwrap, cv::Mat &k_Mat)
{


    int nr= l_unwrap.rows;
    int nc= l_unwrap.cols;


    if(l_unwrap.isContinuous())
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
        double* l_ptr= l_unwrap.ptr<double>(i);
        double* h_ptr= h_wrap.ptr<double>(i);
        uchar* k_ptr= k_Mat.ptr<uchar>(i);

        double* h_unwrap_ptr= h_unwrap.ptr<double>(i);
        for(int j= 0;j< nc;j++)
        {

            double temp= 0.5+ l_ptr[j]/(0.5*PI)- h_ptr[j]/PI;

            int k= temp;

            k_ptr[j]= k;

            h_unwrap_ptr[j]= PI*k+ h_ptr[j];



        }
    }


}

void PhaseShift::unwarpHierarchical(float rate, cv::Mat const l_wrap, cv::Mat h_wrap, cv::Mat &h_unwrap, cv::Mat &k_Mat)
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


void PhaseShift::fourStepPhaseShift(std::vector<cv::Mat> inMats,cv::Mat& outMat)
{

    Mat img1= inMats.at(0);
    Mat img2= inMats.at(1);
    Mat img3= inMats.at(2);
    Mat img4= inMats.at(3);


    cv::Mat result(img1.rows,img1.cols,CV_64F);

    int nl= img1.rows;
    int nc= img1.cols* img1.channels();

    if(img1.isContinuous())
    {
        if(img2.isContinuous())
        {
            if(img3.isContinuous())
            {
                if(img4.isContinuous())
                {
                    nc= nc*nl;
                    nl= 1;
                }
            }
        }

    }


    for(int i= 0;i< nl;i++)
    {
        uchar* ptr1= img1.ptr<uchar>(i);
        uchar* ptr2= img2.ptr<uchar>(i);
        uchar* ptr3= img3.ptr<uchar>(i);
        uchar* ptr4= img4.ptr<uchar>(i);

        double* optr= result.ptr<double>(i);
        for(int j= 0;j< nc;j++)
        {

            double value;

            value= (double)(ptr4[j]- ptr2[j])/(ptr1[j]- ptr3[j]);

            optr[j]= PI/2.0 + atan(value);

        }
    }


    /*****************************************************************************************************************************/


    outMat= result.clone();
}
