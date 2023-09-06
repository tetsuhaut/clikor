module;

#include <Input_Lite.h> // SL::Input_Lite::MouseButtons
#include <Windows.h>

#include <format>
#include <iostream>
#include <vector>

export module os;

export namespace os {
struct [[nodiscard]] MouseEvent final {
  SL::Input_Lite::MouseButtons button;
  bool isDown;
  long x;
  long y;
};

/**
 * Encapsulates the mouse hook installation/uninstallation in the Windows OS.
 */
class [[nodiscard]] MouseEventListener final {
private:
  HHOOK m_mouseHook; // handle to the mouse hook
  static LRESULT CALLBACK mouseCallback(int nCode, WPARAM wParam, LPARAM lParam) noexcept;
  static std::vector<MouseEvent> m_mouseEvents;
public:
  MouseEventListener();
  ~MouseEventListener();
  [[nodiscard]] constexpr std::vector<MouseEvent> getEvents() noexcept { return m_mouseEvents; };
}; // class MouseEventListener
} // namespace os

module : private;

// static member variables must be defined outside class body
std::vector<os::MouseEvent> os::MouseEventListener::m_mouseEvents;

[[nodiscard]] static std::string getLastErrorMessageFromOS() {
  char err[MAX_PATH + 1] { '\0' };
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &err[0], MAX_PATH,
                nullptr);
  return &err[0];
}

/**
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexa
 */
os::MouseEventListener::MouseEventListener()
  : m_mouseHook(nullptr) {
  m_mouseEvents.clear();

  if (nullptr == (m_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseCallback, nullptr, 0))) {
    std::cerr << getLastErrorMessageFromOS() << '\n';
    std::cerr << std::format("Error code {} \n", GetLastError());
  }
}

os::MouseEventListener::~MouseEventListener() {
  UnhookWindowsHookEx(m_mouseHook);
}

/**
 * @see https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelmouseproc
 */
LRESULT CALLBACK os::MouseEventListener::mouseCallback(int nCode, WPARAM event,
    LPARAM data) noexcept {
  try {
    if (HC_ACTION == nCode) {
      const auto& mouseData { *reinterpret_cast<MSLLHOOKSTRUCT*>(data) };

      switch (event) {
        case WM_LBUTTONDOWN: {
          m_mouseEvents.push_back({.button = SL::Input_Lite::MouseButtons::LEFT, .isDown = true, .x = mouseData.pt.x, .y = mouseData.pt.y });
          std::cout << "x=" << mouseData.pt.x << "y=" << mouseData.pt.y << '\n';
        } break;

        case WM_LBUTTONUP: {
          m_mouseEvents.push_back({ .button = SL::Input_Lite::MouseButtons::LEFT, .isDown = false, .x = mouseData.pt.x, .y = mouseData.pt.y });
        } break;

        case WM_MBUTTONDOWN: {
          m_mouseEvents.push_back({ .button = SL::Input_Lite::MouseButtons::MIDDLE, .isDown = true, .x = mouseData.pt.x, .y = mouseData.pt.y });
        } break;

        case WM_MBUTTONUP: {
          m_mouseEvents.push_back({ .button = SL::Input_Lite::MouseButtons::MIDDLE, .isDown = false, .x = mouseData.pt.x, .y = mouseData.pt.y });
        } break;

        case WM_RBUTTONDOWN: {
          m_mouseEvents.push_back({ .button = SL::Input_Lite::MouseButtons::RIGHT, .isDown = true, .x = mouseData.pt.x, .y = mouseData.pt.y });
        } break;

        case WM_RBUTTONUP: {
          m_mouseEvents.push_back({ .button = SL::Input_Lite::MouseButtons::RIGHT, .isDown = false, .x = mouseData.pt.x, .y = mouseData.pt.y });
        } break;
      }
    }
  } catch (...) {
    std::cerr << "Error in mouseCallback!!!\n";
    std::cerr << getLastErrorMessageFromOS() << '\n';
  }

  return CallNextHookEx(nullptr, nCode, event, data);
}