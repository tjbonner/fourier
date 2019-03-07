#include <stdint.h>
#include "timeprocessing.h"
#include "globals.h"


#define FIR_FILTER_TAP_COUNT 121
#define DECIMATION_FACTOR 10



double reorderData(){

}

/***********************************************/
/* windowData will accept a point to an array  */
/* of doubles and will multiply each value     */
/* with a corresponding window weight found    */
/* in the globals.c file. The pointer to the   */
/* data array will now hold the windowed data. */
/***********************************************/
void windowData(float *data[]){
    for(uint16_t i=0; i<sizeof(data); i++){
        data[i] = data[i] * window_weights[i];
    }
}

double antiAliasFilter(){
    float y = 0; //value for y queue
    for(uint32_t i = 0; i < FIR_COEF_COUNT; i++) // Iterates through the size of the x queue.
    {
        y += queue_readElementAt(&xQueue, FIR_COEF_COUNT - i - OFFSET)*firCoefficients[i]; // Convolves x queue with the filter coefficients.
    }
    queue_overwritePush(&yQueue, y); // Add the computed value to the y queue.
    return y; //Return the value as a double.
}

static uint16_t firDecimationCount = 0;
double decimateData(){
    if (firDecimationCount == DECIMATION_FACTOR - 1)
    {
        antiAliasFilter(); // Run antialias filter.
        firDecimationCount = 0; // Reset decimation count.
        return true; // Ran the filter.
    }
    firDecimationCount++; // Increment the decimation count each time the filter fuction is called.
    return false; // Didn't run the filter.
}
