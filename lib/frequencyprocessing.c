#include <stdint.h>
#include "frequencyprocessing.h"
#include "globals.h"

/**************************************************/
/* We can probably remove this if we use a struct */
/* with a length member that we update to be half */
/* of what it was previously. I'll do this when   */
/* we're closer to complete functionality.        */
/**************************************************/
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

float findPeak(float psdx[], uint16_t N){
    float max_value = 0;
    float left_value;
    float right_value;
    float actual_max_value;
    float actual_max_frequency;
    uint16_t max_frequency;
    uint16_t left_index;
    uint16_t right_index;
    float tmp;

    for(uint16_t i=0; i<N; i++){
        tmp = psdx[i];
        if(tmp > max_value){
            max_value = tmp;
            max_frequency = i;
        }
    }
    left_value = psdx[max_frequency-1];
    right_value = psdx[max_frequency+1];
    actual_max_frequency = (left_value - right_value)/(2*(left_value + right_value - 2*max_value));
    actual_max_value = max_value - (left_value - right_value)*actual_max_frequency/4;
    actual_max_frequency += max_frequency;
}


double interpolate();


double calculatePower();