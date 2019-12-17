#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <math.h>
#include <vector>
#include "GraphicElement.cc"
#include "Matrix.cc"
#include "Point.cc"

#define PI 3.14159265358979323846

class Transformer {
 public:
  static constexpr double RESIDUAL = 1e-12;
  Transformer(){};
  virtual ~Transformer(){};

  static void move(GraphicElement* ge, Point point) {
    Matrix moveMatrix = makeMoveMatrix(point);

    transformOriginalPoints(ge, moveMatrix);
  }

  static void scale(GraphicElement* ge, Point point, Point origin) {
    Point originInverted = origin;
    originInverted.x = -originInverted.x;
    originInverted.y = -originInverted.y;
    Matrix scaleMatrix = makeMoveMatrix(originInverted) *
                         makeScaleMatrix(point) * makeMoveMatrix(origin);

    transformOriginalPoints(ge, scaleMatrix);
  }

  static void scale(GraphicElement* ge, Point point) {
    return scale(ge, point, ge->centerOfMass());
  }

  static void rotate(GraphicElement* ge, double angle, Point origin) {
    Point originInverted = origin;
    originInverted.x = -originInverted.x;
    originInverted.y = -originInverted.y;
    Matrix rotateMatrix = makeMoveMatrix(originInverted) *
                          makeRotateMatrix(angle) * makeMoveMatrix(origin);

    transformOriginalPoints(ge, rotateMatrix);
  }

  static void rotate(GraphicElement* ge, double angle) {
    return rotate(ge, angle, ge->centerOfMass());
  }

  static Matrix makeMoveMatrix(Point point) {
    Matrix matrix = Matrix(3, 3);
    matrix[0][0] = 1;
    matrix[1][1] = 1;
    matrix[2][0] = point.x;
    matrix[2][1] = point.y;
    matrix[2][2] = 1;
    return matrix;
  }

  static Matrix make3DMoveMatrix(const Point& point) {
    Matrix matrix = Matrix(4, 4);
    matrix[0][0] = 1;
    matrix[1][1] = 1;
    matrix[2][2] = 1;
    matrix[3][3] = 1;
    matrix[3][0] = point.x;
    matrix[3][1] = point.y;
    matrix[3][2] = point.z;
    return matrix;
  }

  // http://onlinemschool.com/math/library/vector/angl/
  static double angleBetweenVectors(const Point& a, const Point& b) {
    double dot = Matrix::dotProduct(a, b);
    double magA = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    double magB = sqrt(b.x * b.x + b.y * b.y + b.z * b.z);
    double mag = magA * magB;
    if (mag != (double)0.0) return acos(dot / mag);

    return (double)0.0;
  }

  // Cross-product: http://onlinemschool.com/math/library/vector/multiply1/
  static Point crossProduct(const Point& a, const Point& b) {
    return Point(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                 a.x * b.y - a.y * b.x);
  }

  // Normal: https://web.ma.utexas.edu/users/m408m/Display12-5-4.shtml
  static Point planeNormal(const Point& p0, const Point& p1, const Point& p2) {
    Point p10 = p1 - p0;
    Point p20 = p2 - p0;

    // Cross-product:
    // http://adaptivemap.ma.psu.edu/websites/vectormath/crossproduct/crossproduct.html
    return crossProduct(p10, p20);
  }

  static Matrix makeScaleMatrix(Point point) {
    Matrix matrix = Matrix(3, 3);
    matrix[0][0] = point.x;
    matrix[1][1] = point.y;
    matrix[2][2] = 1;
    return matrix;
  }

  static Matrix makeRotateMatrix(double angle) {
    Matrix matrix = Matrix(3, 3);
    matrix[0][0] = cos(angle);
    matrix[0][1] = -sin(angle);
    matrix[1][0] = sin(angle);
    matrix[1][1] = cos(angle);
    matrix[2][2] = 1;
    return matrix;
  }

  static Matrix make3DRotateMatrixX(double angle) {
    Matrix matrix = Matrix(4, 4);
    matrix[0][0] = 1;
    matrix[3][3] = 1;
    matrix[1][1] = cos(angle);
    matrix[1][2] = sin(angle);
    matrix[2][1] = -sin(angle);
    matrix[2][2] = cos(angle);
    return matrix;
  }

  static Matrix make3DRotateMatrixY(double angle) {
    Matrix matrix = Matrix(4, 4);
    matrix[1][1] = 1;
    matrix[3][3] = 1;
    matrix[0][0] = cos(angle);
    matrix[0][2] = -sin(angle);
    matrix[2][0] = sin(angle);
    matrix[2][2] = cos(angle);
    return matrix;
  }

  static Matrix make3DRotateMatrixZ(double angle) {
    Matrix matrix = Matrix(4, 4);
    matrix[2][2] = 1;
    matrix[3][3] = 1;
    matrix[0][0] = cos(angle);
    matrix[0][1] = sin(angle);
    matrix[1][0] = -sin(angle);
    matrix[1][1] = cos(angle);
    return matrix;
  }

