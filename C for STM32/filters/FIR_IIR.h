// IIR Filter ////////////////////////////////////////////////////////////////////////////////////////


#ifndef FIR_IIR_filter_H
#define FIR_IIR_filter_H


int16_t FIR_filter(int16_t x, char shift);

typedef enum {ONES} FIR_type; 
void FIR_init_coeff(FIR_type type);
// ^ allows for different initialisations
// "ONES" turns the FIR into a moving average 

float IIR_filter(float v);

#endif

