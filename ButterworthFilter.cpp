#include "ButterworthFilter.h"

ButterworthFilter::ButterworthFilter(double sampleRate, double lowCut, double highCut, int order)
    : sampleRate(sampleRate), lowCut(lowCut), highCut(highCut), order(order),
    inputHistory(order + 1, 0.0), outputHistory(order + 1, 0.0) {
    calculateCoefficients();
}

void ButterworthFilter::calculateCoefficients() {
    double omegaLow = 2.0 * M_PI * lowCut / sampleRate;
    double omegaHigh = 2.0 * M_PI * highCut / sampleRate;
    double bw = omegaHigh - omegaLow;
    double centerFreq = sqrt(omegaLow * omegaHigh);

    aCoeffs.resize(order + 1, 0.0);
    bCoeffs.resize(order + 1, 0.0);

    aCoeffs[0] = 1.0;
    aCoeffs[1] = -2.0 * cos(centerFreq) / exp(bw);
    aCoeffs[2] = exp(-2.0 * bw);

    bCoeffs[0] = bw;
    bCoeffs[1] = 0.0;
    bCoeffs[2] = -bw;
}

double ButterworthFilter::process(double inputSample) {
    inputHistory[0] = inputSample;
    double outputSample = 0.0;

    for (int i = 0; i <= order; ++i) {
        outputSample += bCoeffs[i] * inputHistory[i] - aCoeffs[i] * outputHistory[i];
    }

    for (int i = order; i > 0; --i) {
        inputHistory[i] = inputHistory[i - 1];
        outputHistory[i] = outputHistory[i - 1];
    }

    outputHistory[0] = outputSample;
    return outputSample;
}
