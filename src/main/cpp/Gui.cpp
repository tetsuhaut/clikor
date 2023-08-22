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
#include <cassert>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>

export module gui.MainWindow;

import gui.Preferences;
import system;

export namespace gui {
class [[nodiscard]] MainWindow final {
private:
  Preferences m_preferences; 
  std::unique_ptr<Fl_Double_Window> m_mainWindow;
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

[[nodiscard]] gui::MainWindow* MAIN_WINDOW(void* self) {
  return static_cast<gui::MainWindow*>(self);
}

static void mainWindowCb(Fl_Widget*, void* mainWindow) {
  // we dont't want the Esc key to close the program
  if (FL_SHORTCUT == Fl::event() && FL_Escape == Fl::event_key()) { return; }

  MAIN_WINDOW(mainWindow)->exit();
}

Fl_Widget* getButton(Fl_Widget* button, std::string_view label) {
  auto* pGroup { button->parent() };
  const auto nbChildren { pGroup->children() };

  for (auto i = 0; i < nbChildren; ++i) {
    if (label == pGroup->child(i)->label()) {
      return pGroup->child(i);
    }
  }
  auto errMsg { std::format("couldn't find the '{}' button", label) };
  assert(false && errMsg.c_str());
  return nullptr;
}

std::unique_ptr<os::MouseEventListener> m_listener;

static void recordButtonCb(Fl_Widget* recordButton, void* mainWindow) {
  // starts listening to clicks
  m_listener = std::make_unique<os::MouseEventListener>();
  recordButton->deactivate();
  getButton(recordButton, STOP_BUTTON_LABEL)->activate();
}

static void stopButtonCb(Fl_Widget* stopButton, void* mainWindow) {
  m_listener.reset();
  getButton(stopButton, RECORD_BUTTON_LABEL)->activate();
  getButton(stopButton, PLAY_BUTTON_LABEL)->activate();
}

static void playButtonCb(Fl_Widget* playButton, void* mainWindow) {
  //SL::Input_Lite::
  getButton(playButton, RECORD_BUTTON_LABEL)->deactivate();
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

//   0
// 0 +---- x
//   |
//   |
//   y
[[nodiscard]] int gui::MainWindow::run() {
  const auto [localX, localY] { m_preferences.getMainWindowXY() };
  m_mainWindow = std::make_unique<Fl_Double_Window>(localX, localY, 3 * BUTTON_WIDTH + 2 * SPACE,
                 BUTTON_HEIGH, "Clikor");
  m_mainWindow->callback(mainWindowCb, this);
  auto recordButton = std::make_unique<Fl_Button>(0, 0, BUTTON_WIDTH, BUTTON_HEIGH, RECORD_BUTTON_LABEL);
  recordButton->callback(recordButtonCb, this);
  auto stopButton = std::make_unique<Fl_Button>(recordButton->x() + SPACE + BUTTON_WIDTH,
                    recordButton->y(), BUTTON_WIDTH, BUTTON_HEIGH, STOP_BUTTON_LABEL);
  stopButton->callback(stopButtonCb, this);
  stopButton->deactivate();
  auto playButton = std::make_unique<Fl_Button>(stopButton->x() + SPACE + BUTTON_WIDTH,
                    recordButton->y(), BUTTON_WIDTH, BUTTON_HEIGH, PLAY_BUTTON_LABEL);
  playButton->callback(playButtonCb, this);
  playButton->deactivate();
  m_mainWindow->end();
  m_mainWindow->show();
  return Fl::run();
}

gui::MainWindow::MainWindow()
  : m_preferences(3 * BUTTON_WIDTH + 2 * SPACE, BUTTON_HEIGH) {
  // nothing to do
}

gui::MainWindow::~MainWindow() {
  m_listener.reset();
}