#ifndef CLIPPINGALGORITHM_H
#define CLIPPINGALGORITHM_H

#include <cfloat>
#include <vector>
#include "GraphicType.cc"
#include "Point.cc"

class ClippingAlgorithm {
 public:
  // virtual ~ClippingAlgorithm() = 0;
  // virtual GraphicType getType() = 0;
  virtual void clip(std::vector<Point>* points) = 0;
};

#endif
