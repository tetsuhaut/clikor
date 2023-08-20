module;

#if defined(_MSC_VER) // removal of specific msvc warnings due to FLTK
#  pragma warning(push)
#  pragma warning(disable : 4191 4244 4365 4458 4514 4625 4626 4668 4820 5026 5027 )
#endif  // _MSC_VER

#include <FL/Fl.H> // assert
#include <FL/fl_ask.H> // fl_alert
#include <FL/Fl_Preferences.H>

#if defined(_MSC_VER)  // end of specific msvc warnings removal
#  pragma warning(pop)
#endif  // _MSC_VER

#include <array>
#include <cassert>
#include <filesystem>
#include <format>
#include <memory> // std::unique_ptr
#include <string_view>
#include <tuple>

export module gui.Preferences;

/**
 * User preferences. This class encapsulates the FLTK user preference management.
 */
export namespace gui {
  class [[nodiscard]] Preferences final {
  private:
#ifndef APP_VENDOR
#  error The macro APP_VENDOR should have been defined in CMakeLists.txt
#endif // APP_VENDOR
#ifndef APP_NAME
#  error The macro APP_NAME should have been defined in CMakeLists.txt
#endif // APP_NAME
    std::unique_ptr<Fl_Preferences> m_preferences { std::make_unique<Fl_Preferences>(
        Fl_Preferences::USER,
        APP_VENDOR,
        APP_NAME)
    };
    int m_defaultWidth;
    int m_defaultHeight;
  public:
    void saveWindowX(int x);
    void saveWindowY(int y);
    void saveWindowWidth(int width);
    void saveWindowHeight(int height);
    void saveHistoryDir(const std::filesystem::path& dir);
    std::string getHandHistoryDir() const;
    [[nodiscard]] std::pair<int, int> getMainWindowXY() const;
    void saveSizeAndPosition(const std::array<int, 4>& xywh);
    Preferences(int defaultWidth, int defaultHeight);
    Preferences(const Preferences&) = delete;
    ~Preferences() = default;
    Preferences& operator=(const Preferences& t) = delete;
  }; // class Preferences
} // namespace gui

module : private;

namespace detail {
static constexpr std::string_view MAIN_WINDOW_X_NAME = "mainwindowx";
static constexpr std::string_view MAIN_WINDOW_Y_NAME = "mainwindowy";
static constexpr std::string_view MAIN_WINDOW_WIDTH = "mainwindoww";
static constexpr std::string_view MAIN_WINDOW_HEIGHT = "mainwindowh";
static constexpr int FL_ERROR = 0; // comes from FLTK documentation

void save(Fl_Preferences& fltkPreferences, std::string_view key, auto&& value) {
  if (FL_ERROR == fltkPreferences.set(key.data(), std::forward<decltype(value)>(value))) {
    fl_alert(std::format("Couldn't save '{}' into the preferences repository.", key).c_str());
  }
}

int getIntWithMin(Fl_Preferences& fltkPreferences, std::string_view key, int minValue) {
  int value;
  fltkPreferences.get(key.data(), value, minValue);
  return (value < minValue) ? minValue : value;
}

int getIntWithDefault(Fl_Preferences& fltkPreferences, std::string_view key, int defaultValue) {
  int value;
  fltkPreferences.get(key.data(), value, defaultValue);
  return value;
}

std::string getString(Fl_Preferences& fltkPreferences, std::string_view key) {
  char value[512];
  fltkPreferences.get(key.data(), &value[0], "", (int)std::size(value) - 1);
  return value;
}

} // namespace detail

void gui::Preferences::saveSizeAndPosition(const std::array<int, 4>& xywh) {
  const auto& [x, y, w, h] { xywh };
  saveWindowX(x);
  saveWindowY(y);
  saveWindowWidth(w);
  saveWindowHeight(h);
}

void gui::Preferences::saveWindowX(int x) {
  detail::save(*m_preferences, detail::MAIN_WINDOW_X_NAME, x);
}

void gui::Preferences::saveWindowY(int y) {
  detail::save(*m_preferences, detail::MAIN_WINDOW_Y_NAME, y);
}

void gui::Preferences::saveWindowWidth(int width) {
  detail::save(*m_preferences, detail::MAIN_WINDOW_WIDTH, width);
}

void gui::Preferences::saveWindowHeight(int height) {
  detail::save(*m_preferences, detail::MAIN_WINDOW_HEIGHT, height);
}

[[nodiscard]] std::pair<int, int> gui::Preferences::getMainWindowXY() const {
  /* compute the center position */
  int dummyX, dummyY, screenWidth, screenHeight;
  Fl::screen_xywh(dummyX, dummyY, screenWidth, screenHeight);
  /* get the previous position from preferences. if none, use the center position */
  auto x { detail::getIntWithDefault(*m_preferences, detail::MAIN_WINDOW_X_NAME, (screenWidth - m_defaultWidth) / 2) };
  auto y { detail::getIntWithDefault(*m_preferences, detail::MAIN_WINDOW_Y_NAME, (screenHeight - m_defaultHeight) / 2) };
  return { x, y };
}

gui::Preferences::Preferences(int defaultWidth, int defaultHeight)
  : m_defaultWidth { defaultWidth },
    m_defaultHeight { defaultHeight } {
  // nothing to do
}