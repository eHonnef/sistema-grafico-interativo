#ifndef SUTHERLANDHODGMAN_H
#define SUTHERLANDHODGMAN_H

#include <math.h>
#include <algorithm>
#include <vector>
#include "ClippingAlgorithm.cc"
#include "Globals.cc"
#include "Point.cc"

class SutherlandHodgman : public ClippingAlgorithm {
 public:
  void clip(std::vector<Point>* points) {
    if (!PointsOperations::isClockwiseOriented(*points))
      PointsOperations::reversePoints(points);

    std::vector<Point> window({Point(xMin, yMin), Point(xMin, yMax),
                               Point(xMax, yMax), Point(xMax, yMin)});
    // i and k are two consecutive indexes
    for (unsigned i = 0; i < window.size(); i++) {
      unsigned k = (i + 1) % window.size();

      // We pass the current array of vertices
      // and the end points of the selected clipper line
      doClip(points, window[i], window[k]);
    }
  }

 private:
  // This functions clips all the edges w.r.t one clip
  // edge of clipping area
  void doClip(std::vector<Point>* polygon, const Point& p0, const Point& p1) {
    std::vector<Point> new_points;
    // (ix,iy),(kx,ky) are the co-ordinate values of
    // the points
    for (unsigned i = 0; i < polygon->size(); i++) {
      // i and k form a line in polygon
      unsigned k = (i + 1) % polygon->size();

      double ix = polygon->at(i).x, iy = polygon->at(i).y;
      double kx = polygon->at(k).x, ky = polygon->at(k).y;

      // Calculating position of first point
      // w.r.t. clipper line
      double i_pos = (p1.x - p0.x) * (iy - p0.y) - (p1.y - p0.y) * (ix - p0.x);

      // Calculating position of second point
      // w.r.t. clipper line
      double k_pos = (p1.x - p0.x) * (ky - p0.y) - (p1.y - p0.y) * (kx - p0.x);

      // Case 1 : When both points are inside
      if (i_pos < 0 && k_pos < 0) {
        // Only second point is added
        new_points.push_back(Point(kx, ky));
      }

      // Case 2: When only first point is outside
      else if ((i_pos > 0 || PointsOperations::compareDoubles(i_pos, 0)) &&
               k_pos < 0) {
        // Point of intersection with edge
        // and the second point is added
        new_points.push_back(PointsOperations::intersection(
            p0, p1, Point(ix, iy), Point(kx, ky)));
        new_points.push_back(Point(kx, ky));
      }

      // Case 3: When only second point is outside
      else if (i_pos < 0 &&
               (k_pos > 0 || PointsOperations::compareDoubles(k_pos, 0))) {
        // Only point of intersection with edge is added
        new_points.push_back(PointsOperations::intersection(
            p0, p1, Point(ix, iy), Point(kx, ky)));
      }

      // Case 4: When both points are outside
      else {
        // No points are added
      }
    }

    // Copying new points into original array
    polygon->assign(new_points.begin(), new_points.end());
  }
};

#endif
