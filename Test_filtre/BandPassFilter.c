#include <math.h>
#include "BandPassFilter.h"

void initBandPassFilter(IIRFilter* filter, float lowFreq, float highFreq, float samplingRate) {
    float omegaLow = 2.0f * PI * lowFreq / samplingRate;
    float omegaHigh = 2.0f * PI * highFreq / samplingRate;

    float omegaCenter = (omegaHigh + omegaLow) / 2.0f;
    float bandwidth = omegaHigh - omegaLow;

    float tanBandwidth = tan(bandwidth / 2.0f);
    float cosCenter = cos(omegaCenter);

    float alpha = tanBandwidth / (1.0f + tanBandwidth);
    filter->b[0] = alpha;
    filter->b[1] = 0;
    filter->b[2] = -alpha;
    filter->a[0] = 1.0f;
    filter->a[1] = -2.0f * cosCenter / (1.0f + tanBandwidth);
    filter->a[2] = (1.0f - tanBandwidth) / (1.0f + tanBandwidth);

    filter->x[0] = filter->x[1] = 0.0f;
    filter->y[0] = filter->y[1] = 0.0f;
}

float applyBandPassFilter(IIRFilter* filter, float input) {
    float output = filter->b[0] * input +
                   filter->b[1] * filter->x[0] +
                   filter->b[2] * filter->x[1] -
                   filter->a[1] * filter->y[0] -
                   filter->a[2] * filter->y[1];

    filter->x[1] = filter->x[0];
    filter->x[0] = input;
    filter->y[1] = filter->y[0];
    filter->y[0] = output;

    return output;
}

