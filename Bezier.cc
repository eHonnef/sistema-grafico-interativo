#ifndef BEZIER_H
#define BEZIER_H

#include <stdexcept>
#include <string>
#include <vector>
#include "Globals.cc"
#include "GraphicElement.cc"
#include "Point.cc"

class Bezier {
 public:
  Bezier(){};
  virtual ~Bezier(){};

  // Source of formula: http://javascript.info/bezier-curve
  // P = (1−t)^3*P1 + 3*(1−t)^2*t*P2 + 3*(1−t)*t^2*P3 + t^3*P4
  static void generatePoints(std::vector<Point>* points, int steps = 100) {
    if (steps < 1)
      throw std::invalid_argument("Bezier curve expects at least 1 step!");

    if (points->size() != 4)
      throw std::invalid_argument("Bezier curve must have 4 points!");

    std::vector<Point> new_points;
    double x, y, step;
    step = (1.0 / steps);
    double P1x, P2x, P3x, P4x, P1y, P2y, P3y, P4y;

    P1x = points->at(0).x;
    P2x = points->at(1).x;
    P3x = points->at(2).x;
    P4x = points->at(3).x;

    P1y = points->at(0).y;
    P2y = points->at(1).y;
    P3y = points->at(2).y;
    P4y = points->at(3).y;

    for (double t = 0.0; t < 1.0; t += step) {
      x = ((1 - t) * (1 - t) * (1 - t) * P1x) +
          (3 * ((1 - t) * (1 - t)) * t * P2x) + (3 * (1 - t) * (t * t) * P3x) +
          ((t * t * t) * P4x);

      y = ((1 - t) * (1 - t) * (1 - t) * P1y) +
          (3 * ((1 - t) * (1 - t)) * t * P2y) + (3 * (1 - t) * (t * t) * P3y) +
          ((t * t * t) * P4y);

      new_points.push_back(Point(x, y));
    }

    points->assign(new_points.begin(), new_points.end());
  }
};

#endif  // BEZIER_H
