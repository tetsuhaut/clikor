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
export class [[nodiscard]] Preferences final {
private:
  std::unique_ptr<Fl_Preferences> m_preferences { std::make_unique<Fl_Preferences>(Fl_Preferences::USER,
        APP_NAME, APP_NAME) }; // TODO : check that APP_NAME macro is defined at compilation
public:
  void saveWindowX(int x);
  void saveWindowY(int y);
  void saveWindowWidth(int width);
  void saveWindowHeight(int height);
  void saveHistoryDir(const std::filesystem::path& dir);
  std::string getHandHistoryDir() const;
  [[nodiscard]] std::tuple<int, int, int, int> getMainWindowXYWH() const;
  void saveSizeAndPosition(const std::array<int, 4>& xywh);
  Preferences() = default;
  Preferences(const Preferences&) = delete;
  ~Preferences() = default;
  Preferences& operator=(const Preferences& t) = delete;
}; // class Preferences

module : private;

namespace detail {
static constexpr int DEFAULT_WIDTH = 800;
static constexpr int DEFAULT_HEIGHT = 600;
static constexpr std::string_view MAIN_WINDOW_X = "mainwindowx";
static constexpr std::string_view MAIN_WINDOW_Y = "mainwindowy";
static constexpr std::string_view MAIN_WINDOW_WIDTH = "mainwindoww";
static constexpr std::string_view MAIN_WINDOW_HEIGHT = "mainwindowh";

void save(Fl_Preferences& fltkPreferences, std::string_view key, auto&& value) {
  if (0 == fltkPreferences.set(key.data(), std::forward<decltype(value)>(value))) {
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

[[nodiscard]] std::tuple<int, int, int, int> getGenericWindowXYWH(
  Fl_Preferences& fltkPreferences,
  std::string_view xname,
  std::string_view yname,
  std::string_view widthname,
  std::string_view heightname) {
  /* get the previous width and height from preferences, if any */
  auto width { getIntWithMin(fltkPreferences, widthname, DEFAULT_WIDTH) };
  auto height { getIntWithMin(fltkPreferences, heightname, DEFAULT_HEIGHT) };
  /* compute the center position */
  int dummyX, dummyY, screenWidth, screenHeight;
  Fl::screen_xywh(dummyX, dummyY, screenWidth, screenHeight);
  /* get the previous position from preferences. if none, use the center position */
  auto x { getIntWithDefault(fltkPreferences, xname, (screenWidth - width) / 2) };
  auto y { getIntWithDefault(fltkPreferences, yname, (screenHeight - height) / 2) };
  return { x, y, width, height };
}
} // namespace detail

void Preferences::saveSizeAndPosition(const std::array<int, 4>& xywh) {
  const auto& [x, y, w, h] { xywh };
  saveWindowX(x);
  saveWindowY(y);
  saveWindowWidth(w);
  saveWindowHeight(h);
}

void Preferences::saveWindowX(int x) {
  detail::save(*m_preferences, detail::MAIN_WINDOW_X, x);
}

void Preferences::saveWindowY(int y) {
  detail::save(*m_preferences, detail::MAIN_WINDOW_Y, y);
}

void Preferences::saveWindowWidth(int width) {
  detail::save(*m_preferences, detail::MAIN_WINDOW_WIDTH, width);
}

void Preferences::saveWindowHeight(int height) {
  detail::save(*m_preferences, detail::MAIN_WINDOW_HEIGHT, height);
}

[[nodiscard]] std::tuple<int, int, int, int> Preferences::getMainWindowXYWH() const {
  return detail::getGenericWindowXYWH(*m_preferences, detail::MAIN_WINDOW_X, detail::MAIN_WINDOW_Y,
                                      detail::MAIN_WINDOW_WIDTH,
                                      detail::MAIN_WINDOW_HEIGHT);
}
