#include <stdint.h>
#include "frequencyprocessing.h"
#include "globals.h"


float keepPositiveFreq(float data[], uint16_t N){
    uint16_t len = N/2 + 1;
    float positive_half[len];
    for(uint16_t i=0; i<len; i++)
        positive_half[i] = data[i];
    return positive_half;
}


float calculateMagSquared(float real_data[], float imag_data, uint16_t N, uint32_t fs){
    float scaler = 1/(fs*N);
    float psdx[N];
    for(uint16_t i=0; i<N; i++){
        real_data[i] *= real_data[i];
        imag_data[i] *= imag_data[i];
        if(i != 0 && i != N){
            psdx[i] = 2*scaler*(real_data[i]+imag_data[i]);
        } 
        else
        {
            psdx[i] = scaler*(real_data[i]+imag_data[i]);
        }
    }
    return psdx;
}


double findPeak();


double interpolate();


double calculatePower();