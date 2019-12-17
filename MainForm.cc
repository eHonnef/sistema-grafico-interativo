#ifndef MAINFORM_H
#define MAINFORM_H

#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "CohenSutherland.cc"
#include "Form.cc"
#include "FormObjetos.cc"
#include "Globals.cc"
#include "NichollLeeNicholl.cc"
#include "ParallelProjection.cc"
#include "PerspectiveProjection.cc"
#include "Scene.cc"
#include "gdk/gdkkeysyms.h"

/**
 * Class that contains the main window components and its event handlers.
 * (signals).
 *
 */
class MainForm : public Form {
 public:
  // Vira membro publico porque retornar um get de um pointer da no mesmo.
  Gtk::Viewport* canvas;

  MainForm() : Form("MainWindow.glade") {
    // Initialize the buttons
    initializeButton("Button_Move_Window_Up", &MainForm::buttonMoveUp);
    initializeButton("Button_Move_Window_Down", &MainForm::buttonMoveDown);
    initializeButton("Button_Move_Window_Left", &MainForm::buttonMoveLeft);
    initializeButton("Button_Move_Window_Right", &MainForm::buttonMoveRight);
    initializeButton("Button_Move_Window_Center", &MainForm::buttonMoveCenter);

    initializeButton("Button_Zoom_Window_In", &MainForm::buttonZoomIn);
    initializeButton("Button_Zoom_Window_Out", &MainForm::buttonZoomOut);

    // initializeButton("Button_Rotate_Window_Anticlockwise",
    //                  &MainForm::buttonRotateWindowLeft);
    // initializeButton("Button_Rotate_Window_Clockwise",
    //                  &MainForm::buttonRotateWindowRight);

    initializeButton("Button_Rotate_Object", &MainForm::buttonRotateObject);

    initializeButton("Button_Move_Object", &MainForm::buttonMove);
    initializeButton("Button_Scale_Object", &MainForm::buttonScale);

    initializeButton("Button_Add_Object", &MainForm::buttonModalShowOnClick);
    initializeButton("Button_Remove_Object", &MainForm::buttonRemoveObject);

    initializeButton("Button_File_Load", &MainForm::buttonLoadFile);
    initializeButton("Button_File_Save", &MainForm::buttonSaveFile);

    // Initialize the file chooser button
    builder->get_widget("FileChooserButton", fileChooser);

    // Initialize Log
    Gtk::TextView* LOG;
    builder->get_widget("Textview_Log", LOG);
    _LOGBOX = static_cast<void*>(LOG);

    // Initialize checkbox
    builder->get_widget("Check_Disable_Clip", checkClipEnabled);
    checkClipEnabled->signal_toggled().connect(
        sigc::mem_fun(*this, &MainForm::clipEnabledOnActive));

    // Initialize radios
    builder->get_widget("Radio_Rotate_Object_Center", radioRotateCenter);
    builder->get_widget("Radio_Rotate_Object_Relative", radioRotateRelative);
    builder->get_widget("Radio_Clip_Nicholl", radioClipNicholl);
    builder->get_widget("Radio_Projection_Parallel", radioProjection);

    // Connecting signals to radio
    radioProjection->signal_clicked().connect(
        sigc::mem_fun(*this, &MainForm::radioProjectionOnChange));

    radioClipNicholl->signal_clicked().connect(
        sigc::mem_fun(*this, &MainForm::radioClipOnChange));

    // Initialize scale
    builder->get_widget("Scale_Rotate_Window_X", windowRotateX);
    builder->get_widget("Scale_Rotate_Window_Y", windowRotateY);
    builder->get_widget("Scale_Rotate_Window_Z", windowRotateZ);

    windowRotateX->signal_value_changed().connect(
        sigc::mem_fun(*this, &MainForm::projectionAngleOnChange));
    windowRotateY->signal_value_changed().connect(
        sigc::mem_fun(*this, &MainForm::projectionAngleOnChange));
    windowRotateZ->signal_value_changed().connect(
        sigc::mem_fun(*this, &MainForm::projectionAngleOnChange));

    // Initialize entry
    builder->get_widget("Entry_Rotate_Object_Degrees", objectRotateDegrees);
    builder->get_widget("Entry_Point_X", x_rotate);
    builder->get_widget("Entry_Point_Y", y_rotate);
    builder->get_widget("Entry_Point_Z", z_rotate);
    builder->get_widget("Entry_Scale_Object", objectScale);

    // Initialize ListStore (object list)
    this->objectList = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(
        builder->get_object("List_Objects"));

    // Initialize canvas (viewport)
    builder->get_widget("Canvas", this->canvas);

    // Initialize treeview
    builder->get_widget("Tree_Objects", this->tree);

    // Connecting signals to tree
    tree->set_activate_on_single_click(true);
    tree->signal_row_activated().connect(
        sigc::mem_fun(*this, &MainForm::rowClick));

    // Initialize the Scene
    this->scene = new Scene(parallel, nln);
    canvas->add(*scene);
    scene->show();

    // Connecting signals to viewport
    canvas->add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK |
                       Gdk::BUTTON_RELEASE_MASK | Gdk::KEY_PRESS_MASK |
                       Gdk::KEY_RELEASE_MASK);

