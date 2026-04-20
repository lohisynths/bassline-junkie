/*
  ==============================================================================

    VAStateVariableFilter.cpp
    Created: 10 May 2015 2:41:43pm
    Author:  Jordan Harris

  ==============================================================================
*/

#include "VAStateVariableFilter.h"
#include "fast_trig.h"
#include "../config.h"

#include <algorithm>

namespace {
constexpr double kLinearResLimit = 0.75;
constexpr double kEffectiveResMax = 0.88;
constexpr double kCompStartRes = 0.65;
constexpr double kMaxCompDb = -6.0;
constexpr double kPi = 3.14159265358979323846;

double clampUserResonance(double resonance)
{
    return std::max(0.0, std::min(1.0, resonance));
}

double mapUserResonance(double resonance)
{
    if (resonance <= kLinearResLimit) {
        return resonance;
    }

    const double t = (resonance - kLinearResLimit) / (1.0 - kLinearResLimit);
    return kLinearResLimit
        + (kEffectiveResMax - kLinearResLimit)
            * (1.0 - bassline::math::pow(1.0 - t, 3.0));
}

double resonanceCompGain(double effectiveResonance)
{
    if (effectiveResonance <= kCompStartRes) {
        return 1.0;
    }

    const double t = (effectiveResonance - kCompStartRes)
        / (kEffectiveResMax - kCompStartRes);
    const double compDb = kMaxCompDb * t * t;
    return bassline::math::pow(10.0, compDb / 20.0);
}
} // namespace

//==============================================================================
double resonanceToQ(double resonance)
{
    return 1.0 / (2.0 * (1.0 - resonance));
}

double pitchToFreq(double pitch)
{
    return bassline::math::exp2((pitch - 69.0) / 12.0) * 440.0;
}

VAStateVariableFilter::VAStateVariableFilter()
{
	shelfGain=1;
    sampleRate = sample_rate;				// default sample rate when constructed
    filterType = SVFLowpass;			// lowpass filter by default

    gCoeff = 1.0f;
    RCoeff = 1.0f;
    KCoeff = 0.0f;
    outputGain = 1.0f;

    cutoffFreq = 1000.0f;
    Q = static_cast<float>(resonanceToQ(0.5));

    z1_A[0] = z2_A[0] = 0.0f;
    z1_A[1] = z2_A[1] = 0.0f;

    //smoothTimeMs = 0.0;		        // 0.0 milliseconds
}

VAStateVariableFilter::~VAStateVariableFilter()
{
}

// Member functions for setting the filter's parameters (and sample rate).
//==============================================================================
void VAStateVariableFilter::setFilterType(const int& newType)
{
    filterType = newType;
}

void VAStateVariableFilter::setCutoffPitch(const float& newCutoffPitch)
{
    if (active) {
        cutoffFreq = static_cast<float>(pitchToFreq(newCutoffPitch));
        //cutoffLinSmooth.setValue(cutoffFreq);
        calcFilter();
    }
}

void VAStateVariableFilter::setCutoff(const float& newCutoffFreq)
{
    if (active) {
        cutoffFreq = newCutoffFreq;
        calcFilter();
    }
}

void VAStateVariableFilter::setRes(const float& newResonance)
{
    if (active) {
        const double userResonance = clampUserResonance(newResonance);
        const double effectiveResonance = mapUserResonance(userResonance);
        Q = static_cast<float>(resonanceToQ(effectiveResonance));
        outputGain = static_cast<float>(resonanceCompGain(effectiveResonance));
        calcFilter();
    }
}

void VAStateVariableFilter::setQ(const float& newQ)
{
    if (active) {
        Q = newQ;
        outputGain = 1.0f;
        calcFilter();
    }
}

void VAStateVariableFilter::setShelfGain(const float& newGain)
{
    if (active) {
        shelfGain = newGain;
        calcFilter();
    }
}

void VAStateVariableFilter::setFilter(const int& newType, const float& newCutoffFreq,
                                      const float& newResonance, const float& newShelfGain)
{
    filterType = newType;
    cutoffFreq = newCutoffFreq;
    const double userResonance = clampUserResonance(newResonance);
    const double effectiveResonance = mapUserResonance(userResonance);
    Q = static_cast<float>(resonanceToQ(effectiveResonance));
    outputGain = static_cast<float>(resonanceCompGain(effectiveResonance));
    shelfGain = newShelfGain;
    calcFilter();
}

void VAStateVariableFilter::setSampleRate(const float& newSampleRate)
{
    sampleRate = newSampleRate;
    //cutoffLinSmooth.reset(sampleRate, smoothTimeMs);
    calcFilter();
}

/*void VAStateVariableFilter::setSmoothingTimeInMs(const float & newSmoothingTimeMs)
{
    smoothTimeMs = newSmoothingTimeMs;
}*/

