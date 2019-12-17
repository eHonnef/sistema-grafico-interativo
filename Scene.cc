#ifndef SCENE_H
#define SCENE_H

#include <cairomm/context.h>
#include <gtkmm/drawingarea.h>
#include <array>
#include <sstream>
#include "Bezier.cc"
#include "CairoRenderer.cc"
#include "Clipper.cc"
#include "CohenSutherland.cc"
#include "DisplayFile.cc"
#include "Globals.cc"
#include "GraphicElement.cc"
#include "GraphicType.cc"
#include "NichollLeeNicholl.cc"
#include "ParallelProjection.cc"
#include "PerspectiveProjection.cc"
#include "PointClip.cc"
#include "Spline.cc"
#include "SutherlandHodgman.cc"
#include "Transformer.cc"
#include "Viewport.cc"
#include "Window.cc"

class Scene : public Gtk::DrawingArea, Transformer {
 public:
  Window *window;
  DisplayFile *displayFile;
  bool clipEnabled = true;
  const double BORDER_SIZE = 50;

  Scene(Projection *projection, ClippingAlgorithm *clipLine) {
    this->projection = projection;
    this->clipLine = clipLine;

    this->window = new Window();
    this->viewport = new Viewport(this->window);
    this->displayFile = new DisplayFile();
    this->renderer =
        new CairoRenderer(this->viewport, this->window, this->displayFile);

    tempAddDefaultElements();
  };
  virtual ~Scene(){};
  //-------------------------------------------------------------------------------
  void setProjection(Projection *p) {
    this->projection = p;
    this->redraw();
  }
  //-------------------------------------------------------------------------------
  void setLineClipper(ClippingAlgorithm *ca) {
    this->clipLine = ca;
    this->redraw();
  }
  //-------------------------------------------------------------------------------
  bool addLine(const std::string &name, int color, int thickness,
               std::array<double, 2> initial,
               std::array<double, 2> finalPoint) {
    if (displayFile->elements.find(name) != displayFile->elements.end())
      return false;

    GraphicElement *ge =
        new GraphicElement(name, GraphicType::LINE, color, thickness);

    ge->addPoint(initial[0], initial[1]);
    ge->addPoint(finalPoint[0], finalPoint[1]);

    displayFile->elements.insert(std::make_pair(ge->name, ge));

    this->redraw();
    return true;
  }
  //-------------------------------------------------------------------------------
  bool addPoint(const std::string &name, int color, int thickness,
                std::array<double, 3> coord) {
    if (displayFile->elements.find(name) != displayFile->elements.end())
      return false;

    GraphicElement *ge =
        new GraphicElement(name, GraphicType::LINE, color, thickness);

    ge->addPoint(coord[0], coord[1]);

    displayFile->elements.insert(std::make_pair(ge->name, ge));

    this->redraw();
    return true;
  }
  //-------------------------------------------------------------------------------
  bool addPolygon(const std::string &name, int color, int thickness,
                  const std::vector<double> &points) {
    if (displayFile->elements.find(name) != displayFile->elements.end())
      return false;

    GraphicElement *ge =
        new GraphicElement(name, GraphicType::POLYGON, color, thickness);

    for (unsigned i = 0; i < points.size(); i += 3)
      ge->addPoint(points[i], points[i + 1], points[i + 2]);

    if (ge->points[0] == ge->points[ge->points.size() - 1])
      ge->points.pop_back();

    displayFile->elements.insert(std::make_pair(ge->name, ge));
    this->redraw();
    return true;
  }
  //-------------------------------------------------------------------------------
  bool addWireframe(const std::string &name, int color, int thickness,
                    const std::vector<double> &points) {
    if (displayFile->elements.find(name) != displayFile->elements.end())
      return false;

    GraphicElement *ge =
        new GraphicElement(name, GraphicType::WIREFRAME, color, thickness);

    for (unsigned i = 0; i < points.size(); i += 3)
      ge->addPoint(points[i], points[i + 1], points[i + 2]);

    displayFile->elements.insert(std::make_pair(ge->name, ge));
    this->redraw();
    return true;
  }
  //-------------------------------------------------------------------------------
  bool addBezier(const std::string &name, int color, int thickness,
                 const std::vector<double> &points) {
    if (displayFile->elements.find(name) != displayFile->elements.end() ||
        points.size() < 4)
      return false;

    GraphicElement *ge =
        new GraphicElement(name, GraphicType::CURVE_BEZIER, color, thickness);

    for (unsigned i = 0; i < 12; i += 3)
      ge->addPoint(points[i], points[i + 1], points[i + 2]);

    displayFile->elements.insert(std::make_pair(ge->name, ge));
    this->redraw();
    return true;
  }
  //-------------------------------------------------------------------------------
  bool addSpline(const std::string &name, int color, int thickness,
                 const std::vector<double> &points) {
    if (displayFile->elements.find(name) != displayFile->elements.end() ||
        points.size() < 4)
      return false;

    GraphicElement *ge =
        new GraphicElement(name, GraphicType::CURVE_SPLINE, color, thickness);

    for (unsigned i = 0; i < points.size(); i += 3)
      ge->addPoint(points[i], points[i + 1], points[i + 2]);

    displayFile->elements.insert(std::make_pair(ge->name, ge));
    this->redraw();
    return true;
  }
  //-------------------------------------------------------------------------------
  bool tempAddDefaultElements() {
    // creating xyz references lines
    // x = red
    GraphicElement *ge =
        new GraphicElement("ref_X", GraphicType::LINE, 0xFF0000, 5);
    ge->addPoint(0, 0, 0)->addPoint(700, 0, 0);
    displayFile->elements.insert(std::make_pair(ge->name, ge));
    // y = green
    ge = new GraphicElement("ref_Y", GraphicType::LINE, 0x00FF00, 5);
    ge->addPoint(0, 0, 0)->addPoint(0, 700, 0);
    displayFile->elements.insert(std::make_pair(ge->name, ge));
    // z = blue
    ge = new GraphicElement("ref_Z", GraphicType::LINE, 0x0000FF, 5);
    ge->addPoint(0, 0, 0)->addPoint(0, 0, 700);
    displayFile->elements.insert(std::make_pair(ge->name, ge));

    // Simple point
    GraphicElement *ge1 =
        new GraphicElement("point", GraphicType::POINT, 0, 10);
    ge1->addPoint(200, 300);
    displayFile->elements.insert(std::make_pair(ge1->name, ge1));

    // Simple line
    GraphicElement *ge2 =
        new GraphicElement("line", GraphicType::LINE, 0xFFFF00, 10);
    ge2->addPoint(0, 0)->addPoint(100, 100);
    displayFile->elements.insert(std::make_pair(ge2->name, ge2));

    // Simple polygon
    GraphicElement *ge3 =
        new GraphicElement("house", GraphicType::POLYGON, 0xFF00FF, 1);
    ge3->addPoint(200, 300);
    ge3->addPoint(200, 350);
    ge3->addPoint(250, 375);
    ge3->addPoint(300, 350);
    ge3->addPoint(300, 300);
    ge3->addPoint(200, 300);
    displayFile->elements.insert(std::make_pair(ge3->name, ge3));

    // Mountain
    GraphicElement *ge4 =
        new GraphicElement("mountain", GraphicType::WIREFRAME, 0x00FFFF, 1);
    ge4->addPoint(-500, -300);
    ge4->addPoint(-100, 50);
    ge4->addPoint(40, -250);
    ge4->addPoint(200, 100);
    ge4->addPoint(500, -150);
    displayFile->elements.insert(std::make_pair(ge4->name, ge4));

    // Bezier 4 points
    GraphicElement *ge5 =
        new GraphicElement("bezier5", GraphicType::CURVE_BEZIER, 50687, 3);
    ge5->addPoint(-500, 0);
    ge5->addPoint(0, 300);
    ge5->addPoint(-500, 400);
    ge5->addPoint(0, 400);
    displayFile->elements.insert(std::make_pair(ge5->name, ge5));

    // Bezier 4 points
    GraphicElement *ge6 =
        new GraphicElement("bezier6", GraphicType::CURVE_BEZIER, 65280, 1);
    ge6->addPoint(0, 0);
    ge6->addPoint(400, 350);
    ge6->addPoint(0, 600);
    ge6->addPoint(150, 0);
    displayFile->elements.insert(std::make_pair(ge6->name, ge6));
    Transformer::move(ge6, Point(-600, 200));

    // Spline 4 points
    GraphicElement *ge7 =
        new GraphicElement("spline7", GraphicType::CURVE_SPLINE, 50687, 2);
    ge7->addPoint(0, 0);
    ge7->addPoint(350, 400);
    ge7->addPoint(600, 0);
    ge7->addPoint(800, 0);
    displayFile->elements.insert(std::make_pair(ge7->name, ge7));
    Transformer::move(ge7, Point(-650, -600));

    GraphicElement *ge8 =
        new GraphicElement("spline7_reference", GraphicType::LINE, 1, 1);
    ge8->addPoint(0, 0);
    ge8->addPoint(350, 400);
    ge8->addPoint(600, 0);
    ge8->addPoint(800, 0);
    displayFile->elements.insert(std::make_pair(ge8->name, ge8));
    Transformer::move(ge8, Point(-650, -600));

    GraphicElement *cube =
        new GraphicElement("cube", GraphicType::POLYGON, 1, 1);
    cube->addPoint(0, 0);
    cube->addPoint(0, 300);
    cube->addPoint(300, 300);
    cube->addPoint(300, 0);
    cube->addPoint(0, 0);
    cube->addPoint(0, 0, -500);
    cube->addPoint(300, 0, -500);
    cube->addPoint(300, 300, -500);
    cube->addPoint(300, 300);
    cube->addPoint(0, 300);
    cube->addPoint(0, 300, -500);
    cube->addPoint(300, 300, -500);
    cube->addPoint(300, 0, -500);
    cube->addPoint(300, 0);
    cube->addPoint(300, 300);
    cube->addPoint(300, 300, -500);
    cube->addPoint(0, 300, -500);
    cube->addPoint(0, 0, -500);
    displayFile->elements.insert(std::make_pair(cube->name, cube));
    Transformer::move(cube, Point(400, -600));

    // Move the polygon
    // Transformer::move(ge3, Point(300, 100));
    // rotate(ge3, degreesToRadians(90), Point(200, 300));
    // scale(ge3, Point(2, 2), Point(200, 300));

    // rotateWindowLeft(90);

    return true;
  }
  //-------------------------------------------------------------------------------
  void redraw() {
    auto win = get_window();
    win->invalidate(true);
  }
  //-------------------------------------------------------------------------------
  bool removeGraphicElement(const std::string &name) {
    if (displayFile->elements.find(name) == displayFile->elements.end())
      return false;

    delete displayFile->elements.at(name);
    displayFile->elements.erase(name);

    redraw();
    return true;
  }

