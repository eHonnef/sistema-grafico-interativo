#ifndef POINT_H
#define POINT_H

#include <math.h>
#include <algorithm>
#include <cfloat>
#include <limits>
#include <vector>

class Point {
 public:
  double x;
  double y;
  double z;

  Point(double x = 0, double y = 0, double z = 0) {
    this->x = x;
    this->y = y;
    this->z = z;
  };
  virtual ~Point(){};

  bool operator==(const Point& p0) const {
    bool x = fabs(p0.x - this->x) < DBL_EPSILON;
    bool y = fabs(p0.y - this->y) < DBL_EPSILON;
    bool z = fabs(p0.z - this->z) < DBL_EPSILON;

    return x && y && z;
  }

  Point operator-(const Point& p0) const {
    return Point(this->x - p0.x, this->y - p0.y, this->z - p0.z);
  }

  Point operator+(const Point& p0) const {
    return Point(this->x + p0.x, this->y + p0.y, this->z + p0.z);
  }

  Point operator+(const double k) const {
    return Point(this->x + k, this->y + k, this->z + k);
  }

  Point operator*(const double k) const {
    return Point(this->x * k, this->y * k, this->z * k);
  }

  void copy(Point point) {
    this->x = point.x;
    this->y = point.y;
    this->z = point.z;
  }
};

#endif  // POINT_H

#ifndef POINTSOPERATIONS_H
#define POINTSOPERATIONS_H

class PointsOperations {
 public:
  static bool isClockwiseOriented(const std::vector<Point>& points) {
    double sum = 0;

    for (auto it_next = points.begin(), it = it_next++; it_next != points.end();
         it++, it_next++)
      sum += (it_next->x - it->x) * (it_next->y + it->y);

    sum += (points.front().x - points.back().x) *
           (points.front().y + points.back().y);

    return sum > 0;
  }

  static void reversePoints(std::vector<Point>* points) {
    std::reverse(points->begin(), points->end());
  }

  static bool isPointsEqual(const Point& p0, const Point& p1) {
    bool x = fabs(p0.x - p1.x) < DBL_EPSILON;
    bool y = fabs(p0.y - p1.y) < DBL_EPSILON;
    bool z = fabs(p0.z - p1.z) < DBL_EPSILON;

    return x && y && z;
  }

  static bool compareDoubles(double d0, double d1) {
    return fabs(d0 - d1) < DBL_EPSILON;
  }

  static Point intersection(const Point& p0, const Point& p1, const Point& q0,
                            const Point& q1) {
    double numx = (p0.x * p1.y - p0.y * p1.x) * (q0.x - q1.x) -
                  (p0.x - p1.x) * (q0.x * q1.y - q0.y * q1.x);
    double denx = (p0.x - p1.x) * (q0.y - q1.y) - (p0.y - p1.y) * (q0.x - q1.x);

    double numy = (p0.x * p1.y - p0.y * p1.x) * (q0.y - q1.y) -
                  (p0.y - p1.y) * (q0.x * q1.y - q0.y * q1.x);
    double deny = (p0.x - p1.x) * (q0.y - q1.y) - (p0.y - p1.y) * (q0.x - q1.x);

    return Point(numx / denx, numy / deny);
  }
};

#endif  // !POINTSOPERATIONS_H
