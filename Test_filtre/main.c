#include <math.h>
#include "kiss_fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "BandPassFilter.h"

#define SAMPLING_RATE 8000
#define NUM_SAMPLES 800
#define BAND_LOW 110.0
#define BAND_HIGH 140.0
#define NUM_FREQUENCIES 3800


typedef struct {
    float frequency;
    float amplitude;
    float phase;
} Group;

void loadBufferFromFile(const char* filename, float* buffer, int length) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < length; i++) {
        if (fscanf(file, "%f,", &buffer[i]) != 1) {
            printf("Erreur : Le fichier %s ne contient pas suffisamment de données\n", filename);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
}

float calculateEnergy(float* signal, int length, IIRFilter* bandPassFilter,float* filteredSignal, float samplingRate, float lowFreq, float highFreq) {
    float energyBP = 0.0f, energyTotal = 0.0f;
    float samplePeriod = 1.0f / samplingRate;

    for (int i = 0; i < length; i++) {
        
        float time = i * samplePeriod;
        float frequency = 1.0 / time;
        
        float filteredSample = applyBandPassFilter(bandPassFilter, signal[i]);
        filteredSignal[i] = filteredSample;
        energyBP += filteredSample * filteredSample;
	
        // Calculer l'énergie totale
        //energyTotal += signal[i] * signal[i];
    }
    // Éviter la division par zéro
    //if (energyTotal == 0.0f) return 0.0f;

    return energyBP;
}

float calculateEnergymag(float* magnitude, float* magnitudefilter,int length, float samplingRate) {
    float energyInBand = 0.0f;
    float energyTotal = 0.0f;

    float freqStep = samplingRate / length;

    for (int i = 0; i < length / 2; i++) {
        float freq = i * freqStep;
        //energyTotal += magnitude[i] * magnitude[i];

        
        energyInBand += magnitudefilter[i] * magnitudefilter[i];
        
    }

    return energyInBand;
}


void computeFFT(float* signal, int length, float* magnitude) {
    kiss_fft_cfg cfg = kiss_fft_alloc(length, 0, NULL, NULL);
    kiss_fft_cpx in[length], out[length];

    for (int i = 0; i < length; i++) {
        in[i].r = signal[i];
        in[i].i = 0.0f;
    }

    kiss_fft(cfg, in, out);

    for (int i = 0; i < length / 2; i++) {
        magnitude[i] = sqrt(out[i].r * out[i].r + out[i].i * out[i].i);
    }

    free(cfg);
}


void generateGroup(Group* group, int length) {
    srand(time(NULL)); // Initialiser le générateur aléatoire

    for (int i = 0; i < length; i++) {
        // Définir la fréquence
        group[i].frequency = (float)(i + 1); // Fréquence entre 1 Hz et length Hz
        if(group[i].frequency >= BAND_LOW && group[i].frequency <= BAND_HIGH){
            group[i].amplitude = 5 + ((float)rand() / RAND_MAX);
        }else{
            group[i].amplitude = ((float)rand() / RAND_MAX); // Entre 0 et 1.0
        }
        group[i].phase = ((float)rand() / RAND_MAX) * 2 * PI ;   
    }
}


void generateTestSignal(float* signal, int length, float freq, float amplitude, float phase) {
    for (int i = 0; i < length; i++) {
        signal[i] += amplitude * sinf(2.0f * PI * freq * i / SAMPLING_RATE + phase);
    }
}

int main() {
    float inputSignal[NUM_SAMPLES] = {0};
    float filteredSignal[NUM_SAMPLES];
    float magnitudeInput[NUM_SAMPLES / 2];
    float magnitudeFiltered[NUM_SAMPLES / 2];
    IIRFilter filter;
    Group Group[NUM_FREQUENCIES];
    /*
    generateGroup(Group, NUM_FREQUENCIES);
    
    for (int i = 0; i < NUM_FREQUENCIES; i++) {
        generateTestSignal(inputSignal, NUM_SAMPLES, Group[i].frequency, Group[i].amplitude,Group[i].phase);
        
    }
    */
    loadBufferFromFile("buffer.txt", inputSignal, NUM_SAMPLES);

    //generateComplexSignal(inputSignal, NUM_SAMPLES, SAMPLING_RATE);
    // Initialiser le filtre passe-bande
    initBandPassFilter(&filter, BAND_LOW, BAND_HIGH, SAMPLING_RATE);

    float energy = calculateEnergy(inputSignal, NUM_SAMPLES, &filter, filteredSignal, SAMPLING_RATE, BAND_LOW, BAND_HIGH);
    
    // Calculer la FFT des signaux
    computeFFT(inputSignal, NUM_SAMPLES, magnitudeInput);
    computeFFT(filteredSignal, NUM_SAMPLES, magnitudeFiltered);

    float energymag = calculateEnergymag(magnitudeInput,magnitudeFiltered, NUM_SAMPLES, SAMPLING_RATE);
    
    // Afficher l energie 
    printf("Énergie en Bande (110–140 Hz) : %.5e\n", energy);

    // Sauvegarder les données pour visualisation
    FILE* file = fopen("output.csv", "w");
    for (int i = 0; i < NUM_SAMPLES / 2; i++) {
        fprintf(file, "%f,%f,%f,%f\n", inputSignal[i], filteredSignal[i], magnitudeInput[i], magnitudeFiltered[i]);
    }
    fclose(file);

    return 0;
}