  bool rotateWindowACW(double angle) {
    std::ostringstream strs;
    strs << "Rodando janela para anti horario em graus: " << angle;
    writeToLog(strs.str());
    window->rotateACW(angle);
    redraw();
    return true;
  }

  bool rotateWindowCW(double angle) {
    std::ostringstream strs;
    strs << "Rodando janela para horario em graus: " << angle;
    writeToLog(strs.str());
    window->rotateCW(-angle);
    redraw();
    return true;
  }

  void zoomIn() {
    window->zoomIn();
    redraw();
  }

  void zoomOut() {
    window->zoomOut();
    redraw();
  }

  //-------------------------------------------------------------------------------
  bool rotateGraphicElement(const std::string &name, double angle) {
    // Angle is in degrees
    if (displayFile->elements.find(name) == displayFile->elements.end())
      return false;

    rotate(displayFile->elements.at(name), degreesToRadians(angle));
    redraw();
    return true;
  }

  bool rotateGraphicElement(const std::string &name, double angle, double x,
                            double y, double z = 1.0) {
    // Angle is in degrees
    if (displayFile->elements.find(name) == displayFile->elements.end())
      return false;

    Point point(x, y, z);
    rotate(displayFile->elements.at(name), degreesToRadians(angle), point);
    redraw();
    return true;
  }

