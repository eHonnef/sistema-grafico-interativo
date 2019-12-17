#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <limits>
#include "Window.cc"

class Viewport {
 public:
  Viewport(Window* window) { this->window = window; };
  virtual ~Viewport(){};

  // Viewport dimensions
  int Xvpmin = 0;
  int Xvpmax = 800;
  int Yvpmin = 0;
  int Yvpmax = 600;
  int nWinMax = 1;
  int nWinMin = -1;

  double transformX(double x) {
    return ((x - nWinMin) / (nWinMax - nWinMin) * (Xvpmax - Xvpmin)) +
           borderSize;
  }

  double transformY(double y) {
    return ((1 - (y - nWinMin) / (nWinMax - nWinMin)) * (Yvpmax - Yvpmin)) +
           borderSize;
  }

  void setBorderSize(double size) { borderSize = size; }

 protected:
  Window* window;
  double borderSize = 0;
};

#endif  // VIEWPORT_H
