#include "agc.h"
#include <math.h>

// Initialiser l'AGC avec les paramètres par défaut
void initAGC(AGC_s *agc, float target, float alpha) {
    agc->gain = 1.0f;
    agc->target = target;
    agc->alpha = alpha;
}

// Mettre à jour le gain en fonction du signal actuel
void updateAGC(AGC_s *agc, float input) {
    // Calculer l'erreur
    float error = agc->target - fabs((double)input);

    // Mettre à jour le gain en fonction de l'erreur
    agc->gain += agc->alpha * error;

    // Limiter le gain pour éviter des valeurs excessives
    if (agc->gain < 0.5f) {//6dB
        agc->gain = 0.5f;
    } else if (agc->gain > 10.0f) {//20dB
        agc->gain = 10.0f;
    }
}

// Appliquer le gain à un signal
float applyGain(AGC_s *agc, float input) {
    return agc->gain * input;
}

void AGC(AGC_s *agc, float *x, float *y, float target, int size)
{
    int i=0;

    for(i=0; i < size; i++)
    {
    	// Appliquer le gain au signal d'entrée
    	y[i] = applyGain(agc, x[i]);
        // Mettre à jour l'AGC avec un signal factice (peut être remplacé par une entrée réelle)
        updateAGC(agc, y[i]);
    }
}
