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

//---------------------------------------------------------
// Unsigned template specializations (to avoid warnings)
//---------------------------------------------------------
template <>
struct Pred_LT_0<unsigned char> {
  static bool run (unsigned char x) {
    return true;
  }
};

template <>
struct Pred_LT_0<unsigned short> {
  static bool run (unsigned short x) {
    return true;
  }
};

template <>
struct Pred_LT_0<unsigned> {
  static bool run (unsigned x) {
    return true;
  }
};

#endif
