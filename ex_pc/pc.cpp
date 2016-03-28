#include "stdio.h"
#include "ap_int.h"
#define N 64

void pc(ap_uint<N> x, int &num) {
  num = 0;
  for (int i = 0; i < N; i++) {
#pragma HLS UNROLL
    num += x[i];
  }
}

int main() {
  int x[3] = {0x21, 0x14, 0x1133};
  int num[3];
  for (int i=0; i<3; i++) {
    pc(x[i],num[i]);
    printf("count(0x%x) = %d\n", x[i], num[i]);
  }
  return 0;
}
