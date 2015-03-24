#include <assert.h>
#include <stdio.h>
#define N 10

#include "../common/common.h"

typedef char MyType;
typedef _iterator<MyType> iterator;

ItuIfaceType g_itu_iface;

iterator findif (iterator begin, iterator end, ap_uint<3> pred_val) {
  bool stop = false;
  for (; begin != end; ++begin) {
    // 
    switch (pred_val) {
      case 0:
        if (*begin > 0) stop = true;
        break;
      case 1:
        if (*begin < 0) stop = true;
        break;
      case 2:
        if (*begin == 0) stop = true;
        break;
      case 3:
        if (*begin % 2 != 0) stop = true;
        break;
      case 4:
        if (*begin % 2 == 0) stop = true;
        break;
    };
    // exit loop of pred satisfied
    if (stop) {
      break;
    }
  }
  return begin;
}

int main () {
  MyType myarray[N];
  for (int i = 0; i < N; ++i) myarray[i] = i;

  iterator begin (0, 0);
  iterator end   (0, N);

  iterator res;

  // odd int
  res = findif(begin, end, 3);
  printf ("%s\n", (res!=end)?"Found":"Not Found");
  // equal 0
  res = findif(begin, end, 2);
  printf ("%s\n", (res!=end)?"Found":"Not Found");

  return 0;
}
