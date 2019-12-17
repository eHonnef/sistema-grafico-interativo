#ifndef PERSPECTIVE_PROJECTION_H
#define PERSPECTIVE_PROJECTION_H

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

class PerspectiveProjection : public Projection {
 public:
  PerspectiveProjection() {}
  virtual ~PerspectiveProjection() {}
  void project(DisplayFile* dp, Window* window) {
    Matrix move, rotateX, rotateY, rotateZ, result;

    // 1. Translade COP para a origem
    move = Transformer::make3DMoveMatrix(window->COP * (-1));

    // 2. Determine VPN
    // double xAngle = Transformer::angleBetweenVectors(Point(1, 0, 0),
    // window->n); double yAngle = Transformer::angleBetweenVectors(Point(0, 1,
    // 0), window->n);

    // 3.Rotacione o mundo em torno de X e Y de forma a alinhar VPN com o eixo Z
    rotateX = Transformer::make3DRotateMatrixX(
        Transformer::degreesToRadians(window->xAngle));
    rotateY = Transformer::make3DRotateMatrixY(
        Transformer::degreesToRadians(window->yAngle));
    rotateZ = Transformer::make3DRotateMatrixZ(
        Transformer::degreesToRadians(window->zAngle));


    writePointToLog(window->COP, "COP");
    writePointToLog(window->origin, "Origige");

    Matrix mPerspective = Matrix(4, 4);
    mPerspective[0][0] = 1;
    mPerspective[1][1] = 1;
    mPerspective[2][2] = 1;
    mPerspective[3][2] = 1 / window->origin.z;
    mPerspective = mPerspective.transpose();

    result = (move * rotateX) * rotateY * rotateZ * mPerspective;

    auto it = dp->elements.begin();
    // 4. projetamos uma linha imaginária de cada ponto do objeto ao COP e
    // calculamos a intersecao com o plano de projecao
    for (; it != dp->elements.end(); ++it) {
      bool isCurve = it->second->type == GraphicType::CURVE_BEZIER ||
                     it->second->type == GraphicType::CURVE_SPLINE;
      Transformer::transform3DPoints(it->second, result, !isCurve);
      // for (unsigned i = 0; i < it->second->points.size(); ++i) {
      //   //toPerspective(it->second->points[i], window->origin.z - window->COP.z);
      // }
    }

    // Move the window itself
    // Transformer::transform3DPoint(&window->origin, result);
    // Transformer::transform3DPoint(&window->v, result);
    // Transformer::transform3DPoint(&window->u, result);
    // Transformer::transform3DPoint(&window->n, result);
  }

 private:
  void toPerspective(Point& p, double d) {
    p.x = p.x / (p.z / d);
    p.y = p.y / (p.z / d);
    p.z = d;
  }
};

#endif