  bool moveGraphicElement(const std::string &name, double x, double y,
                          double z = 1.0) {
    if (displayFile->elements.find(name) == displayFile->elements.end())
      return false;

    x -= displayFile->elements.at(name)->centerOfMass().x;
    y -= displayFile->elements.at(name)->centerOfMass().y;

    Point point(x, y, z);
    move(displayFile->elements.at(name), point);
    redraw();
    return true;
  }

  bool scaleGraphicElement(const std::string &name, double scaleVal) {
    if (displayFile->elements.find(name) == displayFile->elements.end())
      return false;

    Point point(scaleVal, scaleVal, 1);

    scale(displayFile->elements.at(name), point);
    redraw();
    return true;
  }

  double transformX(double x) {
    // return viewport->transformX(x);
    return x;
  }
  double transformY(double y) {
    // return viewport->transformY(y);
    return y;
  }

 protected:
  Viewport *viewport;
  CairoRenderer *renderer;
  ClippingAlgorithm *clipPoint = new PointClip();
  ClippingAlgorithm *clipPolygon = new SutherlandHodgman();
  ClippingAlgorithm *clipLine;
  Projection *projection;
  bool windowInitialized = false;

  // on_draw is called automaticly by GKT everytime a rerender is needed
  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    Gtk::Allocation allocation = get_allocation();

