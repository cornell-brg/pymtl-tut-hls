#include "stdio.h"
#include "ap_int.h"
#define N 64

void pc(ap_uint<N> x, int &num) {
  num = 0;
  for (int i = 0; i < N; i++) {
#pragma HLS UNROLL
    num += x[i];
  }
  //return num;
}

int main() {
  int x = 0x49b34a8;
  int num;
  pc(x,num);
  printf("count(%d) = %d\n", x, num);
  return 0;
}
