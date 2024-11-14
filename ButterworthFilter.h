#ifndef BUTTERWORTH_FILTER_H
#define BUTTERWORTH_FILTER_H

#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


class ButterworthFilter {
public:
    ButterworthFilter(double sampleRate, double lowCut, double highCut, int order);
    double process(double inputSample);

private:
    double sampleRate;
    double lowCut;
    double highCut;
    int order;
    std::vector<double> aCoeffs;
    std::vector<double> bCoeffs;
    std::vector<double> inputHistory;
    std::vector<double> outputHistory;

    void calculateCoefficients();
};

#endif 
