#include "ap_utils.h"
#include <hls_stream.h>
void Child( hls::stream<int> req,
            hls::stream<int> resp  )
{
  int msg =  req.read();
  resp.write( msg + 1 );
}
