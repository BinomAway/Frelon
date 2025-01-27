// IIR Filter ////////////////////////////////////////////////////////////////////////////////////////


#include "FIR_IIR.h"

// COEFFS for lowpass chebyshev1 order 1 with ripple centered at 200 hz for 48000 Hz sampling freq
/*
#define NBQ 1
#define XYV_size 5
float biquada[]={0.9818003581319115,-1.9812314341247965};
float biquadb[]={1,2};
float gain=9653.233352591471;
float xyv[]={0,0,0,0,0,0};
*/


// COEFFS for bandpass chebyshev1 order 2 centered at 150 hz, width=50 for 48000 Hz sampling freq
/*
#define NBQ 2
#define XYV_size 8
float biquada[]={0.9958836743042496,-1.9955905020110396,0.9947513494300885,-1.9942744077599701};
float biquadb[]={1,-2,1,2};
float gain=73077.33128000403;
float xyv[]={0,0,0,0,0,0,0,0,0};
*/


// COEFFS for bandpass chebyshev1 order 4 centered at 100 hz, width=40 for 48000 Hz sampling freq

#define NBQ 4
#define XYV_size 14
float biquada[]={0.9983439863360103,-1.9982320133163622,0.9954728260697103,-1.9953332927617788,0.9946772958747032,-1.9944843359249267,0.9975702713489363,-1.997329123530468};
float biquadb[]={1,-2,1,-2,1,2,1,2};
float gain=19470457182.946293;
float xyv[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


float IIR_filter(float v)
{
	int i,b,xp=0,yp=3,bqp=0;
	float out=v/gain;
	for (i=XYV_size; i>0; i--) {xyv[i]=xyv[i-1];}
	for (b=0; b<NBQ; b++)
	{
		int len=2;
		xyv[xp]=out;
		for(i=0; i<len; i++) { out+=xyv[xp+len-i]*biquadb[bqp+i]-xyv[yp+len-i]*biquada[bqp+i]; }
		bqp+=len;
		xyv[yp]=out;
		xp=yp; yp+=len+1;
	}
	return out;
}
