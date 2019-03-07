#include <stdint.h>
#include "frequencyprocessing.h"
#include "globals.h"

struct signal calculateMagSquared(struct signal real_data, struct signal imag_data){
    struct signal psdx;
    psdx.length = real_data.length;
    psdx.fs = real_data.fs;
    float scaler = 1/(psdx.fs*psdx.N);
    for(uint16_t i=0; i<psdx.length; i++){
        real_data.samples[i] *= real_data.samples[i];
        imag_data.samples[i] *= imag_data.samples[i];
        if(i != 0 && i != N){
            psdx.samples[i] = 2*scaler*(real_data.samples[i]+imag_data.samples[i]);
        } 
        else
        {
            psdx.samples[i] = scaler*(real_data.samples[i]+imag_data.samples[i]);
        }
    }
    return psdx;
}

struct max_values findPeak(struct signal psdx){
    struct max_values val;
    float max_value = 0;
    float left_value;
    float right_value;
    float actual_max_value;
    float actual_max_frequency;
    uint16_t max_frequency;
    uint16_t left_index;
    uint16_t right_index;
    float tmp;

    for(uint16_t i=0; i<psdx.length; i++){
        tmp = psdx.samples[i];
        if(tmp > max_value){
            max_value = tmp;
            max_frequency = i;
        }
    }

    left_value = psdx.samples[max_frequency-1];
    right_value = psdx.samples[max_frequency+1];
    actual_max_frequency = (left_value - right_value)/(2*(left_value + right_value - 2*max_value));
    actual_max_value = max_value - (left_value - right_value)*actual_max_frequency/4;
    actual_max_frequency += max_frequency;
    val.actual_max_frequency = actual_max_frequency;
    val.actual_max_value = actual_max_value;
}


double interpolate();


double calculatePower();