#ifndef PRED_POINT_H
#define PRED_POINT_H

#include "../Types.h"
#include "../Iterator.h"

struct PredPoint_GT_0 {
  static bool run (Point p) {
    return (p.x > 0 && p.y > 0);
  }
};

struct PredPoint_LT_0 {
  static bool run (Point p) {
    return (p.x < 0 && p.y < 0);
  }
};

struct PredPoint_EQ_0 {
  static bool run (Point p) {
    return (p.x == 0 && p.y == 0);
  }
};

#endif