  static double degreesToRadians(double degrees) {
    return (degrees * PI) / 180;
  }

  static double radiansToDegrees(double radians) {
    return (radians * 180) / PI;
  }

  static void transformPoints(GraphicElement* ge, Matrix matrix,
                              bool fromOriginal = true) {
    unsigned size = ge->originalPoints.size();

    if (size > 0) {
      for (unsigned i = 0; i < size; ++i) {
        Matrix gePointMatrix = Matrix(1, 3);
        gePointMatrix[0][0] =
            fromOriginal ? ge->originalPoints[i].x : ge->points[i].x;
        gePointMatrix[0][1] =
            fromOriginal ? ge->originalPoints[i].y : ge->points[i].y;
        gePointMatrix[0][2] = 1;

        Matrix resultMatrix = gePointMatrix * matrix;

        ge->points[i].x = resultMatrix[0][0];
        ge->points[i].y = resultMatrix[0][1];

        // Prevent residual values
        // if (ge->points[i].x < RESIDUAL) ge->points[i].x = 0;
        // if (ge->points[i].y < RESIDUAL) ge->points[i].y = 0;
      }
    }
  }

  static void transform3DPoints(GraphicElement* ge, const Matrix& matrix,
                                bool fromOriginal = true) {
    unsigned size = ge->originalPoints.size();

    if (size > 0) {
      for (unsigned i = 0; i < size; ++i) {
        Matrix gePointMatrix = Matrix(1, 4);
        gePointMatrix[0][0] =
            fromOriginal ? ge->originalPoints[i].x : ge->points[i].x;
        gePointMatrix[0][1] =
            fromOriginal ? ge->originalPoints[i].y : ge->points[i].y;
        gePointMatrix[0][2] =
            fromOriginal ? ge->originalPoints[i].z : ge->points[i].z;
        gePointMatrix[0][3] = 1;

        Matrix resultMatrix = gePointMatrix * matrix;

        ge->points[i].x = resultMatrix[0][0] * resultMatrix[0][3];
        ge->points[i].y = resultMatrix[0][1] * resultMatrix[0][3];
        ge->points[i].z = resultMatrix[0][2] * resultMatrix[0][3];

        // Prevent residual values
        // if (ge->points[i].x < RESIDUAL) ge->points[i].x = 0;
        // if (ge->points[i].y < RESIDUAL) ge->points[i].y = 0;
        // if (ge->points[i].z < RESIDUAL) ge->points[i].z = 0;
      }
    }
  }

  static void transform3DPoint(Point* point, const Matrix& matrix) {
    Matrix pointMatrix = Matrix(1, 4);
    pointMatrix[0][0] = point->x;
    pointMatrix[0][1] = point->y;
    pointMatrix[0][2] = point->z;
    pointMatrix[0][3] = 1;

    Matrix resultMatrix = pointMatrix * matrix;

    point->x = resultMatrix[0][0];
    point->y = resultMatrix[0][1];
    point->z = resultMatrix[0][2];

    // Prevent residual values
    // if (point->x < RESIDUAL) point->x = 0;
    // if (point->y < RESIDUAL) point->y = 0;
    // if (point->z < RESIDUAL) point->z = 0;
  }

  static void transformPoint(Point* point, const Matrix& matrix) {
    Matrix pointMatrix = Matrix(1, 3);
    pointMatrix[0][0] = point->x;
    pointMatrix[0][1] = point->y;
    pointMatrix[0][2] = 1;

    Matrix resultMatrix = pointMatrix * matrix;

    point->x = resultMatrix[0][0];
    point->y = resultMatrix[0][1];

    // Prevent residual values
    // if (point->x < RESIDUAL) point->x = 0;
    // if (point->y < RESIDUAL) point->y = 0;
  }

  static void transformOriginalPoints(GraphicElement* ge, Matrix matrix) {
    int size = ge->originalPoints.size();

    if (size > 0) {
      for (unsigned i = 0; i < size; ++i) {
        Matrix gePointMatrix = Matrix(1, 3);
        gePointMatrix[0][0] = ge->originalPoints[i].x;
        gePointMatrix[0][1] = ge->originalPoints[i].y;
        gePointMatrix[0][2] = 1;

        Matrix resultMatrix = gePointMatrix * matrix;

        ge->originalPoints[i].x = resultMatrix[0][0];
        ge->originalPoints[i].y = resultMatrix[0][1];

        // Prevent residual values
        // if (ge->originalPoints[i].x < RESIDUAL) ge->originalPoints[i].x = 0;
        // if (ge->originalPoints[i].y < RESIDUAL) ge->originalPoints[i].y = 0;
      }
    }
  }
};

#endif  // TRANSFORMER_H
