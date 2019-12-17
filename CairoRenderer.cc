#ifndef CAIRO_RENDERER_H
#define CAIRO_RENDERER_H

#include <cairomm/context.h>
#include "DisplayFile.cc"
#include "GraphicType.cc"
#include "Point.cc"
#include "Viewport.cc"
#include "Window.cc"

class CairoRenderer {
 public:
  CairoRenderer(Viewport *viewport, Window *window, DisplayFile *displayFile) {
    this->window = window;
    this->viewport = viewport;
    this->displayFile = displayFile;
  };
  virtual ~CairoRenderer(){};

  void render(const Cairo::RefPtr<Cairo::Context> &cr) {
    std::map<std::string, GraphicElement *>::iterator it =
        displayFile->elements.begin();
    for (; it != displayFile->elements.end(); ++it) {
      this->renderGraphicElement(cr, it->second);
    }
  }

 protected:
  Window *window;
  Viewport *viewport;
  DisplayFile *displayFile;

  void renderGraphicElement(const Cairo::RefPtr<Cairo::Context> &cr,
                            GraphicElement *ge) {
    double thickness = ge->thickness;
    // adjust thickness by zoom factor
    if (window->zoomLevel < 0) {
      thickness -= -window->zoomLevel;
    } else if (window->zoomLevel > 0) {
      thickness += window->zoomLevel;
    }

    if (thickness < 2) thickness = 2;
    // if (thickness > ge->thickness * 2) thickness = ge->thickness * 2;

    cr->set_line_width((int)thickness);
    // FFAABB
    int r = (ge->color & 0xFF0000) >> 16;
    int g = (ge->color & 0xFF00) >> 8;
    int b = ge->color & 0xFF;
    // TODO: set color from graphicelement
    cr->set_source_rgb(r / 255, g / 255, b / 255);

    // Round it! (in the future each GraphicElement might have its own
    // configuration of line cap/join)
    cr->set_line_join(Cairo::LINE_JOIN_ROUND);
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);

    for (unsigned i = 0; i < ge->points.size(); ++i) {
      if (i < ge->points.size() - 1) {
        // Connect two points
        cr->move_to(viewport->transformX(ge->points.at(i).x),
                    viewport->transformY(ge->points.at(i).y));

        cr->line_to(viewport->transformX(ge->points.at(i + 1).x),
                    viewport->transformY(ge->points.at(i + 1).y));
        cr->stroke();
      } else if (i == 0) {
        // If it's only a point
        cr->move_to(viewport->transformX(ge->points.at(i).x),
                    viewport->transformY(ge->points.at(i).y));
        cr->close_path();
        cr->stroke();
      }
    }

    // Rendering the final line of the polygon
    if (ge->type == GraphicType::POLYGON && ge->points.size() > 1) {
      cr->move_to(viewport->transformX(ge->points.at(ge->points.size() - 1).x),
                  viewport->transformY(ge->points.at(ge->points.size() - 1).y));
      cr->line_to(viewport->transformX(ge->points.at(0).x),
                  viewport->transformY(ge->points.at(0).y));
      cr->stroke();
    }
  }
};

#endif  // CAIRO_RENDERER_H