    canvas->signal_scroll_event().connect(
        sigc::mem_fun(*this, &MainForm::viewportOnScroll));
    canvas->signal_motion_notify_event().connect(
        sigc::mem_fun(*this, &MainForm::viewportMouseMoved));
    canvas->signal_button_release_event().connect(
        sigc::mem_fun(*this, &MainForm::viewportMouseButtonRelease));
    canvas->signal_button_press_event().connect(
        sigc::mem_fun(*this, &MainForm::viewportMouseButtonPress));
    canvas->signal_key_press_event().connect(
        sigc::mem_fun(*this, &MainForm::viewportOnKeyPress), false);
    canvas->signal_key_release_event().connect(
        sigc::mem_fun(*this, &MainForm::viewportOnKeyPress), false);

    // Creating form that add objects in the scene
    formObjetos = new FormObjetos(this->canvas, this->scene);
    formObjetos->tree = this->tree;
    formObjetos->objList = this->objectList;

    formObjetos->manageObjectList();
    show_all_children();
  }

  virtual ~MainForm() {}

 private:
  Glib::RefPtr<Gtk::ListStore> objectList;
  Gtk::TreeView* tree;

  Gtk::Entry* objectRotateDegrees;
  Gtk::Entry* x_rotate;
  Gtk::Entry* y_rotate;
  Gtk::Entry* z_rotate;
  Gtk::Entry* objectScale;

  Gtk::Scale* windowRotateX;
  Gtk::Scale* windowRotateY;
  Gtk::Scale* windowRotateZ;

  Gtk::RadioButton* radioRotateCenter;
  Gtk::RadioButton* radioRotateRelative;
  Gtk::RadioButton* radioClipNicholl;
  Gtk::RadioButton* radioProjection;

  Gtk::CheckButton* checkClipEnabled;

  Gtk::FileChooserButton* fileChooser;

  ClippingAlgorithm* cs = new CohenSutherland();
  ClippingAlgorithm* nln = new NichollLeeNicholl();

  Projection* parallel = new ParallelProjection();
  Projection* perspective = new PerspectiveProjection();

  Scene* scene;
  FormObjetos* formObjetos;

  int x_last = -1, y_last = -1;

  std::string to_string_with_precision(const double& a_value, const int n = 6) {
    std::ostringstream out;
    out << std::setprecision(n) << a_value << std::endl;
    return out.str();
  }

  std::string getSelectedObjectName() {
    Gtk::TreeModel::Row row = *tree->get_selection()->get_selected();
    std::string name;
    row.get_value(0, name);
    return name;
  }

  Gtk::TreeModel::Row getSelectedRow() {
    return *tree->get_selection()->get_selected();
  }

  void initializeButton(const std::string& btnName, void (MainForm::*f)()) {
    Gtk::Button* btn;
    builder->get_widget(btnName, btn);
    btn->signal_clicked().connect(sigc::mem_fun(*this, f));
  }

  void clipEnabledOnActive() {
    scene->clipEnabled = !checkClipEnabled->get_active();
    scene->redraw();
  }

  void rowClick(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column) {
    objectScale->set_text("100");
  }

  void projectionAngleOnChange() {
    // scene->projectionAngleX = windowRotateX->get_value();
    // scene->projectionAngleY = windowRotateY->get_value();
    // scene->projectionAngleZ = windowRotateZ->get_value();
    scene->window->xAngle = windowRotateX->get_value();
    scene->window->yAngle = windowRotateY->get_value();
    scene->window->zAngle = windowRotateZ->get_value();
    scene->redraw();
  }

  bool viewportOnScroll(GdkEventScroll* scroll_event) {
    if (scroll_event->direction == GDK_SCROLL_UP)
      scene->zoomIn();
    else if (scroll_event->direction == GDK_SCROLL_DOWN)
      scene->zoomOut();

    scene->redraw();
    return true;
  }

  bool viewportMouseMoved(GdkEventMotion* event) {
    if (event->type == Gdk::MOTION_NOTIFY) {
      scene->window->xAngle -= event->y / 90;
      scene->window->yAngle -= event->x / 90;
      windowRotateX->set_value(scene->window->xAngle);
      windowRotateY->set_value(scene->window->yAngle);

      scene->redraw();
    }
    /*
    if (event->type == Gdk::MOTION_NOTIFY) {
      if (x_last != -1) scene->window->normalLeft(x_last - event->x);

      if (y_last != -1) scene->window->normalDown(y_last - event->y);

      scene->redraw();
      x_last = event->x;
      y_last = event->y;
    }
    */
    return true;
  }

  bool viewportMouseButtonRelease(GdkEventButton* event) {
    if (event->type == Gdk::BUTTON_RELEASE) {
      y_last = -1;
      x_last = -1;
    }
    return true;
  }

  bool viewportMouseButtonPress(GdkEventButton* event) {
    if (event->type == Gdk::BUTTON_PRESS) {
      x_rotate->set_text(to_string_with_precision(scene->transformX(event->x)));
      y_rotate->set_text(to_string_with_precision(scene->transformY(event->y)));
      z_rotate->set_text("1.00");
    }
    return true;
  }

  bool viewportOnKeyPress(GdkEventKey* key_event) {
    if (key_event->keyval == GDK_KEY_W || key_event->keyval == GDK_KEY_w) {
      writeToLog("w");
    }

    if (key_event->keyval == GDK_KEY_A || key_event->keyval == GDK_KEY_a) {
      writeToLog("a");
    }

    if (key_event->keyval == GDK_KEY_S || key_event->keyval == GDK_KEY_s) {
      writeToLog("s");
    }

    if (key_event->keyval == GDK_KEY_D || key_event->keyval == GDK_KEY_d) {
      writeToLog("d");
    }

    return true;
  }

  void buttonModalShowOnClick() { formObjetos->mainWindow->show(); }

  void buttonRemoveObject() {
    if (scene->removeGraphicElement(getSelectedObjectName()))
      objectList->erase(getSelectedRow());
  }

  void buttonMoveUp() {
    scene->window->foward();
    scene->redraw();
  }

  void buttonMoveDown() {
    scene->window->backward();
    scene->redraw();
  }

  void buttonMoveLeft() {
    scene->window->left();
    scene->redraw();
  }

  void buttonMoveRight() {
    scene->window->right();
    scene->redraw();
  }
  /*
    void buttonRotateWindowLeft() {
      double degrees = std::atof(windowRotateDegrees->get_text().c_str());
      scene->rotateWindowACW(degrees);
    }

    void buttonRotateWindowRight() {
      double degrees = std::atof(windowRotateDegrees->get_text().c_str());
      scene->rotateWindowCW(degrees);
    }
  */
  void buttonRotateObject() {
    double degrees = std::atof(objectRotateDegrees->get_text().c_str());
    double x = std::atof(x_rotate->get_text().c_str());
    double y = std::atof(y_rotate->get_text().c_str());
    double z = std::atof(z_rotate->get_text().c_str());

    if (radioRotateRelative->get_active())
      scene->rotateGraphicElement(getSelectedObjectName(), degrees, x, y, z);
    else if (radioRotateCenter->get_active())
      scene->rotateGraphicElement(getSelectedObjectName(), degrees);
  }

  void buttonScale() {
    double scale = std::atof(objectScale->get_text().c_str()) / 100;

    scene->scaleGraphicElement(getSelectedObjectName(), scale);
  }

  void buttonMove() {
    double x = std::atof(x_rotate->get_text().c_str());
    double y = std::atof(y_rotate->get_text().c_str());
    double z = std::atof(z_rotate->get_text().c_str());

    scene->moveGraphicElement(getSelectedObjectName(), x, y, z);
  }

  void buttonZoomIn() { scene->zoomIn(); }

  void buttonZoomOut() { scene->zoomOut(); }

  void buttonMoveCenter() {
    scene->window->setInitialSize(scene->get_allocation().get_width(),
                                  scene->get_allocation().get_height());
    scene->redraw();
  }

  void buttonSaveFile() {
    if (fileChooser->get_filename().empty())
      return writeToLog("Unable to save to file. File is not valid.");

    if (scene->displayFile->saveToFile(fileChooser->get_filename())) {
      scene->redraw();
      formObjetos->manageObjectList();
      return writeToLog("Success to save to file: " +
                        fileChooser->get_filename());
    }
  }

  void buttonLoadFile() {
    if (fileChooser->get_filename().empty())
      return writeToLog("Unable to load file. File is not valid.");

    if (scene->displayFile->loadFromFile(fileChooser->get_filename())) {
      formObjetos->manageObjectList();
      scene->redraw();
      return writeToLog("Success to load from file.");
    }
  }

  void radioProjectionOnChange() {
    if (radioProjection->get_active()) return scene->setProjection(parallel);

    scene->setProjection(perspective);
  }

  void radioClipOnChange() {
    if (radioClipNicholl->get_active()) return scene->setLineClipper(nln);

    scene->setLineClipper(cs);
  }
};

#endif
