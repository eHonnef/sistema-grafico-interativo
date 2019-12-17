#ifndef DISPLAY_FILE_H
#define DISPLAY_FILE_H

#include <map>
#include <string>
#include "GraphicElement.cc"
#include "WavefrontFile.cc"

class DisplayFile {
 public:
  std::map<std::string, GraphicElement*> elements;

  DisplayFile(){};
  virtual ~DisplayFile(){};

  // Ensures that exists the points based on the original points
  void restorePoints() {
    std::map<std::string, GraphicElement*>::iterator it;
    for (it = elements.begin(); it != elements.end(); ++it)
      it->second->restorePoints();
  }

  bool loadFromFile(const std::string& filename) {
    WavefrontFile wff = WavefrontFile(filename);
    if (wff.LoadedMeshes.size() == 0) return false;

    GraphicElement* ge;
    for (unsigned i = 0; i < wff.LoadedMeshes.size(); ++i) {
      ge = new GraphicElement(wff.LoadedMeshes[i].MeshName,
                              defineGraphicType(wff.LoadedMeshes[i].Vertices),
                              0x0, 3);
      insertPoints(wff.LoadedMeshes[i].Vertices, &ge->originalPoints);
      elements.insert(std::make_pair(ge->name, ge));
    }

    return true;
  }

  bool saveToFile(const std::string& filename) { return true; }

 private:
  GraphicType defineGraphicType(const std::vector<Vertex>& vertex) {
    if (vertex.size() == 1) return GraphicType::POINT;

    if (vertex.size() == 2) return GraphicType::LINE;

    if (vertex.size() > 2) return GraphicType::POLYGON;
  }

  void insertPoints(const std::vector<Vertex>& vertex,
                    std::vector<Point>* points) {
    for (unsigned i = 0; i < vertex.size(); ++i)
      points->push_back(Point(vertex[i].Position.X, vertex[i].Position.Y,
                             vertex[i].Position.Z));
  }
};

#endif  // DISPLAY_FILE_H
