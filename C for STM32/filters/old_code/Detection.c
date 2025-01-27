//
// Created by maaloton 12/12/2023.
//

#include <stdio.h>
#include "Detection.h"

/*Critère :
* frelon -> f0 entre 110Hz et 135Hz -> SNR <2.2
* abeille -> f0 entre 148Hz et 182Hz -> SNR > 3
* mouche -> f0 entre 190Hz et 210Hz -> SNR > 3
* */
//retour : 1 = frelon, 2 = abeille, 3 = mouche , -1 = Non trouvé
int detection(int nbpeak,int *IndicePic, float *freqAll, float *AmpALL){
    int i=0;
    float f0=0;
    float N0=0;
    int newNbpeak=0;


 //ENTRE 200 ET 1000HZ


    while(freqAll[IndicePic[i]]<1000   && i<nbpeak) {

            if(freqAll[IndicePic[i]]>200 && fabs(IndicePic[i]-IndicePic[i+1])>10){    // on verifie que 2 indices ne réprésente pas le même pic 
                f0 += freqAll[IndicePic[i + 1]] - freqAll[IndicePic[i]];
                N0+=AmpALL[IndicePic[i+1]]-AmpALL[IndicePic[i]];
                newNbpeak++;
            }
            i++;

        }
        f0/=(float)newNbpeak;
        N0/=(float)newNbpeak;


//ENTRE 1000 ET 2000HZ
    i=0;
    int cpt=0,j=0,k=0,l=0;
    float maxAmp=0,bruit=0,SNR=0,N1=0,bruit2=0;
    while((float)i*f0<=1000)i++;
    while((float)i*f0>=1000 && (float)i*f0<=2000){
        while(freqAll[j]<=(float)i*f0-1 )j++;
        maxAmp = AmpALL[j-1] > AmpALL[j] ? (AmpALL[j-1] > AmpALL[j+1] ? AmpALL[j-1] : AmpALL[j+1]) : (AmpALL[j] > AmpALL[j+1] ? AmpALL[j] : AmpALL[j+1]);
        while(freqAll[k]<=(i-0.5)*f0-1 )k++;
        while(freqAll[l]<=(i+0.5)*f0-1 )l++;
        bruit = AmpALL[k-1] < AmpALL[k] ? (AmpALL[k-1] < AmpALL[k+1] ? AmpALL[k-1] : AmpALL[k+1]) : (AmpALL[k] < AmpALL[k+1] ? AmpALL[k] : AmpALL[k+1]);
        bruit2 = AmpALL[l-1] < AmpALL[l] ? (AmpALL[l-1] < AmpALL[l+1] ? AmpALL[l-1] : AmpALL[l+1]) : (AmpALL[l] < AmpALL[l+1] ? AmpALL[l] : AmpALL[l+1]);
        bruit=(bruit+bruit2)/2;
        SNR+=maxAmp/bruit;
        N1+=maxAmp;
        i++;
        cpt++;

    }
    N1/=(float)cpt;
    SNR/=(float)cpt;




    if(f0>=110 && f0<=135 && SNR < 2.2) return 1;       //frelon
    else if(f0>=148 && f0 <=182 && SNR > 2.4 ) return 2;  //abeille
    else if (f0 >=190 && f0 <= 210 && SNR > 2.4) return 3;//mouche
    else return -1;

}