void VAStateVariableFilter::setIsActive(bool isActive)
{
    active = isActive;
}

//==============================================================================
void VAStateVariableFilter::calcFilter()
{
    if (active) {

        // prewarp the cutoff (for bilinear-transform filters)
        float wd = static_cast<float>(cutoffFreq * 2.0f * kPi);
        float T = 1.0f / (float)sampleRate;
        float wa = static_cast<float>(
            (2.0f / T) * bassline::math::tan(wd * T / 2.0f));

        // Calculate g (gain element of integrator)
        gCoeff = wa * T / 2.0f;			// Calculate g (gain element of integrator)

        // Calculate Zavalishin's R from Q (referred to as damping parameter)
        RCoeff = 1.0f / (2.0f * Q);

        // Gain for BandShelving filter
        KCoeff = shelfGain;
    }
}

float VAStateVariableFilter::process(const float& input, const int& channelIndex)
{
    if (active) {

        // Do the cutoff parameter smoothing per sample.
        //cutoffFreq = cutoffLinSmooth.getNextValue();
        //calcFilter();

        // Filter processing:
        const float HP = (input - (2.0f * RCoeff + gCoeff) * z1_A[channelIndex] - z2_A[channelIndex])
            / (1.0f + (2.0f * RCoeff * gCoeff) + gCoeff * gCoeff);

        const float BP = HP * gCoeff + z1_A[channelIndex];

        const float LP = BP * gCoeff + z2_A[channelIndex];

        const float UBP = 2.0f * RCoeff * BP;

        const float BShelf = input + UBP * KCoeff;

        const float Notch = input - UBP;

        const float AP = input - (4.0f * RCoeff * BP);

        const float Peak = LP - HP;

        z1_A[channelIndex] = gCoeff * HP + BP;		// unit delay (state variable)
        z2_A[channelIndex] = gCoeff * BP + LP;		// unit delay (state variable)

        float output = 0.0f;
        bool applyCompensation = false;

        // Selects which filter type this function will output.
        switch (filterType) {
        case SVFLowpass:
            output = LP;
            applyCompensation = true;
            break;
        case SVFBandpass:
            output = BP;
            applyCompensation = true;
            break;
        case SVFHighpass:
            output = HP;
            applyCompensation = true;
            break;
        case SVFUnitGainBandpass:
            output = UBP;
            break;
        case SVFBandShelving:
            output = BShelf;
            break;
        case SVFNotch:
            output = Notch;
            break;
        case SVFAllpass:
            output = AP;
            break;
        case SVFPeak:
            output = Peak;
            break;
        default:
            output = 0.0f;
            break;
        }

        return applyCompensation ? output * outputGain : output;
    }
    else {	// If not active, return input
        return input;
    }
}

void VAStateVariableFilter::processAudioBlock(float* const samples,  const int& numSamples,
                                              const int& channelIndex)
{
    // Test if filter is active. If not, bypass it
    if (active) {

        // Loop through the sample block and process it
        for (int i = 0; i < numSamples; ++i) {

            // Do the cutoff parameter smoothing per sample.
            //cutoffFreq = cutoffLinSmooth.getNextValue();
            //calcFilter();       // calculate the coefficients for the smoother

            // Filter processing:
            const float input = samples[i];

            const float HP = (input - (2.0f * RCoeff + gCoeff) * z1_A[channelIndex] - z2_A[channelIndex])
                       / (1.0f + (2.0f * RCoeff * gCoeff) + gCoeff * gCoeff);

            const float BP = HP * gCoeff + z1_A[channelIndex];

            const float LP = BP * gCoeff + z2_A[channelIndex];

            const float UBP = 2.0f * RCoeff * BP;

            const float BShelf = input + UBP * KCoeff;

            const float Notch = input - UBP;

            const float AP = input - (4.0f * RCoeff * BP);

            const float Peak = LP - HP;

            z1_A[channelIndex] = gCoeff * HP + BP;		// unit delay (state variable)
            z2_A[channelIndex] = gCoeff * BP + LP;		// unit delay (state variable)

            float output = 0.0f;
            bool applyCompensation = false;

            // Selects which filter type this function will output.
            switch (filterType) {
            case SVFLowpass:
                output = LP;
                applyCompensation = true;
                break;
            case SVFBandpass:
                output = BP;
                applyCompensation = true;
                break;
            case SVFHighpass:
                output = HP;
                applyCompensation = true;
                break;
            case SVFUnitGainBandpass:
                output = UBP;
                break;
            case SVFBandShelving:
                output = BShelf;
                break;
            case SVFNotch:
                output = Notch;
                break;
            case SVFAllpass:
                output = AP;
                break;
            case SVFPeak:
                output = Peak;
                break;
            default:
                output = 0.0f;
                break;
            }

            samples[i] = applyCompensation ? output * outputGain : output;
        }
    }
}

//==============================================================================
