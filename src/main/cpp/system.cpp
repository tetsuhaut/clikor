module;

#include <format>
#include <Windows.h>
#include <iostream>

export module system;

export namespace os {
/**
 * Encapsulates the mouse hook installation/uninstallation in the Windows OS.
 */
class [[nodiscard]] MouseEventListener final {
private:
  HHOOK m_mouseHook; // handle to the mouse hook
  static LRESULT CALLBACK mouseCallback(int nCode, WPARAM wParam, LPARAM lParam) noexcept;
public:
  MouseEventListener();
  ~MouseEventListener();
}; // class MouseEventListener
} // namespace system

module : private;

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
LRESULT CALLBACK os::MouseEventListener::mouseCallback(int nCode, WPARAM event, LPARAM data) noexcept {
  try {
    if (HC_ACTION == nCode) {
      const auto& mouseData { *reinterpret_cast<MSLLHOOKSTRUCT*>(data) };

      switch (event) {
        case WM_LBUTTONDOWN: {
          std::cout << std::format("LEFT CLICK DOWN x={} y={}\n", mouseData.pt.x, mouseData.pt.y);
        } break;

        case WM_LBUTTONUP: {
          std::cout << std::format("LEFT CLICK UP x={} y={}\n", mouseData.pt.x, mouseData.pt.y);
        } break;
      }
    }
  } catch (...) {
    std::cerr << "Error in mouseCallback!!!\n";
    std::cerr << getLastErrorMessageFromOS() << '\n';
  }

  return CallNextHookEx(nullptr, nCode, event, data);
}