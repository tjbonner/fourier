#ifndef GLOBALS_H_   /* Include guard */
#define GLOBALS_H_

#define WINDOW_SIZE 50000

extern float window_weights[WINDOW_SIZE];

/**********************************************/
/* The FFT code that our system uses can only */
/* handle lengths that are a power of 2,      */
/* and it's still faster anyways, so we need  */
/* to find out how many samples we have and   */
/* return the next power of 2 so that we can  */
/* zero-pad the data before sending it to the */
/* fft function. This function will also be   */
/* used anywhere else we need a power of 2.   */
/**********************************************/
int nextPow2(uint16_t N);

#endif  // GLOBALS_H_
