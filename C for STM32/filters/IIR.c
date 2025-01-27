// IIR Filter ////////////////////////////////////////////////////////////////////////////////////////


// see .h for coeff definition and different shape of filters

#include "FIR_IIR.h"



// coeffs for a chebychev1 lowpass order4  ripple 0.3, center 120 hz @ 48kHz
/*
#define NBQ 2
#define XYV_size 14
REAL biquada[]={0.9939363481059377,-1.9936762185610304,0.9854229553653764,-1.9853265743477273};
REAL biquadb[]={1,2,1,2};
REAL gain=638841315.2583077;
REAL xyv[]={0,0,0,0,0,0,0,0,0};
*/

// coeffs for a chebychev1 bandpass order4 center 120 hz and width 40 @ 48kHz
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
