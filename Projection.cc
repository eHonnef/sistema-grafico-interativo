#ifndef PROJECTION_H
#define PROJECTION_H
#include "DisplayFile.cc"
#include "Window.cc"

class Projection {
 public:
  virtual void project(DisplayFile* dp, Window* window) = 0;
};

#endif
