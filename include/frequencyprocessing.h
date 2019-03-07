#ifndef FREQUENCYPROCESSING_H_    /* Include guard */
#define FREQUENCYPROCESSING_H_

/*************************************************/
/* We only need to worry about the postive       */
/* frequency output of the FFT, this will remove */
/* the negative frequencies from our array. Call */
/* this twice, once for the real portion and     */
/* once for the imaginary portion.               */
/*************************************************/
float keepPositiveFreq(float *data[], uint16_t N);


double calculateMagSquared(float real_data[], float imag_data, uint16_t N, uint32_t fs);


double findPeak();


double interpolate();


double calculatePower();

#endif  // FREQUENCYPROCESSING_H_
