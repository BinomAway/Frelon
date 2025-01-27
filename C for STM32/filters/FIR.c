// FIR cyclic buffer //////////////////////////////////////////////////////////////////

#include "FIR_IIR.h"

#define FIR_N 16
// ^ max buffer size seems to be 128 (at 48kHz sample rate)

int32_t	FIR_x[FIR_N];
int16_t FIR_a[FIR_N];
int16_t FIR_filter(int16_t x, char shift){

	static int k = 0;
	FIR_x[k] = x;

	int s = 0;
	int ii=0;
	for(ii=FIR_N-1; ii>=k; ii--){
		s += FIR_a[ii-k]*FIR_x[ii];  //a[i-k]
	}
	for(ii=0; ii<k; ii++){
		s += FIR_a[FIR_N+ii-k]*FIR_x[ii];  //a[N+i-k]
	}
	s = s >> shift; //dividing coeff (for normalisation purpose)
	int16_t y = (int16_t) (s >> 16);

	if(--k<0){ k+=FIR_N; }

	return y;
}

int FIR_init = 0;
void FIR_init_coeff(FIR_type type){

	switch(type){
	case ONES:
		for(int i=0; i<FIR_N; i++){

				FIR_a[i] = 1;

		}
		break;

	// case OTHER_INIT...
	}

}