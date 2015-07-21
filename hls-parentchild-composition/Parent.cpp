#include "ap_utils.h"
#include <hls_stream.h>
void Parent( hls::stream<int>& req,
             hls::stream<int>& resp )
{

#pragma HLS PIPELINE
  for ( int i = 0; i < 10; ++i ) {
    req.write( i );
    ap_wait();
    resp.read();
  }
}
