#ifndef NICHOLLLEENICHOLL_H
#define NICHOLLLEENICHOLL_H
#include <vector>
#include "ClippingAlgorithm.cc"
#include "Globals.cc"
#include "Point.cc"

class NichollLeeNicholl : public ClippingAlgorithm {
 public:
  void clip(std::vector<Point> *points) {
    std::vector<Point> new_points;
    Point min = Point(xMin, yMin);
    Point max = Point(xMax, yMax);
    Point p0, p1;
    for (unsigned i = 0, j = 1; j < points->size(); ++i, ++j) {
      p0 = points->at(i);
      p1 = points->at(j);
      if (NLC(p0, p1, min, max)) {
        if (!isInside(new_points, p0)) new_points.push_back(p0);

        if (!isInside(new_points, p1)) new_points.push_back(p1);
      }
    }
    // points->clear();
    points->assign(new_points.begin(), new_points.end());
  }

 private:
  bool isInside(const std::vector<Point> &points, const Point &p) {
    return std::find(points.begin(), points.end(), p) != points.end();
  }

  bool NLC(Point &A, Point &B, const Point &Pmin, const Point &Pmax) {
    /* 1 */
    if (A.x > B.x) std::swap(A, B);

    /* 2 */
    int C1 = 0;
    if (A.x < Pmin.x) C1 += 1;
    if (A.x > Pmax.x) C1 += 2;
    if (A.y < Pmin.y) C1 += 4;
    if (A.y > Pmax.y) C1 += 8;

    int C2 = 0;
    if (B.x < Pmin.x) C2 += 1;
    if (B.x > Pmax.x) C2 += 2;
    if (B.y < Pmin.y) C2 += 4;
    if (B.y > Pmax.y) C2 += 8;

    /* 3 */
    if (C1 & C2) return false;

    switch (C1) {
        /* 4 */
      case 0: {
        if (C2 == 0) return true;

        double dx = B.x - A.x;
        double dy = B.y - A.y;

        if (dy >= 0) {
          double dxr = Pmax.x - A.x;
          double dyt = Pmax.y - A.y;

          if ((dy * dxr) < (dx * dyt)) {
            Clip2Right(A, B, dx, dy, dxr);
          } else {
            Clip2Top(A, B, dx, dy, dyt);
          }
          return true;
        } else {
          double dxr = Pmax.x - A.x;
          double dyb = Pmin.y - A.y;

          if ((dy * dxr) < (dx * dyb)) {
            Clip2Bottom(A, B, dx, dy, dyb);
          } else {
            Clip2Right(A, B, dx, dy, dxr);
          }
          return true;
        }
        break;
      }

        /* 5 */
      case 1: {
        double dx = B.x - A.x;
        double dy = B.y - A.y;

        double dxl = Pmin.x - A.x;
        double dyt = Pmax.y - A.y;

        if (C2 == 0) {
          Clip1Left(A, dx, dy, dxl);
          return true;
        }

        double dyb;

        if (dy >= 0) {
          if ((dy * dxl) > (dx * dyt)) return false;

          double dxr = Pmax.x - A.x;
          double dyt = Pmax.y - A.y;

          if ((dy * dxr) < (dx * dyt)) {
            Clip2Right(A, B, dx, dy, dxr);
          } else {
            Clip2Top(A, B, dx, dy, dyt);
          }

          dx = B.x - A.x;
          dy = B.y - A.y;
          dxl = Pmin.x - A.x;

          Clip1Left(A, dx, dy, dxl);
          return true;
        } else {
          double dxl = Pmin.x - A.x;
          dyb = Pmin.y - A.y;

          if ((dy * dxl) < (dx * dyb)) return false;

          double dxr = Pmax.x - A.x;
          dyb = Pmin.y - A.y;

          if ((dy * dxr) < (dx * dyb)) {
            Clip2Bottom(A, B, dx, dy, dyb);
          } else {
            Clip2Right(A, B, dx, dy, dxr);
          }

          dx = B.x - A.x;
          dy = B.y - A.y;
          dxl = Pmin.x - A.x;

          Clip1Left(A, dx, dy, dxl);
          return true;
        }
        break;
      }

        /* 6 */
      case 4: {
        double dx = B.x - A.x;
        double dy = B.y - A.y;
        double dxr = Pmax.x - A.x;
        double dyb = Pmin.y - A.y;

        if (C2 == 0) {
          Clip1Bottom(A, dx, dy, dyb);
          return true;
        }

        if (dy > 0) {
          if ((dy * dxr) < (dx * dyb)) return false;

          double dyt = Pmax.y - A.y;

          if ((dy * dxr) < (dx * dyt)) {
            Clip2Right(A, B, dx, dy, dxr);
          } else {
            Clip2Top(A, B, dx, dy, dyt);
          }

          dx = B.x - A.x;
          dy = B.y - A.y;
          dyb = Pmin.y - A.y;

          Clip1Bottom(A, dx, dy, dyb);
          return true;
        } else {
          return false;
        }
      }

        /* 7 */
      case 5: {
        double dx = B.x - A.x;
        double dy = B.y - A.y;

        if (dy > 0) {
          double dxl = Pmin.x - A.x;
          double dyt = Pmax.y - A.y;

          if ((dy * dxl) > (dx * dyt)) return false;

          double dxr = Pmax.x - A.x;
          double dyb = Pmin.y - A.y;

          if ((dy * dxr) < (dx * dyb)) return false;

          if ((dyb * dxr) < (dxl * dyt)) {
            if ((dy * dxl) < (dx * dyb)) {
              Clip1Bottom(A, dx, dy, dyb);

              if (B.x > Pmax.x) {
                dx = B.x - A.x;
                dy = B.y - A.y;
                dxr = Pmax.x - A.x;

                Clip2Right(A, B, dx, dy, dxr);
              }
              return true;
            }

            Clip1Left(A, dx, dy, dxl);

            if (C2 == 0) return true;

            if (((dy * dxr) < (dx * dyt)) && (C2 != 0)) {
              dx = B.x - A.x;
              dy = B.y - A.y;
              dxr = Pmax.x - A.x;

              Clip2Right(A, B, dx, dy, dxr);
              return true;
            }

            dx = B.x - A.x;
            dy = B.y - A.y;
            dyt = Pmax.y - A.y;

            Clip2Top(A, B, dx, dy, dyt);
            return true;
          } else {
            if ((dy * dxr) < (dx * dyt)) {
              Clip1Bottom(A, dx, dy, dyb);

              if (B.x > Pmax.x) {
                dx = B.x - A.x;
                dy = B.y - A.y;
                dxr = Pmax.x - A.y;
                Clip2Right(A, B, dx, dy, dxr);
              }

              return true;
            }

            if ((dy * dxl) < (dx * dyb)) {
              Clip1Bottom(A, dx, dy, dyb);

              if (C2 != 0) {
                dx = B.x - A.x;
                dy = B.y - A.y;
                dyt = Pmax.y - A.y;

                Clip2Top(A, B, dx, dy, dyt);
              }
              return true;
            }

            Clip1Left(A, dx, dy, dxl);

            if (C2 == 0) return true;

            dx = B.x - A.x;
            dy = B.y - A.y;
            dyt = Pmax.y - A.y;

            Clip2Top(A, B, dx, dy, dyt);
            return true;
          }
        } else
          return false;
        break;
      }

        /* 8 */
      case 8: {
        double dx = B.x - A.x;
        double dy = B.y - A.y;

        double dxr = Pmax.x - A.x;
        double dyt = Pmax.y - A.y;

        if (C2 == 0) {
          Clip1Top(A, dx, dy, dyt);
          return true;
        }

        if (dy < 0) {
          if ((dy * dxr) > (dx * dyt)) return false;

          double dyb = Pmin.y - A.y;

          if ((dy * dxr) > (dx * dyb)) {
            Clip2Right(A, B, dx, dy, dxr);
          } else {
            Clip2Bottom(A, B, dx, dy, dyb);
          }

          dx = B.x - A.x;
          dy = B.y - A.y;
          dyt = Pmax.y - A.y;

          Clip1Top(A, dx, dy, dyt);
          return true;
        } else
          return false;
        break;
      }

        /* 9 */
      case 9: {
        double dx = B.x - A.x;
        double dy = B.y - A.y;

        if (dy < 0) {
          double dxr = Pmax.x - A.x;
          double dyt = Pmax.y - A.y;

          if ((dy * dxr) > (dx * dyt)) return false;

          double dxl = Pmin.x - A.x;
          double dyb = Pmin.y - A.y;

          if ((dy * dxl) < (dx * dyb)) return false;

          if ((dyt * dxr) > (dxl * dyb)) {
            if ((dy * dxl) > (dx * dyt)) {
              Clip1Top(A, dx, dy, dyt);

              if (B.x > Pmax.x) {
                dx = B.x - A.x;
                dy = B.y - A.y;
                dxr = Pmax.x - A.x;

                Clip2Right(A, B, dx, dy, dxr);
              }
              return true;
            }
            Clip1Left(A, dx, dy, dxl);

            if (C2 == 0) return true;

            if ((dy * dxr) > (dx * dyb)) {
              dx = B.x - A.x;
              dy = B.y - A.y;
              dxr = Pmax.x - A.x;

              Clip2Right(A, B, dx, dy, dxr);
              return true;
            }

            dx = B.x - A.x;
            dy = B.y - A.y;
            dyb = Pmin.y - A.y;

            Clip2Bottom(A, B, dx, dy, dyb);
            return true;
          } else {
            if ((dy * dxr) > (dx * dyb)) {
              Clip1Top(A, dx, dy, dyt);

              if (B.x > Pmax.x) {
                dx = B.x - A.x;
                dy = B.y - A.y;
                dxr = Pmax.x - A.x;

                Clip2Right(A, B, dx, dy, dxr);
              }
              return true;
            }

            if ((dy * dxl) > (dx * dyt)) {
              Clip1Top(A, dx, dy, dyt);

              if (C2 != 0) {
                dx = B.x - A.x;
                dy = B.y - A.y;
                dyb = Pmin.y - A.y;

                Clip2Bottom(A, B, dx, dy, dyb);
              }
              return true;
            }
            Clip1Left(A, dx, dy, dxl);

            if (C2 != 0) {
              dx = B.x - A.x;
              dy = B.y - A.y;
              dyb = Pmin.y - A.y;

              Clip2Bottom(A, B, dx, dy, dyb);
            }
            return true;
          }
        } else
          return false;
      }
    }
  }

  void Clip1Left(Point &A, double dx, double dy, double dxl) {
    A.x = A.x + dxl;
    A.y = A.y + dxl * dy / dx;
  }

  void Clip1Top(Point &A, double dx, double dy, double dyt) {
    A.x = A.x + dyt * (dx / dy);
    A.y = A.y + dyt;
  }

  void Clip1Bottom(Point &A, double dx, double dy, double dyb) {
    A.x = A.x + dyb * (dx / dy);
    A.y = A.y + dyb;
  }

  void Clip2Right(Point A, Point &B, double dx, double dy, double dxr) {
    B.x = A.x + dxr;
    B.y = A.y + dxr * (dy / dx);
  }

  void Clip2Top(Point A, Point &B, double dx, double dy, double dyt) {
    B.x = A.x + dyt * (dx / dy);
    B.y = A.y + dyt;
  }

  void Clip2Bottom(Point A, Point &B, double dx, double dy, double dyb) {
    B.x = A.x + dyb * (dx / dy);
    B.y = A.y + dyb;
  }
};

#endif
