#ifndef PREPROCESSING_H_    /* Include guard */
#define PREPROCESSING_H_

#define POW10 1024
#define POW11 2048
#define POW12 4096
#define POW13 8192
#define POW14 16384
#define POW15 32768
#define POW16 65536
#define POW17 131072
#define POW18 262144
#define POW19 524288
#define POW20 1048576
#define POW21 2097152

/**********************************************/
/* The output of the ADC does not have the    */ 
/* data in the correct order, so before and   */
/* processing can be done, the order must     */
/* be corrected.                              */
/**********************************************/
double reorderData();

/**********************************************/
/* In order to remove multipath with a higher */
/* resolution than our ADC affords we will    */
/* use a parabolic interpolation later on.    */
/* By windowing the data, we can take         */
/* advantage of the peak smearing to get      */
/* better points for our parabolic fit.       */
/**********************************************/
void windowData(double **data);

/**********************************************/
/* We are sampling way above what the beat    */
/* frequency will be, and while this allows   */
/* for future extension of the system, right  */
/* now we want to increase computational      */
/* efficiency by only keeping 1/3 of our      */
/* sampled data, and to do this we need to    */
/* first anti-alias filter the data.          */
/**********************************************/
double antiAliasFilter();

/**********************************************/
/* Now that the data has been passed through  */
/* the anti-alias filter, we will decimate    */
/* by three (keep every third sample).        */
/**********************************************/
double decimateData();

/**********************************************/
/* The FFT code that our system uses can only */
/* handle lengths that are a power of 2,      */
/* and it's still faster anyways, so we need  */
/* to find out how many samples we have and   */
/* return the next power of 2 so that we can  */
/* zero-pad the data before sending it to the */
/* fft function.                              */
/**********************************************/
int nextPow2();

#endif  // PREPROCESSING_H_
