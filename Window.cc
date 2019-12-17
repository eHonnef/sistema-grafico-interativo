#ifndef WINDOW_H
#define WINDOW_H

#include <sstream>
#include "DisplayFile.cc"
#include "Globals.cc"
#include "GraphicElement.cc"
#include "Point.cc"
#include "Transformer.cc"

// Normalized window is [-1,1] with center on 0, 0
class Window {
 public:
  int zoomLevel = 0;
  // Window dimensions
  Point origin;  // window center
  Point v;       // y
  Point u;       // x
  Point n;       // normal

  // Original window dimensions
  Point originalOrigin;  // window center
  Point originalV;       // y
  Point originalU;       // x
  Point originalN;       // normal

  // Center of projection (Caso especial, centro de projecao na origem)
  Point COP = Point(0, 0, 10);

  double width;
  double height;
  double angle = 0;
  double xAngle = 45;
  double yAngle = 45;
  double zAngle = 0;

  Window() {
    origin = Point();
    v = Point();
    u = Point();
    n = Point();

    originalOrigin = Point();
    originalV = Point();
    originalU = Point();
    originalN = Point();
  }

  virtual ~Window(){};

  void rotateCW(int ammount = -5) {
    // // Rotate the window by ammount in degrees
    // double angle = Transformer::degreesToRadians(ammount);
    // this->angle += angle;
  }

  void rotateACW(int ammount = 5) { rotateCW(ammount); }

  void zoomIn(double ammount = 1) { zoomLevel += ammount; }

  void zoomOut(double ammount = 1) { zoomLevel -= ammount; }

  void normalUp(int ammount = 10) {}

  void normalDown(int ammount = 10) {}

  void normalLeft(int ammount = 10) {}

  void normalRight(int ammount = 10) {}

  void left(int ammount = 5) { right(-ammount); }

  void right(int ammount = 5) {
    // double signal = ammount > 0 ? 1.0 : -1.0;
    // double ammountP = (1.0 + (abs(ammount) / 100)) * signal;
    // Point destination = Point(originalU.x * ammountP, originalU.y * ammountP,
    //                           originalU.z * ammountP);

    // // Move the window to this new destination
    // Matrix move = Transformer::make3DMoveMatrix(destination);
    // Transformer::transform3DPoint(&originalOrigin, move);
    // Transformer::transform3DPoint(&originalV, move);
    // Transformer::transform3DPoint(&originalU, move);
    // Transformer::transform3DPoint(&originalN, move);
  }

  void backward(double ammount = -5) {
    Point destination = origin.z + ammount;

    writePointToLog(destination, "destination");
    // Move the window to this new destination
    Matrix move = Transformer::make3DMoveMatrix(destination);
    Transformer::transform3DPoint(&origin, move);
    Transformer::transform3DPoint(&v, move);
    Transformer::transform3DPoint(&u, move);
    // Transformer::transform3DPoint(&originalN, move);
    n.copy(Transformer::planeNormal(origin, u, v));

    writePointToLog(origin, "origin");
    writePointToLog(v, "u");
    writePointToLog(u, "v");
    writePointToLog(n, "n");
    writeWindowPoints();
  }

  void foward(double ammount = 5) { backward(ammount); }

  void toWindowCoordinates(DisplayFile* dp) {
    Matrix move, rotate, scale, zoom, result;

    // 1. Translade Wc para a origem;
    // ● Translade o mundo de [-Wcx, -Wcy].
    move = Transformer::makeMoveMatrix(Point(-origin.x, -origin.y));

    // 2. Determine vup e o ângulo de vup com Y
    double angleToY = PI - (PI / 2 + angle);

    // 3. Rotacione o mundo de forma a alinhar vup com o eixo Y;
    // ● Rotacione o mundo por -θ(Y, vup).
    rotate = Transformer::makeRotateMatrix(-angle);

    // 3.5 Zoom
    // double zoomFactor = zoomLevel == 0
    //                         ? 1.0
    //                         : (zoomLevel > 0 ? 1.0 + (zoomLevel / 2.0)
    //                                          : 1.0 - (zoomLevel / -20.0));
    // if (zoomFactor <= 0.0) zoomFactor = 0.05;
    // zoom = Transformer::makeScaleMatrix(Point(zoomFactor, zoomFactor));
    zoom = Transformer::makeScaleMatrix(Point(1, 1));

    // 4. Normalize o conteúdo da window, realizando um escalonamento do mundo;
    scale = Transformer::makeScaleMatrix(Point(1 / width, 1 / height));

    // result = ((move * rotate) * zoom) * scale;
    result = scale;

    // 5. Armazene as coordenadas SCN de cada objeto.
    std::map<std::string, GraphicElement*>::iterator it;
    for (it = dp->elements.begin(); it != dp->elements.end(); ++it)
      Transformer::transformPoints(it->second, result, false);
  }

  // Set the initial size of the window in world coordinates
  void setInitialSize(double w, double h) {
    // Window starts centralized at the world
    // 0. Crie ou mova a window onde desejar;
    v.x = 0;
    v.y = h / 2;
    v.z = 0;

    u.x = w / 2;
    u.y = 0;
    u.z = v.z;

    origin.x = 0;
    origin.y = 0;
    origin.z = v.z;

    // Calculate the normal
    n.copy(Transformer::planeNormal(origin, u, v));

    // n.x = 0;
    // n.y = 0;
    // n.z = 9;

    // Turn 30 degrees Matrix rotateX =
    // Matrix rotateX =
    //     Transformer::make3DRotateMatrixX(Transformer::degreesToRadians(45));
    // Matrix rotateY =
    //     Transformer::make3DRotateMatrixY(Transformer::degreesToRadians(45));
    // Transformer::transform3DPoint(&origin, rotateX * rotateY);
    // Transformer::transform3DPoint(&v, rotateX * rotateY);
    // Transformer::transform3DPoint(&u, rotateX * rotateY);
    // Transformer::transform3DPoint(&n, rotateX * rotateY);

    // Save to be restored
    originalOrigin.copy(origin);
    originalV.copy(v);
    originalU.copy(u);
    originalN.copy(n);

    width = w;
    height = h;
  }

  void restoreWindowPoints() {
    origin.copy(originalOrigin);
    v.copy(originalV);
    u.copy(originalU);
    n.copy(originalN);
  }

  void writeWindowPoints() {
    originalOrigin.copy(origin);
    originalV.copy(v);
    originalU.copy(u);
    originalN.copy(n);
  }
};

#endif  // WINDOW_H
