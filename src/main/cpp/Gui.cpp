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

#include <FL/Fl.H> // Fl::event()
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <Input_Lite.h>

#if defined(_MSC_VER)  // end of specific msvc warnings removal
#  pragma warning(pop)
#elif defined(__MINGW32__)
#  pragma GCC diagnostic pop
#endif  // _MSC_VER

// as of 2023/08/20, gcc can't do 'import std'. See https://gcc.gnu.org/wiki/LibstdcxxTodo TODO: when it is possible, use it
#include <algorithm> // std::range::for_each
#include <memory> // std::unique_ptr

export module gui.MainWindow;

import gui.Preferences;
import os;

export namespace gui {
class [[nodiscard]] MainWindow final {
private:
  Preferences m_preferences;
  std::unique_ptr<Fl_Double_Window> m_mainWindow;
  std::unique_ptr<Fl_Button> m_recordButton;
  std::unique_ptr<Fl_Button> m_stopButton;
  std::unique_ptr<Fl_Button> m_playButton;
  std::unique_ptr<os::MouseEventListener> m_mouseEventListener;
  void mainWindowCb();
  void recordButtonCb();
  void stopButtonCb();
  void playButtonCb();
public:
  MainWindow();
  ~MainWindow();
  void exit();
  [[nodiscard]] int run();
};
} // namespace gui

module : private;

static constexpr auto STOP_BUTTON_LABEL = "Stop";
static constexpr auto PLAY_BUTTON_LABEL = "Play";
static constexpr auto RECORD_BUTTON_LABEL = "Record";
static constexpr int BUTTON_WIDTH { 100 };
static constexpr int BUTTON_HEIGH { 30 };
static constexpr int SPACE { 5 };

[[nodiscard]] constexpr gui::MainWindow* MAIN_WINDOW(void* self) {
  return static_cast<gui::MainWindow*>(self);
}

void gui::MainWindow::mainWindowCb() {
  // we dont't want the Esc key to close the program
  if (FL_SHORTCUT == Fl::event() && FL_Escape == Fl::event_key()) { return; }

  exit();
}

void gui::MainWindow::recordButtonCb() {
  // starts listening to clicks
  m_mouseEventListener = std::make_unique<os::MouseEventListener>();
  m_recordButton->deactivate();
  m_stopButton->activate();
  m_playButton->deactivate();
}

void gui::MainWindow::stopButtonCb() {
  m_mouseEventListener.reset();
  m_recordButton->activate();
  m_playButton->activate();
}

void gui::MainWindow::playButtonCb() {
  // get the current mouse coordinates
  const auto x { Fl::event_x_root() /* - Fl::event_x()*/};
  const auto y { Fl::event_y_root() /* - Fl::event_y()*/};
  // avoid problematic user actions
  m_recordButton->deactivate();
  m_stopButton->activate();
  // play the captured mouse clicks...
  auto mouseEvents { os::MouseEventListener::getEvents() };
  // excepted the last click on the stop button
  mouseEvents.pop_back();
  mouseEvents.pop_back();
  std::ranges::for_each(mouseEvents, [](const auto & mouseEvent) {
    SL::Input_Lite::SendInput(SL::Input_Lite::MousePositionAbsoluteEvent { .X = mouseEvent.x, .Y = mouseEvent.y });
    SL::Input_Lite::SendInput(SL::Input_Lite::MouseButtonEvent { .Pressed = mouseEvent.isDown, .Button = mouseEvent.button });
  });
  // reset the mouse on the play button
  SL::Input_Lite::SendInput(SL::Input_Lite::MousePositionAbsoluteEvent { .X = x, .Y = y });
  m_recordButton->activate();
}

void gui::MainWindow::exit() {
  /* remember the windows position when closing them */
  if (m_mainWindow) {
    m_preferences.saveSizeAndPosition(m_mainWindow->x(), m_mainWindow->y(), m_mainWindow->w(),
                                      m_mainWindow->h());
  }

  /* hide all windows: this will terminate the MainWindow */
  while (Fl::first_window()) { Fl::first_window()->hide(); }
}

gui::MainWindow::MainWindow()
  : m_preferences(3 * BUTTON_WIDTH + 2 * SPACE, BUTTON_HEIGH) {
  // nothing to do
}

gui::MainWindow::~MainWindow() {
  m_mouseEventListener.reset();
}

//   0
// 0 +---- x
//   |
//   |
//   y
[[nodiscard]] int gui::MainWindow::run() {
  static_assert(0 != std::ssize(APP_NAME), "The APP_NAME macro is undefined!!!");
  const auto [localX, localY] { m_preferences.getMainWindowXY() };
  m_mainWindow = std::make_unique<Fl_Double_Window>(localX, localY, 3 * BUTTON_WIDTH + 2 * SPACE,
                 BUTTON_HEIGH, APP_NAME);
  m_mainWindow->callback([](Fl_Widget*, void* self) { MAIN_WINDOW(self)->mainWindowCb(); }, this);
  m_recordButton = std::make_unique<Fl_Button>(0, 0, BUTTON_WIDTH, BUTTON_HEIGH,
                   RECORD_BUTTON_LABEL);
  m_recordButton->callback([](Fl_Widget*, void* self) { MAIN_WINDOW(self)->recordButtonCb(); }, this);
  m_stopButton = std::make_unique<Fl_Button>(m_recordButton->x() + SPACE + BUTTON_WIDTH,
                 m_recordButton->y(), BUTTON_WIDTH, BUTTON_HEIGH, STOP_BUTTON_LABEL);
  m_stopButton->callback([](Fl_Widget*, void* self) { MAIN_WINDOW(self)->stopButtonCb(); }, this);
  m_stopButton->deactivate();
  m_playButton = std::make_unique<Fl_Button>(m_stopButton->x() + SPACE + BUTTON_WIDTH,
                 m_recordButton->y(), BUTTON_WIDTH, BUTTON_HEIGH, PLAY_BUTTON_LABEL);
  m_playButton->callback([](Fl_Widget*, void* self) { MAIN_WINDOW(self)->playButtonCb(); }, this);
  m_playButton->deactivate();
  m_mainWindow->end();
  m_mainWindow->show();
  return Fl::run();
}
