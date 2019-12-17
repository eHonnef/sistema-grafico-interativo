#ifndef POINTCLIP_H
#define POINTCLIP_H

#include <algorithm>
#include <vector>
#include "ClippingAlgorithm.cc"
#include "Globals.cc"

class PointClip : public ClippingAlgorithm {
 public:
  // GraphicType getType() {}
  void clip(std::vector<Point>* points) {
    points->erase(std::remove_if(points->begin(), points->end(),
                                 [](Point p) {
                                   return (p.x < xMin || p.x > xMax ||
                                           p.y < yMin || p.y > yMax);
                                 }),
                  points->end());
  }
};
#endif  // !POINTCLIP_H