#include "ap_utils.h"

#include "../hls/include/Types.h"

// ------------------------------------------------------------------
// Saccilized Processing Element
// Compare Point Equal to Zero
// ------------------------------------------------------------------
void EqZero (PeIfaceType &ac) {
  PeReqMsg  req;
  Point p;

  // read x value
  ac.req.read( req );
  p.x = req.data;
  ac.resp.write( PeRespMsg( req.id, req.type, 0 ) );

  // read y value
  ac.req.read( req );
  p.y = req.data;
  ac.resp.write( PeRespMsg( req.id, req.type, 0 ) );

  // write back result
  ac.req.read( req );
  PeDataType data = p == 0 ? 1 : 0;
  ac.resp.write( PeRespMsg( req.id, req.type, data ) );
}

int test_point (Point p) {
  PeIfaceType aciface;
  PeRespMsg resp;

  // 1. send x value
  aciface.req.write( PeReqMsg( 1, MSG_WRITE, 0) );
  // 2. send y value
  aciface.req.write( PeReqMsg( 1, MSG_WRITE, 1) );
  // 3. read result
  aciface.req.write( PeReqMsg( 1, MSG_READ, 0) );

  EqZero( aciface );

  aciface.resp.read( resp );
  aciface.resp.read( resp );
  aciface.resp.read( resp );

  return resp.data;
}

int main () {
  Point p1(0,1);
  int res1 = test_point( p1 );
  Point p2(3,0);
  int res2 = test_point( p2 );
  Point p3(0,0);
  int res3 = test_point( p3 );

  printf ("--------------------\n");
  printf ("Result 1: %d\n", res1);
  printf ("Result 2: %d\n", res2);
  printf ("Result 3: %d\n", res3);
  printf ("--------------------\n");
}
