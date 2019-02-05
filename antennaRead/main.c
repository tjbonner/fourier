#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "computefft.h"

int main(){
	int numData = 8000;
	int zeroPad = 8192*2;
	double data[zeroPad];
	FILE *dataIn;
    FILE *dataOut;

	dataIn = fopen("testSignal.txt","r");
	if(dataIn == NULL){
		printf("Cannot open file\n\r");
		return -1;
	}

	int i,j;
	double buffer;
	for(i=0; i<numData; i++){
		fscanf(dataIn, "%lf", &buffer);
		data[i] = buffer;
	}
    for(j=i; j<zeroPad; j++) data[j] = 0;

    fclose(dataIn);
	computefft(data, 13);

    dataOut = fopen("signalOut.txt","w");
    if(dataOut == NULL){
        printf("Cannot create file\n\r");
        return -2;
    }

    for(i=0; i<zeroPad; i++){
        //printf("loop #: %d, real value: %0.3lf\n", i, data[i]);
        fprintf(dataOut, "%0.3lf %0.3lf\n", data[i], data[i++]);
    }

	return 0;
}
