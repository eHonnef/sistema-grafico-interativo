#ifndef GLOBALS_H
#define GLOBALS_H

#include <gtkmm/textview.h>
#include <string>
#include <sstream>
#include "Point.cc"

static void* _LOGBOX;

const double xMin = -1;
const double yMin = -1;
const double xMax = 1;
const double yMax = 1;

static void writePointToLog(const Point& p, const std::string& name) {
  Gtk::TextView* txt = static_cast<Gtk::TextView*>(_LOGBOX);
  Glib::RefPtr<Gtk::TextBuffer> buffer = txt->get_buffer();
  Gtk::TextBuffer::iterator iter = buffer->get_iter_at_offset(0);

  std::ostringstream strs;
  strs << "\n-------" << name << "-------";
  strs << "\nX: " << p.x << ", Y: " << p.y << ", Z: " << p.z;
  strs << "\n--------------------";

  iter = buffer->insert(iter, strs.str() + "\n");
}

static void writeToLog(const std::string& message) {
  Gtk::TextView* txt = static_cast<Gtk::TextView*>(_LOGBOX);
  Glib::RefPtr<Gtk::TextBuffer> buffer = txt->get_buffer();
  Gtk::TextBuffer::iterator iter = buffer->get_iter_at_offset(0);

  iter = buffer->insert(iter, message + "\n");
}

#endif  // !GLOBALS_H
