# fourier
Raspberry Pi implementation of power spectrum analysis

Much of the code included in this repository is pulled from the following github repositories:
prashantrar/pi_gpu_fft_wrapper
apcragg/fmcw_radar

To get GPU_FFT enter the following at the command prompt:

sudo rpi-update && sudo reboot

To build and run the example program:

cd /opt/vc/src/hello_pi/hello_fft
make
sudo mknod char_dev c 100 0
sudo ./hello_fft.bin

API documentation can be found in the hello_fft folder.
