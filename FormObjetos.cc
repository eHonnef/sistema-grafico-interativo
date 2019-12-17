#ifndef FORMOBJETOS_H
#define FORMOBJETOS_H

#include "Form.cc"
#include "GraphicType.cc"
#include "Scene.cc"

class FormObjetos : public Form {
 public:
  // Not cool but oh well
  Gtk::TreeView* tree;
  Glib::RefPtr<Gtk::ListStore> objList;

  FormObjetos(Gtk::Viewport* canvas, Scene* scene)
      : Form("AdicionaObjeto.glade") {
    this->canvas = canvas;
    this->scene = scene;

    builder->get_widget("TabGroup", tabGroup);

    initializeButton("Button_OK", &FormObjetos::buttonOKOnClick);
    initializeButton("Button_Cancel", &FormObjetos::buttonCancel);

    initializeButton("Button_Adiciona_Wireframe",
                     &FormObjetos::buttonAddOnClick);

    initializeButton("Button_Remove_Wireframe",
                     &FormObjetos::buttonRemoveOnClick);

    builder->get_widget("Entry_Nome_Objeto", name);
    builder->get_widget("Entry_Thickness", thickness);
    builder->get_widget("Entry_Color", color);

    builder->get_widget("Radio_Poligono", radioPoligono);
    builder->get_widget("Radio_Wireframe", radioWireframe);
    builder->get_widget("Radio_Bezier", radioBezier);
    builder->get_widget("Radio_Spline", radioSpline);
  }

  virtual ~FormObjetos() {}
  /*
  void addObjectToList(const std::string& name, std::string type) {
    Gtk::TreeModel::Row row = *objList->append();
    row.set_value(0, name);
    row.set_value(1, type);
  }
  */

  void manageObjectList() {
    Gtk::TreeModel::Children rows = objList->children();
    Gtk::TreeModel::Row row;
    std::string value;

    for (auto it = rows.begin(); it != rows.end(); ++it) {
      row = *it;
      row.get_value(0, value);
      if (scene->displayFile->elements.find(value) ==
          scene->displayFile->elements.end()) {
        objList->erase(it);
        it = rows.begin();
      }
    }

    auto it = scene->displayFile->elements.begin();
    auto rit = rows.begin();

    for (; it != scene->displayFile->elements.end(); ++it) {
      rit = rows.begin();
      for (; rit != rows.end(); ++rit) {
        row = *rit;
        row.get_value(0, value);
        if (it->second->name == value) break;
      }

      if (rit == rows.end()) {
        row = *objList->append();
        row.set_value(0, it->second->name);
        row.set_value(1, it->second->getType());
      }
    }
  }

 private:
  Gtk::Viewport* canvas;
  Scene* scene;
  Gtk::Notebook* tabGroup;

  Gtk::Entry* name;
  Gtk::Entry* thickness;
  Gtk::Entry* color;

  Gtk::RadioButton* radioPoligono;
  Gtk::RadioButton* radioWireframe;
  Gtk::RadioButton* radioBezier;
  Gtk::RadioButton* radioSpline;

  void tabReta(std::vector<std::vector<double> >* vec) {
    std::vector<double> returnVec;
    Gtk::Entry* x0;
    Gtk::Entry* x1;
    Gtk::Entry* y0;
    Gtk::Entry* y1;

    builder->get_widget("Entry_Reta_x0", x0);
    builder->get_widget("Entry_Reta_x1", x1);
    builder->get_widget("Entry_Reta_y0", y0);
    builder->get_widget("Entry_Reta_y1", y1);

    returnVec.push_back(std::atof(x0->get_text().c_str()));
    returnVec.push_back(std::atof(y0->get_text().c_str()));
    vec->push_back(returnVec);
    returnVec.clear();

    returnVec.push_back(std::atof(x1->get_text().c_str()));
    returnVec.push_back(std::atof(y1->get_text().c_str()));
    vec->push_back(returnVec);
  }