    // Setting the default window size as the size of viewport
    if (!windowInitialized) {
      window->setInitialSize(allocation.get_width(), allocation.get_height());
      viewport->setBorderSize(BORDER_SIZE);
      viewport->Xvpmin = BORDER_SIZE;
      viewport->Xvpmax = allocation.get_width() - BORDER_SIZE;
      viewport->Yvpmin = BORDER_SIZE;
      viewport->Yvpmax = allocation.get_height() - BORDER_SIZE;

      windowInitialized = true;
    }

    // Restore points of graphic elements
    displayFile->restorePoints();

    // Restore the points of the window (that were previously transformed by
    // parallel projection)
    window->restoreWindowPoints();

    // @todo generate bezier and spline
    /*
    auto it = displayFile->elements.begin();
    for (; it != displayFile->elements.end(); ++it) {
      if (it->second->type == GraphicType::CURVE_BEZIER) {
        Bezier::generatePoints(&it->second->points, 200);
      } else if (it->second->type == GraphicType::CURVE_SPLINE) {
        Spline::generatePoints(&it->second->points, 200);
      }
    }
    */

    // Make projection
    projection->project(displayFile, window);

    // Transform points of graphic elements to window points
    window->toWindowCoordinates(displayFile);

    auto it = displayFile->elements.begin();
    for (; it != displayFile->elements.end(); ++it) {
      // Enable or disable clip throught interface
      if (clipEnabled) {
        if (it->second->type == GraphicType::LINE ||
            it->second->type == GraphicType::WIREFRAME) {
          Clipper::clip(clipLine, &it->second->points);
        } else if (it->second->type == GraphicType::POLYGON) {
          Clipper::clip(clipPolygon, &it->second->points);
        } else if (it->second->type == GraphicType::POINT) {
          Clipper::clip(clipPoint, &it->second->points);
        } else if (it->second->type == GraphicType::CURVE_BEZIER) {
          // Prepare the points of Bezier with 200 steps
          Bezier::generatePoints(&it->second->points, 200);
          // Clip the curve if clipping enabled
          Clipper::clip(clipLine, &it->second->points);
        } else if (it->second->type == GraphicType::CURVE_SPLINE) {
          // Prepare the points of Spline with 200 steps
          Spline::generatePoints(&it->second->points, 200);
          // Clip the curve if clipping enabled
          Clipper::clip(clipLine, &it->second->points);
        }
        // You still have to generate the curves
      } else if (it->second->type == GraphicType::CURVE_BEZIER) {
        Bezier::generatePoints(&it->second->points, 200);
      } else if (it->second->type == GraphicType::CURVE_SPLINE) {
        Spline::generatePoints(&it->second->points, 200);
      }
    }
    // Render the scene
    renderer->render(cr);

    // Draw the window border
    drawWindowBorder(cr);

    return true;
  };

  void drawWindowBorder(const Cairo::RefPtr<Cairo::Context> &cr) {
    Gtk::Allocation allocation = get_allocation();

    // window box
    cr->set_source_rgb(1, 0, 0);
    cr->set_line_width(4);
    cr->move_to(viewport->transformX(-1), viewport->transformY(-1));
    cr->line_to(viewport->transformX(-1), viewport->transformY(1));
    cr->line_to(viewport->transformX(1), viewport->transformY(1));
    cr->line_to(viewport->transformX(1), viewport->transformY(-1));
    cr->line_to(viewport->transformX(-1), viewport->transformY(-1));
    cr->stroke();

    // vertical line
    cr->set_source_rgb(1, 1, 1);
    cr->set_line_width(1);
    cr->move_to(viewport->transformX(0), viewport->transformY(-1));
    cr->line_to(viewport->transformX(0), viewport->transformY(1));
    cr->stroke();

    // horizontal line
    cr->set_source_rgb(1, 1, 1);
    cr->set_line_width(1);
    cr->move_to(viewport->transformX(-1), viewport->transformY(0));
    cr->line_to(viewport->transformX(1), viewport->transformY(0));
    cr->stroke();
  }
};

#endif  // SCENE_H
