//========================================================================
// PopCount
//========================================================================

#include <stdio.h>
#include "ap_int.h"

//------------------------------------------------------------------------
// PopCount Module
//------------------------------------------------------------------------
// We will be using HLS to turn this into RTL.

void PopCount( ap_uint<64> in, int& out )
{
  out = 0;
  for ( int i = 0; i < 64; i++ ) {
    #pragma HLS UNROLL
    out += in[i];
  }
}

//------------------------------------------------------------------------
// main
//------------------------------------------------------------------------
// We include a simple ad-hoc test.

int main()
{
  int in[3] = { 0x21, 0x14, 0x1133 };

  for ( int i = 0; i < 3; i++ ) {
    int out;
    PopCount( in[i], out );
    printf( "popcount(0x%x) = %d\n", in[i], out );
  }

  return 0;
}

