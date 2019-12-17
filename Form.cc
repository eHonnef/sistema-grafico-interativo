#ifndef FORM_H
#define FORM_H
#include <gtk-3.0/gtk/gtk.h>
#include <gtkmm.h>
#include <gtkmm/application.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/main.h>
#include <gtkmm/viewport.h>
#include <gtkmm/window.h>
#include <string>

class Form : public Gtk::Window {
 public:
  Gtk::Window* mainWindow;
  Glib::RefPtr<Gtk::Builder> builder;

  Form(const std::string& GUIfile) {
    // Loading components from glade file
    this->builder = Gtk::Builder::create_from_file(GUIfile.c_str());

    // Getting main window
    this->builder->get_widget("MainWindow", this->mainWindow);
  }
  ~Form() {}

 protected:
  void windowOnClose() {
    // TODO: signal handler para quando as janelas fecham
  }
};

#endif
