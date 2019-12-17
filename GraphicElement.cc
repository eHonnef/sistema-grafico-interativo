#ifndef GRAPHICELEMENT_H
#define GRAPHICELEMENT_H

#include <string>
#include <vector>
#include "GraphicType.cc"
#include "Point.cc"

class GraphicElement {
 public:
  std::string name;
  GraphicType type;
  int color;
  double thickness;
  std::vector<Point> originalPoints;  // Points in the coordinate system
  std::vector<Point> points;          // Points in window system

  GraphicElement(std::string name, GraphicType type, int color,
                 double thickness) {
    this->name = name;
    this->type = type;
    this->color = color;
    this->thickness = thickness;
  };

  virtual ~GraphicElement(){};

  GraphicElement* addPoint(double x, double y, double z = 0) {
    originalPoints.push_back(Point(x, y, z));
    points.push_back(Point(x, y, z));
    return this;
  }

  void restorePoints() {
    points.assign(originalPoints.begin(), originalPoints.end());
  }

  Point centerOfMass() {
    int size = this->originalPoints.size();
    double x = 0;
    double y = 0;
    for (unsigned i = 0; i < size; ++i) {
      x += this->originalPoints.at(i).x;
      y += this->originalPoints.at(i).y;
    }

    x = size > 0 ? x / size : 0;
    y = size > 0 ? y / size : 0;

    return Point(x, y);
  }

  std::string getType() {
    if (type == GraphicType::LINE) return "LINE";
    if (type == GraphicType::POINT) return "POINT";
    if (type == GraphicType::WIREFRAME) return "WIREFRAME";
    if (type == GraphicType::CURVE_BEZIER) return "CURVE_BEZIER";
    if (type == GraphicType::CURVE_SPLINE) return "CURVE_SPLINE";
    if (type == GraphicType::POLYGON) return "POLYGON";
  }
};

#endif  // GRAPHICELEMENT_H
