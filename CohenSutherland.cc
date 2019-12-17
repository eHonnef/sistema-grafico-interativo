#ifndef COHENSUTHERLAND_H
#define COHENSUTHERLAND_H
#include <vector>
#include "ClippingAlgorithm.cc"
#include "Globals.cc"
#include "Point.cc"

class CohenSutherland : public ClippingAlgorithm {
 public:
  void clip(std::vector<Point>* points) {
    std::vector<Point> new_points;
    Point p0, p1;
    for (unsigned i = 0, j = 1; j < points->size(); ++i, ++j) {
      p0 = points->at(i);
      p1 = points->at(j);
      if (cohenSutherlandRun(p0.x, p0.y, p1.x, p1.y)) {
        if (!isInside(new_points, p0)) {
          new_points.push_back(Point(p0));
        }

        if (!isInside(new_points, p1)) {
          new_points.push_back(Point(p1));
        }
      }
    }
    points->assign(new_points.begin(), new_points.end());
  }

 private:
  bool isInside(const std::vector<Point>& points, const Point& p) {
    return std::find(points.begin(), points.end(), p) != points.end();
  }

  typedef int OutCode;

  const int INSIDE = 0;  // 0000
  const int LEFT = 1;    // 0001
  const int RIGHT = 2;   // 0010
  const int BOTTOM = 4;  // 0100
  const int TOP = 8;     // 1000

  // Compute the bit code for a point (x, y) using the clip rectangle
  // bounded diagonally by (xmin, ymin), and (xmax, ymax)

  // ASSUME THAT xmax, xmin, ymax and ymin are global constants.

  OutCode ComputeOutCode(double x, double y) {
    OutCode code;

    code = INSIDE;  // initialised as being inside of [[clip window]]

    if (x < xMin)  // to the left of clip window
      code |= LEFT;
    else if (x > xMax)  // to the right of clip window
      code |= RIGHT;
    if (y < yMin)  // below the clip window
      code |= BOTTOM;
    else if (y > yMax)  // above the clip window
      code |= TOP;

    return code;
  }

  bool cohenSutherlandRun(double& x0, double& y0, double& x1, double& y1) {
    // compute outcodes for P0, P1, and whatever point lies outside the clip
    // rectangle
    OutCode outcode0 = ComputeOutCode(x0, y0);
    OutCode outcode1 = ComputeOutCode(x1, y1);
    bool accept = false;

    while (true) {
      if (!(outcode0 | outcode1)) {
        // bitwise OR is 0: both points inside window; trivially accept and exit
        // loop
        accept = true;
        break;
      } else if (outcode0 & outcode1) {
        // bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT,
        // TOP, or BOTTOM), so both must be outside window; exit loop (accept is
        // false)
        break;
      } else {
        // failed both tests, so calculate the line segment to clip
        // from an outside point to an intersection with clip edge
        double x, y;

        // At least one endpoint is outside the clip rectangle; pick it.
        OutCode outcodeOut = outcode0 ? outcode0 : outcode1;

        // Now find the intersection point;
        // use formulas:
        //   slope = (y1 - y0) / (x1 - x0)
        //   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
        //   y = y0 + slope * (xm - x0), where xm is xmin or xmax
        // No need to worry about divide-by-zero because, in each case, the
        // outcode bit being tested guarantees the denominator is non-zero
        if (outcodeOut & TOP) {  // point is above the clip window
          x = x0 + (x1 - x0) * (yMax - y0) / (y1 - y0);
          y = yMax;
        } else if (outcodeOut & BOTTOM) {  // point is below the clip window
          x = x0 + (x1 - x0) * (yMin - y0) / (y1 - y0);
          y = yMin;
        } else if (outcodeOut &
                   RIGHT) {  // point is to the right of clip window
          y = y0 + (y1 - y0) * (xMax - x0) / (x1 - x0);
          x = xMax;
        } else if (outcodeOut & LEFT) {  // point is to the left of clip window
          y = y0 + (y1 - y0) * (xMin - x0) / (x1 - x0);
          x = xMin;
        }

        // Now we move outside point to intersection point to clip
        // and get ready for next pass.
        if (outcodeOut == outcode0) {
          x0 = x;
          y0 = y;
          outcode0 = ComputeOutCode(x0, y0);
        } else {
          x1 = x;
          y1 = y;
          outcode1 = ComputeOutCode(x1, y1);
        }
      }
    }
    return accept;
  }
};

#endif