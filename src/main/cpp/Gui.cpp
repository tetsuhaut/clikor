module;

#if defined(_MSC_VER) // removal of specific msvc warnings due to FLTK
#  pragma warning(push)
#  pragma warning(disable : 4100 4191 4242 4244 4266 4365 4458 4514 4625 4626 4668 4820 5026 5027 5219 )
#elif defined(__MINGW32__) // removal of specific gcc warnings due to FLTK
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wcast-function-type"
#  pragma GCC diagnostic ignored "-Wsuggest-override"
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Weffc++"
#endif  // _MSC_VER

#include <FL/Fl.H> // Fl::*
#include <FL/FL_ask.H> // fl_alert
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Tree.H>

#if defined(_MSC_VER)  // end of specific msvc warnings removal
#  pragma warning(pop)
#elif defined(__MINGW32__)
#  pragma GCC diagnostic pop
#endif  // _MSC_VER

#include <memory>
#include <string>
#include <tuple>

export module gui.MainWindow;

import gui.Preferences;

export namespace gui {
class [[nodiscard]] MainWindow final {
private:
  Preferences m_preferences;
  std::unique_ptr<Fl_Double_Window> m_mainWindow;
public:
  void exit();
  [[nodiscard]] int run();
};
}
module : private;

[[nodiscard]] gui::MainWindow* MAIN_WINDOW(void* self) {
  return static_cast<gui::MainWindow*>(self);
}

void mainWindowCb(Fl_Widget*, void* mainWindow) {
  // we dont't want the Esc key to close the program
  if (FL_SHORTCUT == Fl::event() && FL_Escape == Fl::event_key()) { return; }

  MAIN_WINDOW(mainWindow)->exit();
}

void gui::MainWindow::exit() {
  /* remember the windows position when closing them */
  if (m_mainWindow) {
    std::array xywh {m_mainWindow->x(), m_mainWindow->y(), m_mainWindow->w(), m_mainWindow->h()};
    m_preferences.saveSizeAndPosition(xywh);
  }

  /* hide all windows: this will terminate the MainWindow */
  while (Fl::first_window()) { Fl::first_window()->hide(); }
}

[[nodiscard]] int gui::MainWindow::run() {
  const auto [localX, localY, width, height] { m_preferences.getMainWindowXYWH() };
  m_mainWindow = std::make_unique<Fl_Double_Window>(localX, localY, width, height, "Clikor");
  m_mainWindow->callback(mainWindowCb, this);
  m_mainWindow->end();
  m_mainWindow->show();
  return Fl::run();
}
