#ifndef PARALLEL_PROJECTION_H
#define PARALLEL_PROJECTION_H

#include <math.h>
#include <algorithm>
#include <cfloat>
#include <limits>
#include <sstream>
#include <vector>
#include "DisplayFile.cc"
#include "Globals.cc"
#include "GraphicElement.cc"
#include "Point.cc"
#include "Projection.cc"
#include "Transformer.cc"
#include "Window.cc"

class ParallelProjection : public Projection {
 public:
  ParallelProjection(){};
  virtual ~ParallelProjection(){};

  void project(DisplayFile* dp, Window* window) {
    Matrix move, rotateX, rotateY, rotateZ, result;

    // View reference point
    Point VRP = Point(window->origin.x, window->origin.y, window->origin.z);
    // view plane normal (a.k.a window normal)
    // Point VPN = Point(window->n.x, window->n.y, window->n.z);

    // 1.	Translade VRP para a origem
    move = Transformer::make3DMoveMatrix(window->origin * (-1));

    // 2.	Determine VPN: Decomponha e determine os ângulos de VPN com X e
    // double xAngle = atan(window->n.y / window->n.z);
    // double yAngle = atan(window->n.x / window->n.z);

    // window->xAngle = -xAngle;
    // window->yAngle = -yAngle;

    // double xAngle = Transformer::angleBetweenVectors(window->n, Point(1, 0,
    // 0)); double yAngle = Transformer::angleBetweenVectors(window->n, Point(0,
    // 1, 0));

    // 3.	Rotacione o mundo em torno de X e Y de forma a alinhar VPN com Z
    rotateX = Transformer::make3DRotateMatrixX(
        Transformer::degreesToRadians(window->xAngle));
    rotateY = Transformer::make3DRotateMatrixY(
        Transformer::degreesToRadians(window->yAngle));
    rotateZ = Transformer::make3DRotateMatrixZ(
        Transformer::degreesToRadians(window->zAngle));

    result = (move * rotateX) * rotateY * rotateZ;

    // Move all the world
    auto it = dp->elements.begin();
    for (; it != dp->elements.end(); ++it) {
      bool isCurve = it->second->type == GraphicType::CURVE_BEZIER ||
                     it->second->type == GraphicType::CURVE_SPLINE;
      Transformer::transform3DPoints(it->second, result, !isCurve);
    }

    // // Move the window itself
    // Transformer::transform3DPoint(&window->origin, result);
    // Transformer::transform3DPoint(&window->v, result);
    // Transformer::transform3DPoint(&window->u, result);
    // Transformer::transform3DPoint(&window->n, result);
  }
};

#endif  // PARALLEL_PROJECTION_H
