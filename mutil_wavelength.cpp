#include "mutil_wavelength.h"


/** @brief  Contains all DLP SDK classes, functions, etc. */
namespace dlp{

/** @brief Constructs object */
mutil_wavelength::mutil_wavelength()
{
}

/** @brief Retrieves settings from \ref dlp::Parameters object to configure
 *  \ref dlp::Pattern::Sequence generation and \ref dlp::Capture::Sequence decoding
 *  @param[in]  settings    \ref dlp::Parameters object to retrieve settings from
 *
 * @retval STRUCTURED_LIGHT_SETTINGS_PATTERN_COLOR_MISSING              \ref dlp::Parameters list missing \ref dlp::StructuredLight::pattern_color_
 * @retval STRUCTURED_LIGHT_SETTINGS_PATTERN_ROWS_MISSING               \ref dlp::Parameters list missing \ref dlp::StructuredLight::pattern_rows_
 * @retval STRUCTURED_LIGHT_SETTINGS_PATTERN_COLUMNS_MISSING            \ref dlp::Parameters list missing \ref dlp::StructuredLight::pattern_columns_
 * @retval STRUCTURED_LIGHT_SETTINGS_PATTERN_ORIENTATION_MISSING        \ref dlp::Parameters list missing \ref dlp::StructuredLight::pattern_orientation_
 */
ReturnCode mutil_wavelength::Setup(const dlp::Parameters &settings){
    ReturnCode ret;

    if(!this->projector_set_){
        if(settings.Get(&this->pattern_rows_).hasErrors())
            return ret.AddError(STRUCTURED_LIGHT_SETTINGS_PATTERN_ROWS_MISSING);

       if((settings.Get(&this->pattern_columns_).hasErrors()))
            return ret.AddError(STRUCTURED_LIGHT_SETTINGS_PATTERN_COLUMNS_MISSING);
    }

//    ret = settings.Get(&this->over_sample_);
//    if(this->over_sample_.Get() < 1) this->over_sample_.Set(1);


    ret = settings.Get(&this->pattern_orientation_);
    if(ret.hasErrors())
        return ret.AddError(STRUCTURED_LIGHT_SETTINGS_PATTERN_ORIENTATION_MISSING);

    ret = settings.Get(&this->pattern_color_);
    if(ret.hasErrors())
        return ret.AddError(STRUCTURED_LIGHT_SETTINGS_PATTERN_COLOR_MISSING);

    ret = settings.Get(&this->bitdepth_);
    if(ret.hasErrors())
        return ret.AddError(MUTIL_WAVELENGTH_BITDEPTH_MISSING);

    switch(this->bitdepth_.Get()){
        case dlp::Pattern::Bitdepth::MONO_8BPP:
            this->maximum_value_ = 255;
            break;
        case dlp::Pattern::Bitdepth::MONO_7BPP:
            this->maximum_value_ = 127;
            break;
        case dlp::Pattern::Bitdepth::MONO_6BPP:
            this->maximum_value_ = 63;
            break;
        case dlp::Pattern::Bitdepth::MONO_5BPP:
            this->maximum_value_ = 31;
            break;
        default:
            return ret.AddError(MUTIL_WAVELENGTH_BITDEPTH_TOO_SMALL);
    }

    switch(this->pattern_orientation_.Get()){
    case dlp::Pattern::Orientation::VERTICAL:
        this->resolution_ = this->pattern_columns_.Get();
        break;
    case dlp::Pattern::Orientation::HORIZONTAL:
        this->resolution_ = this->pattern_rows_.Get();
        break;
//    case dlp::Pattern::Orientation::DIAMOND_ANGLE_1:
//    case dlp::Pattern::Orientation::DIAMOND_ANGLE_2:
//        this->resolution_ = this->pattern_columns_.Get() +
//                            (this->pattern_rows_.Get()/2);
//        break;
    case dlp::Pattern::Orientation::INVALID:
    default:
        return ret.AddError(STRUCTURED_LIGHT_NOT_SETUP);
        break;
    }

    ret = settings.Get(&this->pixels_per_period_);
    if(ret.hasErrors())
        return ret.AddError(MUTIL_WAVELENGTH_PIXELS_PER_PERIOD_MISSING);

    this->frequency_.Set( (float) this->resolution_ / this->pixels_per_period_.Get());
    this->phase_counts_ = this->frequency_.Get() * 2;

    // Store the number of four phase patterns
    this->sequence_count_total_ = 4;


    ret = settings.Get(&this->sequence_count_);
    if(ret.hasErrors())
        return ret.AddError(MUTIL_WAVELENGTH_SEQUENCE_COUNT_MISSING);

    if(settings.Contains(this->repeat_phases_))
        settings.Get(&this->repeat_phases_);




    // Setup has been completed
    this->is_setup_ = true;

    return ret;
}

void mutil_wavelength::Generate_sin_ladder_Pattern(std::vector<cv::Mat> &PatternSequence)
{
    int rowNumber=1140;
    int colNumber=912;
    int pixels_per_period=20;
    const double PI=3.1415926;
    int segmented_ladderNum=6;
    int H_ladderNum=std::ceil((float)rowNumber/(segmented_ladderNum*pixels_per_period));
    int V_ladderNum=std::ceil((float)colNumber/(segmented_ladderNum*pixels_per_period));
        for(int n=0;n<4;n++)
        {
            cv::Mat sinusoidal_pattern(rowNumber, colNumber , CV_8UC1);
            for(int i=0;i<rowNumber;i++)
            {
                for(int j=0;j<colNumber;j++)
                {
                    sinusoidal_pattern.at<uchar>(i,j)=128+127*cos((i+1)*2*PI/pixels_per_period+n*PI/2);
                }
            }
            PatternSequence.push_back(sinusoidal_pattern);
        }
        cv::Mat H_ladder_phase(rowNumber, colNumber, CV_32FC1, cv::Scalar(0));
        for(int i=0;i<rowNumber;i++)
        {
            for(int j=0;j<colNumber;j++)
            {
                H_ladder_phase.at<float>(i,j)=
                    -PI+2*PI/(H_ladderNum-1)*floor((float)i/pixels_per_period);
            }
        }
        for(int n=0;n<4;n++)
        {
            cv::Mat ladder_pattern(rowNumber, colNumber, CV_8UC1, cv::Scalar(0));
            for(int i=0;i<rowNumber;i++)
            {
                for(int j=0;j<colNumber;j++)
                {
                    ladder_pattern.at<uchar>(i,j)=
                            128+127*cos(H_ladder_phase.at<float>(i,j)+n*PI/2);
                }
            }
            PatternSequence.push_back(ladder_pattern);
        }
        cv::Mat H_segmented_ladder_phase(rowNumber, colNumber, CV_32FC1, cv::Scalar(0));
        for(int i=0;i<rowNumber;i++)
        {
            for(int j=0;j<colNumber;j++)
            {
                    H_segmented_ladder_phase.at<float>(i,j)=
                        -PI+2*PI/(segmented_ladderNum-1)*floor((float)(i%((segmented_ladderNum)*pixels_per_period))/pixels_per_period);
            }
        }
        for(int n=0;n<4;n++)
        {
            cv::Mat ladder_pattern(rowNumber, colNumber, CV_8UC1, cv::Scalar(0));
            for(int i=0;i<rowNumber;i++)
            {
                for(int j=0;j<colNumber;j++)
                {
                    ladder_pattern.at<uchar>(i,j)=
                            128+127*cos(H_segmented_ladder_phase.at<float>(i,j)+n*PI/2);
                }
            }
            PatternSequence.push_back(ladder_pattern);
        }
        for(int n=0;n<4;n++)
        {
            cv::Mat sinusoidal_pattern(rowNumber, colNumber, CV_8UC1);
            for(int i=0;i<rowNumber;i++)
            {
                for(int j=0;j<colNumber;j++)
                {
                    sinusoidal_pattern.at<uchar>(i,j)=128+127*cos(2*PI*(j+1)/pixels_per_period+n*PI/2);
                }
            }
            PatternSequence.push_back(sinusoidal_pattern);
        }


        cv::Mat V_ladder_phase(rowNumber, colNumber, CV_32FC1, cv::Scalar(0));
        for(int i=0;i<rowNumber;i++)
        {
            for(int j=0;j<colNumber;j++)
            {
                V_ladder_phase.at<float>(i,j)=
                    -PI+2*PI/(V_ladderNum-1)*floor((float)j/pixels_per_period);
            }
        }
        for(int n=0;n<4;n++)
        {
            cv::Mat ladder_pattern(rowNumber, colNumber, CV_8UC1, cv::Scalar(0));
            for(int i=0;i<rowNumber;i++)
            {
                for(int j=0;j<colNumber;j++)
                {
                    ladder_pattern.at<uchar>(i,j)=
                            128+127*cos(V_ladder_phase.at<float>(i,j)+n*PI/2);
                }
            }
            PatternSequence.push_back(ladder_pattern);
        }

        //分段


    cv::Mat V_segmented_ladder_phase(rowNumber, colNumber, CV_32FC1, cv::Scalar(0));
        for(int i=0;i<rowNumber;i++)
        {
            for(int j=0;j<colNumber;j++)
            {
                    V_segmented_ladder_phase.at<float>(i,j)=
                        -PI+2*PI/(segmented_ladderNum-1)*floor((float)(j%((segmented_ladderNum)*pixels_per_period))/pixels_per_period);
            }
        }
        for(int n=0;n<4;n++)
        {
            cv::Mat ladder_pattern(rowNumber, colNumber, CV_8UC1, cv::Scalar(0));
            for(int i=0;i<rowNumber;i++)
            {
                for(int j=0;j<colNumber;j++)
                {
                    ladder_pattern.at<uchar>(i,j)=
                            128+127*cos(V_segmented_ladder_phase.at<float>(i,j)+n*PI/2);
                }
            }
            PatternSequence.push_back(ladder_pattern);
        }

}

void mutil_wavelength::createSinData(int move, int stepNum, float pixel, Pattern::Orientation orientation, cv::Mat &sin_data)
{
    unsigned int rows     = this->pattern_rows_.Get();
    unsigned int columns  = this->pattern_columns_.Get();

    cv::Mat input_img(rows,columns,CV_8U);

    float moveLength= 2*MULTI_WAVELENGTH_PI/stepNum;


    if(Pattern::Orientation::VERTICAL== orientation)
    {

        for(int i= 0;i< rows; i++)
        {
            for(int j= 0;j< columns; j += 1)
            {

                input_img.at<uchar>(i,j)= 128 +127*cos(j/pixel*2*MULTI_WAVELENGTH_PI + move*moveLength);

            }
        }

    }
    else
    {

        for(int i= 0;i< rows; i++)
        {
            for(int j= 0;j< columns; j += 1)
            {
                input_img.at<uchar>(i,j)= 128 +127*cos(i/pixel*2*MULTI_WAVELENGTH_PI + move*moveLength);

            }
        }


    }


    sin_data= input_img.clone();
}

//调用自己的函数，转换成dlp的格式
void mutil_wavelength::generatePhaseShiftPatterns(int stepNum,float wavelength,Pattern::Orientation orientation,Pattern::Sequence *pattern_sequence)
{
    unsigned int rows     = this->pattern_rows_.Get();
    unsigned int columns  = this->pattern_columns_.Get();

    std::vector<cv::Mat> mat_list;

    Generate_sin_ladder_Pattern(mat_list);   //调用自己的函数


    stepNum= mat_list.size();

    for(int i= 0;i< 0+stepNum;i++)//遍历转换
    {
        cv::Mat sin_data;
//        createSinData(i,stepNum,wavelength,orientation,sin_data);

        sin_data= mat_list.at(i);

        dlp::Image sine_phase_image;
        sine_phase_image.Create(columns, rows, dlp::Image::Format::MONO_UCHAR );

        for(     unsigned int yRow = 0; yRow < rows;    yRow++){
            for( unsigned int xCol = 0; xCol < columns; xCol++){
                sine_phase_image.Unsafe_SetPixel(    xCol, yRow, unsigned char( sin_data.at<uchar>(yRow,xCol)) );
            }
        }

        dlp::Pattern sine_phase_pattern;

        sine_phase_pattern.bitdepth  = this->bitdepth_.Get();
        sine_phase_pattern.color     = this->pattern_color_.Get();
        sine_phase_pattern.data_type = dlp::Pattern::DataType::IMAGE_DATA;
        sine_phase_pattern.image_data.Create(sine_phase_image);

        // Add the patterns to the return sequence
        for(unsigned int iCount = 0; iCount < this->repeat_phases_.Get();iCount++){
            pattern_sequence->Add(sine_phase_pattern);
        }

        sine_phase_image.Clear();
    }
}
//

/** @brief      Retrieves module settings
 *  @param[in]  settings Pointer to return settings
 *  @retval     STRUCTURED_LIGHT_NULL_POINTER_ARGUMENT  Input argument is NULL
 */
ReturnCode mutil_wavelength::GetSetup( dlp::Parameters *settings)const{
    ReturnCode ret;

    if(!settings)
        return ret.AddError(STRUCTURED_LIGHT_NULL_POINTER_ARGUMENT);

    settings->Set(this->pattern_rows_);
    settings->Set(this->pattern_columns_);
    settings->Set(this->pattern_color_);
    settings->Set(this->pattern_orientation_);
    settings->Set(this->sequence_count_);
    settings->Set(this->bitdepth_);

    return ret;
}



/** @brief Generates a \ref dlp::Pattern::Sequence based on the settings specified
 *  @param[out] pattern_sequence Return pointer to \ref dlp::Pattern::Sequence
 *  @retval STRUCTURED_LIGHT_NULL_POINTER_ARGUMENT  Return argument is NULL
 *  @retval STRUCTURED_LIGHT_NOT_SETUP              Module has NOT been setup
 *  @retval STRUCTURED_LIGHT_CAPTURE_SEQUENCE_SIZE_INVALID  Requested number of patterns is NOT possible with the specified DMD resolution
 */
ReturnCode mutil_wavelength:: GeneratePatternSequence(Pattern::Sequence *pattern_sequence){
    ReturnCode ret;

    // Check that Mutil Wavelength object is setup
    if((!this->isSetup()))
        return ret.AddError(STRUCTURED_LIGHT_NOT_SETUP);

    // Check that argument is not null
    if(!pattern_sequence)
        return ret.AddError(STRUCTURED_LIGHT_NULL_POINTER_ARGUMENT);



    // Clear the pattern sequence
    pattern_sequence->Clear();

    unsigned int max_count= this->sequence_count_.Get();


    unsigned int resolution_y     = this->pattern_rows_.Get();
    unsigned int resolution_x  = this->pattern_columns_.Get();


    /******************************************************************************************************************************/
    //generate multiply wavelength patterns
    //generate x sequence
    for(int i_sequence= 0;i_sequence< max_count;i_sequence++)
    {
        float period_pixels = ((float)resolution_x) / (0.5*std::pow(2.0,i_sequence));
        float angular_frequency = 2 * THREE_PHASE_PI / period_pixels;
        float amplitude = this->maximum_value_/2;
        float offset    = amplitude;    // Sets minimum value to zero


        // Generate the three phase code values
        std::vector< unsigned char > sine_phase_value_0;
        std::vector< unsigned char > sine_phase_value_90;
        std::vector< unsigned char > sine_phase_value_180;
        std::vector< unsigned char > sine_phase_value_270;

        // First create a set of binary patterns
        sine_phase_value_0.clear();
        sine_phase_value_90.clear();
        sine_phase_value_180.clear();
        sine_phase_value_270.clear();


    for(unsigned int iPoint = 0; iPoint < this->resolution_; iPoint++){
        sine_phase_value_0.push_back(   lroundf( amplitude * sin( ( angular_frequency * ((float)iPoint) ) + 1*MULTI_WAVELENGTH_PI) + offset ) );
        sine_phase_value_90.push_back( lroundf( amplitude * sin( (angular_frequency * ((float)iPoint) ) + 1.5* MULTI_WAVELENGTH_PI) + offset ) );
        sine_phase_value_180.push_back(lroundf( amplitude * sin( (angular_frequency * ((float)iPoint) ) + 2.0* MULTI_WAVELENGTH_PI) + offset ) );
        sine_phase_value_270.push_back( lroundf( amplitude * sin( (angular_frequency * ((float)iPoint) ) + 2.5*MULTI_WAVELENGTH_PI) + offset ) );
    }

    // Get the image resolution
    unsigned int rows     = this->pattern_rows_.Get();
    unsigned int columns  = this->pattern_columns_.Get();

    // Create the three phase images
    dlp::Image sine_phase_image_0;
    dlp::Image sine_phase_image_90;
    dlp::Image sine_phase_image_180;
    dlp::Image sine_phase_image_270;

    sine_phase_image_0.Create(  columns, rows, dlp::Image::Format::MONO_UCHAR );
    sine_phase_image_90.Create( columns, rows, dlp::Image::Format::MONO_UCHAR );
    sine_phase_image_180.Create( columns, rows, dlp::Image::Format::MONO_UCHAR );
    sine_phase_image_270.Create( columns, rows, dlp::Image::Format::MONO_UCHAR );


    switch(this->pattern_orientation_.Get()){
    case dlp::Pattern::Orientation::VERTICAL:
        for(     unsigned int yRow = 0; yRow < rows;    yRow++){
            for( unsigned int xCol = 0; xCol < columns; xCol++){
                sine_phase_image_0.Unsafe_SetPixel(    xCol, yRow, sine_phase_value_0.at(xCol) );
                sine_phase_image_90.Unsafe_SetPixel( xCol, yRow, sine_phase_value_90.at(xCol) );
                sine_phase_image_180.Unsafe_SetPixel( xCol, yRow, sine_phase_value_180.at(xCol) );
                sine_phase_image_270.Unsafe_SetPixel( xCol, yRow, sine_phase_value_270.at(xCol) );
            }
        }

        break;
    case dlp::Pattern::Orientation::HORIZONTAL:
        for(     unsigned int yRow = 0; yRow < rows;    yRow++){
            for( unsigned int xCol = 0; xCol < columns; xCol++){
                sine_phase_image_0.Unsafe_SetPixel(    xCol, yRow, sine_phase_value_0.at(yRow) );
                sine_phase_image_90.Unsafe_SetPixel( xCol, yRow, sine_phase_value_90.at(yRow) );
                sine_phase_image_180.Unsafe_SetPixel( xCol, yRow, sine_phase_value_180.at(yRow) );
                sine_phase_image_270.Unsafe_SetPixel( xCol, yRow, sine_phase_value_270.at(yRow) );
            }
        }
        break;

    case dlp::Pattern::Orientation::INVALID:
    default:
        return ret.AddError(STRUCTURED_LIGHT_NOT_SETUP);
        break;
    }

    // Create the patterns
    dlp::Pattern sine_phase_pattern_0;
    dlp::Pattern sine_phase_pattern_90;
    dlp::Pattern sine_phase_pattern_180;
    dlp::Pattern sine_phase_pattern_270;

    sine_phase_pattern_0.bitdepth  = this->bitdepth_.Get();
    sine_phase_pattern_0.color     = this->pattern_color_.Get();
    sine_phase_pattern_0.data_type = dlp::Pattern::DataType::IMAGE_DATA;
    sine_phase_pattern_0.image_data.Create(sine_phase_image_0);

    sine_phase_pattern_90.bitdepth  = this->bitdepth_.Get();
    sine_phase_pattern_90.color     = this->pattern_color_.Get();
    sine_phase_pattern_90.data_type = dlp::Pattern::DataType::IMAGE_DATA;
    sine_phase_pattern_90.image_data.Create(sine_phase_image_90);

    sine_phase_pattern_180.bitdepth  = this->bitdepth_.Get();
    sine_phase_pattern_180.color     = this->pattern_color_.Get();
    sine_phase_pattern_180.data_type = dlp::Pattern::DataType::IMAGE_DATA;
    sine_phase_pattern_180.image_data.Create(sine_phase_image_180);

    sine_phase_pattern_270.bitdepth  = this->bitdepth_.Get();
    sine_phase_pattern_270.color     = this->pattern_color_.Get();
    sine_phase_pattern_270.data_type = dlp::Pattern::DataType::IMAGE_DATA;
    sine_phase_pattern_270.image_data.Create(sine_phase_image_270);

    // Add the patterns to the return sequence
    for(unsigned int iCount = 0; iCount < this->repeat_phases_.Get();iCount++){
        pattern_sequence->Add(sine_phase_pattern_0);
    }
    for(unsigned int iCount = 0; iCount < this->repeat_phases_.Get();iCount++){
        pattern_sequence->Add(sine_phase_pattern_90);
    }
    for(unsigned int iCount = 0; iCount < this->repeat_phases_.Get();iCount++){
        pattern_sequence->Add(sine_phase_pattern_180);
    }

    for(unsigned int iCount = 0; iCount < this->repeat_phases_.Get();iCount++){
        pattern_sequence->Add(sine_phase_pattern_270);
    }

    // Clear the images
    sine_phase_image_0.Clear();
    sine_phase_image_90.Clear();
    sine_phase_image_180.Clear();
    sine_phase_image_270.Clear();
  }


    //generate y sequence
    for(int i_sequence= 0;i_sequence< max_count;i_sequence++)
    {
        float period_pixels = ((float)resolution_y) / (0.5*std::pow(2.0,i_sequence));
        float angular_frequency = 2 * THREE_PHASE_PI / period_pixels;
        float amplitude = this->maximum_value_/2;
        float offset    = amplitude;    // Sets minimum value to zero


    // Generate the three phase code values
    std::vector< unsigned char > sine_phase_value_0;
    std::vector< unsigned char > sine_phase_value_90;
    std::vector< unsigned char > sine_phase_value_180;
    std::vector< unsigned char > sine_phase_value_270;

    // First create a set of binary patterns
    sine_phase_value_0.clear();
    sine_phase_value_90.clear();
    sine_phase_value_180.clear();
    sine_phase_value_270.clear();


    for(unsigned int iPoint = 0; iPoint < resolution_y; iPoint++){
        sine_phase_value_0.push_back(   lroundf( amplitude * sin( ( angular_frequency * ((float)iPoint) ) + 1.0*MULTI_WAVELENGTH_PI) + offset ) );
        sine_phase_value_90.push_back( lroundf( amplitude * sin( (angular_frequency * ((float)iPoint) ) + 1.5* MULTI_WAVELENGTH_PI) + offset ) );
        sine_phase_value_180.push_back(lroundf( amplitude * sin( (angular_frequency * ((float)iPoint) ) + 2.0* MULTI_WAVELENGTH_PI) + offset ) );
        sine_phase_value_270.push_back( lroundf( amplitude * sin( (angular_frequency * ((float)iPoint) ) + 2.5*MULTI_WAVELENGTH_PI) + offset ) );
    }

    // Get the image resolution
    unsigned int rows     = this->pattern_rows_.Get();
    unsigned int columns  = this->pattern_columns_.Get();

    // Create the three phase images
    dlp::Image sine_phase_image_0;
    dlp::Image sine_phase_image_90;
    dlp::Image sine_phase_image_180;
    dlp::Image sine_phase_image_270;

    sine_phase_image_0.Create(  columns, rows, dlp::Image::Format::MONO_UCHAR );
    sine_phase_image_90.Create( columns, rows, dlp::Image::Format::MONO_UCHAR );
    sine_phase_image_180.Create( columns, rows, dlp::Image::Format::MONO_UCHAR );
    sine_phase_image_270.Create( columns, rows, dlp::Image::Format::MONO_UCHAR );



        for(     unsigned int yRow = 0; yRow < rows;    yRow++){
            for( unsigned int xCol = 0; xCol < columns; xCol++){
                sine_phase_image_0.Unsafe_SetPixel(    xCol, yRow, sine_phase_value_0.at(yRow) );
                sine_phase_image_90.Unsafe_SetPixel( xCol, yRow, sine_phase_value_90.at(yRow) );
                sine_phase_image_180.Unsafe_SetPixel( xCol, yRow, sine_phase_value_180.at(yRow) );
                sine_phase_image_270.Unsafe_SetPixel( xCol, yRow, sine_phase_value_270.at(yRow) );
            }
        }


    // Create the patterns
    dlp::Pattern sine_phase_pattern_0;
    dlp::Pattern sine_phase_pattern_90;
    dlp::Pattern sine_phase_pattern_180;
    dlp::Pattern sine_phase_pattern_270;

    sine_phase_pattern_0.bitdepth  = this->bitdepth_.Get();
    sine_phase_pattern_0.color     = this->pattern_color_.Get();
    sine_phase_pattern_0.data_type = dlp::Pattern::DataType::IMAGE_DATA;
    sine_phase_pattern_0.image_data.Create(sine_phase_image_0);

    sine_phase_pattern_90.bitdepth  = this->bitdepth_.Get();
    sine_phase_pattern_90.color     = this->pattern_color_.Get();
    sine_phase_pattern_90.data_type = dlp::Pattern::DataType::IMAGE_DATA;
    sine_phase_pattern_90.image_data.Create(sine_phase_image_90);

    sine_phase_pattern_180.bitdepth  = this->bitdepth_.Get();
    sine_phase_pattern_180.color     = this->pattern_color_.Get();
    sine_phase_pattern_180.data_type = dlp::Pattern::DataType::IMAGE_DATA;
    sine_phase_pattern_180.image_data.Create(sine_phase_image_180);

    sine_phase_pattern_270.bitdepth  = this->bitdepth_.Get();
    sine_phase_pattern_270.color     = this->pattern_color_.Get();
    sine_phase_pattern_270.data_type = dlp::Pattern::DataType::IMAGE_DATA;
    sine_phase_pattern_270.image_data.Create(sine_phase_image_270);

    // Add the patterns to the return sequence
    for(unsigned int iCount = 0; iCount < this->repeat_phases_.Get();iCount++){
        pattern_sequence->Add(sine_phase_pattern_0);
    }
    for(unsigned int iCount = 0; iCount < this->repeat_phases_.Get();iCount++){
        pattern_sequence->Add(sine_phase_pattern_90);
    }
    for(unsigned int iCount = 0; iCount < this->repeat_phases_.Get();iCount++){
        pattern_sequence->Add(sine_phase_pattern_180);
    }

    for(unsigned int iCount = 0; iCount < this->repeat_phases_.Get();iCount++){
        pattern_sequence->Add(sine_phase_pattern_270);
    }

    // Clear the images
    sine_phase_image_0.Clear();
    sine_phase_image_90.Clear();
    sine_phase_image_180.Clear();
    sine_phase_image_270.Clear();
  }


    /*******************************************************************************************************************/
    /*******************************************************************************************************************/
    //generate Three Frequence Heterdyne patterns
    //generate Three Frequence Heterdyne x
    float p_1= 52;
    float p_2= 58;
    float p_3= 620;

    //生成我们自己的图片
    generatePhaseShiftPatterns(4,p_1,dlp::Pattern::Orientation::VERTICAL,pattern_sequence);
//    generatePhaseShiftPatterns(4,p_2,dlp::Pattern::Orientation::VERTICAL,pattern_sequence);
//    generatePhaseShiftPatterns(4,p_3,dlp::Pattern::Orientation::VERTICAL,pattern_sequence);

//    generatePhaseShiftPatterns(4,p_1,dlp::Pattern::Orientation::HORIZONTAL,pattern_sequence);
//    generatePhaseShiftPatterns(4,p_2,dlp::Pattern::Orientation::HORIZONTAL,pattern_sequence);
//    generatePhaseShiftPatterns(4,p_3,dlp::Pattern::Orientation::HORIZONTAL,pattern_sequence);




    /*******************************************************************************************************************/

    /*******************************************************************************************************************/
    //generate Three Frequence Heterdyne patterns
    //generate Three Frequence Heterdyne x

//    p_1= 1824;
//    p_2= 114;
//    p_3= 38;

//    generatePhaseShiftPatterns(4,p_1,dlp::Pattern::Orientation::VERTICAL,pattern_sequence);
//    generatePhaseShiftPatterns(4,p_2,dlp::Pattern::Orientation::VERTICAL,pattern_sequence);
//    generatePhaseShiftPatterns(4,p_3,dlp::Pattern::Orientation::VERTICAL,pattern_sequence);

//    p_1= 2280;
//    p_2= 142.5;
//    p_3= 47.5;


//    generatePhaseShiftPatterns(4,p_1,dlp::Pattern::Orientation::HORIZONTAL,pattern_sequence);
//    generatePhaseShiftPatterns(4,p_2,dlp::Pattern::Orientation::HORIZONTAL,pattern_sequence);
//    generatePhaseShiftPatterns(4,p_3,dlp::Pattern::Orientation::HORIZONTAL,pattern_sequence);




    /*******************************************************************************************************************/


    return ret;
}



/** @brief Decodes the \ref dlp::Capture::Sequence and returns the \ref dlp::DisparityMap
 *  @param[in] capture_sequence \ref dlp::Capture::Sequence to be decoded
 *  @param[in] disparity_map Return pointer for generated \ref dlp::DisparityMap
 *  @retval STRUCTURED_LIGHT_NULL_POINTER_ARGUMENT      Input arguments NULL
 *  @retval STRUCTURED_LIGHT_NOT_SETUP                  Module has NOT been setup
 *  @retval STRUCTURED_LIGHT_CAPTURE_SEQUENCE_EMPTY     Supplied sequence is empty
 *  @retval STRUCTURED_LIGHT_CAPTURE_SEQUENCE_SIZE_INVALID  Supplied sequence has a difference count than what was generated
 *  @retval STRUCTURED_LIGHT_DATA_TYPE_INVALID          Supplied sequence does NOT contain valid image data or a image file name
*/
ReturnCode mutil_wavelength::DecodeCaptureSequence(Capture::Sequence *capture_sequence, dlp::DisparityMap *disparity_map){
    ReturnCode ret;

    // Check the pointers
    if(!capture_sequence || !disparity_map)
        return ret.AddError(STRUCTURED_LIGHT_NULL_POINTER_ARGUMENT);

    // Check that ThreePhase object is setup
//    if(!this->isSetup() || !this->hybrid_unwrap_module_.isSetup())
//        return ret.AddError(STRUCTURED_LIGHT_NOT_SETUP);

    // Check that CaptureSequence is not empty
    if(capture_sequence->GetCount() == 0)
        return ret.AddError(STRUCTURED_LIGHT_CAPTURE_SEQUENCE_EMPTY);

    // Check that correct number of images present
    if(capture_sequence->GetCount() != this->sequence_count_total_)
        return ret.AddError(STRUCTURED_LIGHT_CAPTURE_SEQUENCE_SIZE_INVALID);

    // Create a vector of the images to decode
    std::vector<dlp::Image> images_coded;

    // Store the image resolution
    unsigned int image_rows    = 0;
    unsigned int image_columns = 0;

    // Seperate the GrayCode captures
    dlp::Capture::Sequence gray_code_sequence;

    // Check and copy the image data for the three phase patterns
    for(unsigned int iCapture = 0; iCapture < this->sequence_count_total_; iCapture++){
        dlp::Capture capture;
        dlp::Image   image;
        unsigned int capture_rows;
        unsigned int capture_columns;

        // Grab the capture from the sequence
        ReturnCode ret_error = capture_sequence->Get(iCapture, &capture);

        // Check that capture was grabbed
        if(ret_error.hasErrors())
            return ret_error;

        // The first three captures are the sinusoidal patterns
        if(iCapture < (3*this->repeat_phases_.Get())){

            // Check the capture type
            switch(capture.data_type){
            case dlp::Capture::DataType::IMAGE_FILE:
            {
                // Check that the file exists
                if(!dlp::File::Exists(capture.image_file))
                    return ret.AddError(FILE_DOES_NOT_EXIST);

                // Load the file and check the resolution
                ret_error = image.Load(capture.image_file);
                if(ret_error.hasErrors())
                    return ret_error;

                break;
            }
            case dlp::Capture::DataType::IMAGE_DATA:
            {
                // Check that the image data is not empty
                if(capture.image_data.isEmpty())
                    return ret.AddError(IMAGE_EMPTY);

                // Load the file and check the resolution
                cv::Mat temp_image_data;
                capture.image_data.Unsafe_GetOpenCVData(&temp_image_data);
                ret_error = image.Create(temp_image_data);
                if(ret_error.hasErrors())
                    return ret_error;
                temp_image_data.release();
                break;
            }
            case dlp::Capture::DataType::INVALID:
            default:
                return ret.AddError(STRUCTURED_LIGHT_DATA_TYPE_INVALID);
            }

            // Get the image resolution
            image.GetColumns(&capture_columns);
            image.GetRows(&capture_rows);

            // If on the first capture store the resolution
            if(iCapture == 0){
                image_columns = capture_columns;
                image_rows    = capture_rows;
            }

            // Check that each image has the same resolution
            if( (capture_rows    != image_rows) ||
                (capture_columns != image_columns))
                return ret.AddError(STRUCTURED_LIGHT_PATTERN_SIZE_INVALID);

            // Convert the image to monochrome
            image.ConvertToMonochrome();

            // Add the image to the list
            images_coded.push_back(image);

            // Clear the image
            image.Clear();
        }
        else{
            // Add all GrayCode captures to sequence for seperate decoding
            gray_code_sequence.Add(capture);
        }
    }

    // Decode the GrayCode sequence
//    dlp::DisparityMap gray_code_disparity;
//    ret = this->hybrid_unwrap_module_.DecodeCaptureSequence(&gray_code_sequence,&gray_code_disparity);

//    if(ret.hasErrors())
//        return ret;

    // Check the resolution of the GrayCode disparity map
    unsigned int gray_code_disparity_rows;
    unsigned int gray_code_disparity_columns;
//    gray_code_disparity.GetColumns(&gray_code_disparity_columns);
//    gray_code_disparity.GetRows(&gray_code_disparity_rows);

    if((gray_code_disparity_columns != image_columns) ||
       (gray_code_disparity_rows    != image_rows))
        return ret.AddError(STRUCTURED_LIGHT_PATTERN_SIZE_INVALID);

    // Allocate memory for the disparity map
//    ret = this->disparity_map_.Create( image_columns, image_rows, this->pattern_orientation_.Get(),this->over_sample_.Get());

//    if(ret.hasErrors())
//        return ret;


    // Decode each pixel
    int   disparity_value;
    int   gray_code_disparity_value;
    float phase_value;
    unsigned char intensity_0;
    unsigned char intensity_p120;
    unsigned char intensity_n120;

//    float over_sample = float(this->over_sample_.Get());

//    for(     unsigned int yRow = 0; yRow < image_rows;    yRow++){
//        for( unsigned int xCol = 0; xCol < image_columns; xCol++){

//            // Get the sinusoidal intensity values
//            float intensity_phase_0    = 0;
//            float intensity_phase_p120 = 0;
//            float intensity_phase_n120 = 0;

//            for(unsigned int iCount = 0; iCount < this->repeat_phases_.Get();iCount++){
//                unsigned char temp_0    = 0;
//                unsigned char temp_p120 = 0;
//                unsigned char temp_n120 = 0;

//                unsigned char image_0    = (this->repeat_phases_.Get()*0) + iCount;
//                unsigned char image_p120 = (this->repeat_phases_.Get()*1) + iCount;
//                unsigned char image_n120 = (this->repeat_phases_.Get()*2) + iCount;

//                images_coded.at(image_0   ).Unsafe_GetPixel(xCol,yRow,&temp_0);
//                images_coded.at(image_p120).Unsafe_GetPixel(xCol,yRow,&temp_p120);
//                images_coded.at(image_n120).Unsafe_GetPixel(xCol,yRow,&temp_n120);

//                intensity_phase_0 += temp_0;
//                intensity_phase_p120 += temp_p120;
//                intensity_phase_n120 += temp_n120;

//            }

//            intensity_phase_0    = intensity_phase_0    / this->repeat_phases_.Get();
//            intensity_phase_p120 = intensity_phase_p120 / this->repeat_phases_.Get();
//            intensity_phase_n120 = intensity_phase_n120 / this->repeat_phases_.Get();


////            images_coded.at(0).Unsafe_GetPixel(xCol,yRow,&intensity_0);
////            images_coded.at(1).Unsafe_GetPixel(xCol,yRow,&intensity_p120);
////            images_coded.at(2).Unsafe_GetPixel(xCol,yRow,&intensity_n120);

//            // Calculate the wrapped phase
//            phase_value = atan( sqrt(3.0) * (intensity_phase_n120 - intensity_phase_p120) /
//                               (2.0*(intensity_phase_0)-(intensity_phase_n120)-(intensity_phase_p120)) )
//                                / THREE_PHASE_PI;

//            if((phase_value >= 0.5) || (phase_value <= -0.5)){
//                // Pixel is invalid
//                disparity_value = dlp::DisparityMap::INVALID_PIXEL;
//            }
//            else{
//                // Convert the phase to a wrapped pixel value
////                disparity_value = lroundf(over_sample*(phase_value + 0.5) * ((float)this->resolution_) / this->phase_counts_);

//                if(this->use_hybrid_.Get()){
//                    // Get the gray code disparity pixel value
//                    gray_code_disparity.Unsafe_GetPixel(xCol,yRow,&gray_code_disparity_value);

//                    //disparity_vals[(unsigned int)gray_code_disparity_value]++;

//                    if((gray_code_disparity_value != dlp::DisparityMap::INVALID_PIXEL) &&
//                       (gray_code_disparity_value != dlp::DisparityMap::EMPTY_PIXEL)){


//                        // Check that the phase change regions are correct
//                        if(((gray_code_disparity_value+1) % 4) == 0){
//                            // The fourth region of a period should be greater than
//                            // 0.25 and absolutely greater than 0. If it is less than
//                            // zero the phase has been missclassified
//                            if(phase_value < 0) gray_code_disparity_value++;
//                        }
//                        else if(((gray_code_disparity_value+1) % 4) == 1){
//                            // The first region of a period should be less than
//                            // -0.25 and absolutely less than 0. If it is greater than
//                            // zero the phase has been missclassified
//                            if(phase_value > 0) gray_code_disparity_value--;
//                        }

//                        // Adjust the GrayCode disparity value to the phase regions
//                        gray_code_disparity_value = gray_code_disparity_value / 4;

//                        // Add the GrayCode disparity value to unwrap the values
//                        disparity_value += (over_sample*gray_code_disparity_value*this->resolution_/this->phase_counts_);
//                    }
//                    else{
//                        disparity_value = dlp::DisparityMap::INVALID_PIXEL;
//                    }
//                }
//                else{
//                    // Non hybrid method not implemented
//                    disparity_value = dlp::DisparityMap::INVALID_PIXEL;
//                }
//            }

//            // Save the calculated pixel value
//            this->disparity_map_.Unsafe_SetPixel(xCol,yRow,disparity_value);
//        }
//    }

//    std::ofstream myfile;
//      myfile.open ("disparity.txt");
//      for(unsigned int i=0;i<10000;i++){
//          myfile << i << ", " <<  disparity_vals[i];
//      }
//      myfile.close();

    // Copy the disparity map to the pointer
    ret = disparity_map->Create(this->disparity_map_);

    return ret;
}


}
