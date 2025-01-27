#ifndef BANDPASSFILTER_H
#define BANDPASSFILTER_H

#define PI 3.14159265359

typedef struct {
    float b[3]; // Coefficients du numérateur
    float a[3]; // Coefficients du dénominateur
    float x[2]; // Décalages du signal d'entrée
    float y[2]; // Décalages du signal de sortie
} IIRFilter;

void initBandPassFilter(IIRFilter* filter, float lowFreq, float highFreq, float samplingRate);
float applyBandPassFilter(IIRFilter* filter, float input);

#endif // BANDPASSFILTER_H

