#ifndef PRED_H
#define PRED_H

#include "../Types.h"
#include "../Iterator.h"

template <typename T>
struct Pred_GT_0 {
  static bool run (T x) {
    return (x > 0);
  }
};

template <typename T>
struct Pred_LT_0 {
  static bool run (T x) {
    return (x < 0);
  }
};

template <typename T>
struct Pred_EQ_0 {
  static bool run (T x) {
    return (x == 0);
  }
};

#endif
