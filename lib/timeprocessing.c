#include <stdint.h>
#include "timeprocessing.h"
#include "globals.h"

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

}

double decimateData(){

}
