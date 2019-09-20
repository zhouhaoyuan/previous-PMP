#ifndef MUTIL_WAVELENGTH_H
#define MUTIL_WAVELENGTH_H

#include <common/returncode.hpp>
#include <common/debug.hpp>
#include <common/parameters.hpp>
#include <common/capture/capture.hpp>
#include <common/pattern/pattern.hpp>
#include <common/disparity_map.hpp>

#include <structured_light/structured_light.hpp>
#include <structured_light/gray_code/gray_code.hpp>
#include <structured_light/three_phase/three_phase.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#define MUTIL_WAVELENGTH_FREQUENCY_MISSING                   "MUTIL_WAVELENGTH_FREQUENCY_MISSING"
#define MUTIL_WAVELENGTH_SEQUENCE_COUNT_MISSING                   "MUTIL_WAVELENGTH_SEQUENCE_COUNT_MISSING"
#define MUTIL_WAVELENGTH_PIXELS_PER_PERIOD_MISSING           "MUTIL_WAVELENGTH_PIXELS_PER_PERIOD_MISSING"
#define MUTIL_WAVELENGTH_PIXELS_PER_PERIOD_NOT_DIVISIBLE_BY_EIGHT           "MUTIL_WAVELENGTH_PIXELS_PER_PERIOD_NOT_DIVISIBLE_BY_EIGHT"
#define MUTIL_WAVELENGTH_BITDEPTH_MISSING                    "MUTIL_WAVELENGTH_BITDEPTH_MISSING"
#define MUTIL_WAVELENGTH_BITDEPTH_TOO_SMALL                  "MUTIL_WAVELENGTH_BITDEPTH_TOO_SMALL"
#define MULTI_WAVELENGTH_PI              3.14159265359

/** @brief  Contains all DLP SDK classes, functions, etc. */
namespace dlp{

/** @class      ThreePhase
 *  @ingroup    StructuredLight
 *  @brief      Structured Light subclass used to generate and decode Three Phase sinusoidal patterns
 *              binary patterns
 */
class mutil_wavelength : public dlp::StructuredLight
{
public:
    mutil_wavelength();

    class Parameters{
    public:
        DLP_NEW_PARAMETERS_ENTRY(Frequency,         "MUTIL_WAVELENGTH_PARAMETERS_FREQUENCY", double, 2.0);
        DLP_NEW_PARAMETERS_ENTRY(SequenceCount,     "MUTIL_WAVELENGTH_PARAMETERS_SEQUENCE_COUNT", unsigned int,    0);
        DLP_NEW_PARAMETERS_ENTRY(PixelsPerPeriod,   "MUTIL_WAVELENGTH_PARAMETERS_PIXELS_PER_PERIOD", unsigned int, 8);
        DLP_NEW_PARAMETERS_ENTRY(Bitdepth,          "MUTIL_WAVELENGTH_PARAMETERS_BITDEPTH",  dlp::Pattern::Bitdepth, dlp::Pattern::Bitdepth::MONO_8BPP);
        DLP_NEW_PARAMETERS_ENTRY(RepeatPhases,      "MUTIL_WAVELENGTH_PARAMETERS_REPEAT_PHASES", unsigned int, 1);

    };



    ReturnCode Setup(const dlp::Parameters &settings);
    ReturnCode GetSetup( dlp::Parameters *settings) const;

    ReturnCode GeneratePatternSequence(Pattern::Sequence *pattern_sequence);
    ReturnCode DecodeCaptureSequence(Capture::Sequence *capture_sequence,dlp::DisparityMap *disparity_map);



private:
    void createSinData(int move, int stepNum, float pixel, Pattern::Orientation orientation, cv::Mat &sin_data);

    void generatePhaseShiftPatterns(int stepNum,float wavelength,Pattern::Orientation orientation,Pattern::Sequence *pattern_sequence);

    void Generate_sin_ladder_Pattern(std::vector<cv::Mat> &PatternSequence);
private:
    Parameters::Frequency       frequency_;
    Parameters::SequenceCount       sequence_count_;
    Parameters::PixelsPerPeriod pixels_per_period_;
    Parameters::Bitdepth        bitdepth_;
    Parameters::RepeatPhases    repeat_phases_;

    float phase_counts_;
    float maximum_value_;
    unsigned int  resolution_;
};
}

#endif // MUTIL_WAVELENGTH_H
