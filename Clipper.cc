#ifndef CLIPPER_H
#define CLIPPER_H

#include "ClippingAlgorithm.cc"
#include "Point.cc"

class Clipper {
 public:
  Clipper() {}
  virtual ~Clipper() {}
  static void clip(ClippingAlgorithm* ca, std::vector<Point>* points) {
    ca->clip(points);
  }
};

#endif
