#include "Harmonizer.h"
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <thread>

int main() {
    Harmonizer harmonizer("input4.wav", "output.wav", {0,5});
    harmonizer.createChord();
    return 0;
}

// #include <iostream>
// #include <cstdio>
// #include <sndfile.h>
// #include <cstdlib>
// #include <cmath>

// #define BUFFER_LEN 1024
// #define MAX_CHANNELS 6

// static void data_processing (double *data, int count, int channels) ;

// int main (void) {

//   static double data [BUFFER_LEN] ;
//   static double data2 [BUFFER_LEN] ;
//   static double outdata [BUFFER_LEN] ;

//   SNDFILE *infile, *outfile, *infile2 ;
//   SF_INFO sfinfo ;
//   int readcount ;
//   SF_INFO sfinfo2 ;
//   int readcount2 ;

//   const char *infilename = "output0.wav" ;
//   const char *infilename2 = "output1.wav" ;
//   const char *outfilename = "output.wav" ;

//   infile = sf_open (infilename, SFM_READ, &sfinfo);
//   infile2 = sf_open (infilename2, SFM_READ, &sfinfo2);
//   outfile = sf_open (outfilename, SFM_WRITE, &sfinfo);

//   while ((readcount = sf_read_double (infile, data, BUFFER_LEN)) && (readcount2 = sf_read_double (infile2, data2, BUFFER_LEN))) { 
//     data_processing (data, readcount, sfinfo.channels) ;
//     data_processing(data2, readcount2, sfinfo2.channels) ;

//     for(int i=0; i < 1024;++i) {
//     outdata[i] = (data[i] + data2[i]) -(data[i])*(data2[i])/65535;
//     }

//     sf_write_double (outfile, outdata , readcount) ;
//   } ;

//   /* Close input and output files. */
//   sf_close (infile) ;
//   sf_close (infile2) ;
//   sf_close (outfile) ;
//   return 0 ;
// }

// static void data_processing(double *data, int count, int channels) { 
//   double channel_gain [MAX_CHANNELS] = { 1, 1, 1, 1, 1, 1 } ;
//   int k, chan ;

//   for (chan = 0 ; chan < channels ; chan ++)
//     for (k = chan ; k < count ; k+= channels)
//       data [k] *= channel_gain [chan] ;

//   return ;
// }