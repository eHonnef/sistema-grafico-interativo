#ifdef __linux__

#elif _WIN32
//#include <gtk-3.0/gtk/gtk.h>
#endif
#include <gtkmm/application.h>
#include <cstdio>
#include "MainForm.cc"

int main(int argc, char *argv[]) {
  try {
    // Iniciando aplicacao
    auto app = Gtk::Application::create(argc, argv, "");
    MainForm *mf = new MainForm();
    return app->run(*mf->mainWindow);
  } catch (std::exception &e) {
    // std::printf("%s", e);
    throw e;
  }
}
