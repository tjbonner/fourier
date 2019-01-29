#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mailbox.h"
#include "gpu_fft.h"
#include "computefft.h"

int main(){
	int numData = 8000;
	int zeroPad = 8192;
	double sigData[zeroPad];
	FILE *myFile;
	myFile = fopen("testSignal.txt","r");
	if(myFile == NULL){
		printf("Cannot open file\n\r");
		return -1;
	}

	int i;
	double buffer;
	for(i=0; i<numData; i++){
		fscanf(myFile, "%lf", &buffer);
		sigData[i] = buffer;
		//printf("%.15f ", sigData[i]);
	}

    /*
	for(int j=numData; j<zeroPad; j++){
		sigData[j] = 0;
		printf("%.15f ", sigData[j]);
	}
    */

	double* data = sigData;
	computefft(data, 13);

	return 0;
}