  void tabPonto(double* x, double* y) {
    Gtk::Entry* _x;
    Gtk::Entry* _y;

    builder->get_widget("Entry_Ponto_x", _x);
    builder->get_widget("Entry_Ponto_y", _y);

    *x = std::atof(_x->get_text().c_str());
    *y = std::atof(_y->get_text().c_str());
  }

  void tabWireframe(std::vector<double>* vec) {
    Glib::RefPtr<Gtk::ListStore> pointsList =
        Glib::RefPtr<Gtk::ListStore>::cast_dynamic(
            builder->get_object("List_Wireframe"));

    Gtk::TreeModel::Children rows = pointsList->children();
    Gtk::TreeModel::Row row;
    double x, y, z;

    for (auto it = rows.begin(); it != rows.end(); ++it) {
      row = *it;
      row.get_value(0, x);
      row.get_value(1, y);
      row.get_value(2, z);
      vec->push_back(x);
      vec->push_back(y);
      vec->push_back(z);
    }
  }

  void buttonRemoveOnClick() {}

  void buttonAddOnClick() {
    Gtk::Entry* x;
    Gtk::Entry* y;
    Gtk::Entry* z;

    Gtk::TreeView* pointsTree;
    Glib::RefPtr<Gtk::ListStore> pointsList =
        Glib::RefPtr<Gtk::ListStore>::cast_dynamic(
            builder->get_object("List_Wireframe"));

    builder->get_widget("Tree_Wireframe", pointsTree);
    builder->get_widget("Entry_Wireframe_x", x);
    builder->get_widget("Entry_Wireframe_y", y);
    builder->get_widget("Entry_Wireframe_z", z);

    Gtk::TreeModel::Row row = *pointsList->append();
    row.set_value(0, std::atof(x->get_text().c_str()));
    row.set_value(1, std::atof(y->get_text().c_str()));
    row.set_value(2, std::atof(z->get_text().c_str()));
  }

  void buttonOKOnClick() {
    int tab = tabGroup->get_current_page();

    int colorCode = toHex("0x" + color->get_text());
    int thick = std::atoi(thickness->get_text().c_str());

    if (tab == 0) {
      // Line
      std::vector<std::vector<double> > points;
      tabReta(&points);

      scene->addLine(name->get_text(), colorCode, thick,
                     {points.at(0).at(0), points.at(0).at(1)},
                     {points.at(1).at(0), points.at(1).at(1)});

      // addObjectToList(name->get_text(), "LINE");

    } else if (tab == 1) {
      // Point
      double x, y;
      tabPonto(&x, &y);

      scene->addPoint(name->get_text(), colorCode, thick, {x, y});

    } else if (tab == 2) {
      // Wireframe
      std::vector<double> points;
      tabWireframe(&points);

      if (radioPoligono->get_active()) {
        scene->addPolygon(name->get_text(), colorCode, thick, points);
      } else if (radioWireframe->get_active()) {
        scene->addWireframe(name->get_text(), colorCode, thick, points);
      } else if (radioBezier->get_active()) {
        scene->addBezier(name->get_text(), colorCode, thick, points);
      } else if (radioSpline->get_active()) {
        scene->addSpline(name->get_text(), colorCode, thick, points);
      }
    }

    manageObjectList();
    this->mainWindow->close();
  }

  void buttonCancel() { this->mainWindow->close(); }

  void error(const std::string& errorMessage) {
    Gtk::MessageDialog dialog(*this, "THIS IS A ERROR!");
    dialog.set_secondary_text(errorMessage);
    dialog.run();
  }

  void initializeButton(const std::string& btnName, void (FormObjetos::*f)()) {
    Gtk::Button* btn;
    builder->get_widget(btnName, btn);
    btn->signal_clicked().connect(sigc::mem_fun(*this, f));
  }

  int toHex(const std::string& str) {
    int rtn = std::stoul(str, NULL, 16);
    return rtn;
  }
};

#endif
