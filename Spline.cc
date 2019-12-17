#ifndef SPLINE_H
#define SPLINE_H

#include <stdexcept>
#include <string>
#include <vector>
#include "Globals.cc"
#include "GraphicElement.cc"
#include "Matrix.cc"
#include "Point.cc"

class Spline {
 public:
  Spline(){};
  virtual ~Spline(){};

  static void generatePoints(std::vector<Point>* points, int steps = 200) {
    if (steps < 1)
      throw std::invalid_argument("Spline curve expects at least 1 step!");

    if (points->size() < 4)
      throw std::invalid_argument("Spline curve must have at least 4 points!");

    std::vector<Point> gbs;
    std::vector<Point> vertices;
    double delta, x, y, z, delta_x, delta_x2, delta_x3, delta_y, delta_y2,
        delta_y3, delta_z, delta_z2, delta_z3;
    unsigned n = steps;
    delta = 1.0 / n;
    double delta_square = delta * delta;
    double delta_cube = delta_square * delta;

    Matrix bspline_matrix = getMBSMatrix();
    Matrix delta_matrix = getEMatrix(delta);

    for (unsigned int i = 0; i <= points->size() - 4; ++i) {
      for (unsigned int j = 0; j < 4; ++j) {
        gbs.push_back(Point(points->at(i + j).x, points->at(i + j).y,
                            points->at(i + j).z));
      }

      std::vector<Point> coeficients =
          calculate_multiplication(bspline_matrix, gbs);
      std::vector<Point> differences_matrix =
          calculate_multiplication(delta_matrix, coeficients);
      std::vector<Point> aux = forward_differences(
          n, differences_matrix[0].x, differences_matrix[1].x,
          differences_matrix[2].x, differences_matrix[3].x,
          differences_matrix[0].y, differences_matrix[1].y,
          differences_matrix[2].y, differences_matrix[3].y,
          differences_matrix[0].z, differences_matrix[1].z,
          differences_matrix[2].z, differences_matrix[3].z);
      vertices.insert(vertices.end(), aux.begin(), aux.end());
      gbs.clear();
    }

    points->assign(vertices.begin(), vertices.end());
  }

  static std::vector<Point> calculate_multiplication(Matrix matrix,
                                                     std::vector<Point> gbs) {
    std::vector<Point> coeficients;
    for (unsigned int i = 0; i < gbs.size(); ++i) {
      double x = 0.0, y = 0.0, z = 0.0;
      for (unsigned int j = 0; j < gbs.size(); ++j) {
        x += matrix[i][j] * gbs[j].x;
        y += matrix[i][j] * gbs[j].y;
        z += matrix[i][j] * gbs[j].z;
      }
      coeficients.push_back(Point(x, y, z));
    }
    return coeficients;
  }

  static std::vector<Point> forward_differences(
      unsigned n, double x, double delta_x, double delta_x2, double delta_x3,
      double y, double delta_y, double delta_y2, double delta_y3, double z,
      double delta_z, double delta_z2, double delta_z3) {
    std::vector<Point> coordinates;
    double old_x = x;
    double old_y = y;
    double old_z = z;
    double delta_x2_ = delta_x2;

    unsigned i = 1;
    while (i < n) {
      ++i;
      x += delta_x;
      delta_x += delta_x2_;
      delta_x2_ += delta_x3;
      y += delta_y;
      delta_y += delta_y2;
      delta_y2 += delta_y3;
      z += delta_z;
      delta_z += delta_z2;
      delta_z2 += delta_z3;
      coordinates.push_back(Point(old_x, old_y, old_z));
      coordinates.push_back(Point(x, y, z));
      old_x = x;
      old_y = y;
      old_z = z;
    }
    return coordinates;
  }

  static Matrix getEMatrix(double delta) {
    double delta_square = delta * delta;
    double delta_cube = delta_square * delta;

    Matrix E = Matrix(4, 4);

    E[0][3] = (double)1.0;
    E[1][0] = delta_cube;
    E[1][1] = delta_square;
    E[1][2] = delta;
    E[2][0] = (double)6 * delta_cube;
    E[2][1] = (double)2 * delta_square;
    E[3][0] = (double)6 * delta_cube;

    return E;
  }

  static Matrix getMBSMatrix() {
    Matrix mbs = Matrix(4, 4);
    mbs[0][0] = (double)-1 / 6;
    mbs[0][1] = (double)3 / 6;
    mbs[0][2] = (double)-3 / 6;
    mbs[0][3] = (double)1 / 6;

    mbs[1][0] = (double)3 / 6;
    mbs[1][1] = (double)-6 / 6;
    mbs[1][2] = (double)3 / 6;
    mbs[1][3] = (double)0 / 6;

    mbs[2][0] = (double)-3 / 6;
    mbs[2][1] = (double)0 / 6;
    mbs[2][2] = (double)3 / 6;
    mbs[2][3] = (double)0 / 6;

    mbs[3][0] = (double)1 / 6;
    mbs[3][1] = (double)4 / 6;
    mbs[3][2] = (double)1 / 6;
    mbs[3][3] = (double)0 / 6;

    return mbs;
  }
};

#endif  // SPLINE_H
