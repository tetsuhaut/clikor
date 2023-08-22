module;

#include <Windows.h>
#include <stdio.h>

export module system;

export namespace os {
  LRESULT CALLBACK MyMouseCallback(int nCode, WPARAM wParam, LPARAM lParam);

  class MouseEventListener {
  private:
    HHOOK mouseHook { nullptr }; // handle to the mouse hook
    HHOOK windowHook { nullptr }; // handle to the window hook
    MSG msg {}; // struct with information about all messages in our queue
    friend LRESULT CALLBACK MyMouseCallback(int nCode, WPARAM wParam, LPARAM lParam);
  public:
    static MouseEventListener& Instance() {
      static MouseEventListener myHook;
      return myHook;
    }

    // function to install our mouseHook
    void startListening();

    // function to uninstall our mouseHook
    void stopListening();

    // function to "deal" with our messages
    int Messsages();
  }; // class MouseEventListener
} // namespace system

module : private;

void os::MouseEventListener::startListening() {
    /*
    SetWindowHookEx(
    WM_MOUSE_LL = mouse low level mouseHook type,
    MyMouseCallback = our callback function that will deal with system messages about mouse
    NULL, 0);

    c++ note: we can check the return SetWindowsHookEx like this because:
    If it return NULL, a NULL value is 0 and 0 is false.
    */
    if (nullptr == (mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MyMouseCallback, NULL, 0))) {
        printf_s("Error: %lu \n", GetLastError());
    }
}

// function to uninstall our mouseHook
void os::MouseEventListener::stopListening() {
    UnhookWindowsHookEx(mouseHook);
}

// function to "deal" with our messages
int os::MouseEventListener::Messsages()
{
    // while we do not close our application
    while (WM_QUIT != msg.message) {
        if (GetMessage(&msg, NULL, 0, 0/*, PM_REMOVE*/)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(1);
    }
    stopListening(); // if we close, let's uninstall our mouseHook
    return (int)msg.wParam; // return the messages
}

LRESULT CALLBACK os::MyMouseCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam; // WH_MOUSE_LL struct
    /*
    nCode, this parameters will determine how to process a message
    This callback in this case only have information when it is 0 (HC_ACTION): wParam and lParam contain info

    wParam is about WINDOWS MESSAGE, in this case MOUSE messages.
    lParam is information contained in the structure MSLLHOOKSTRUCT
    */

    // we have information in wParam/lParam ? If yes, let's check it:
    if (nCode == 0) { 
        // Mouse struct contain information?    
        // if (pMouseStruct != NULL)
        // {
        //  printf_s("Mouse Coordinates: x = %i | y = %i \n", pMouseStruct->pt.x, pMouseStruct->pt.y);
        // }

        switch (wParam) {
            case WM_LBUTTONDOWN: {
                printf_s("LEFT CLICK DOWN\n");
            } break;
            case WM_LBUTTONUP: {
                printf_s("LEFT CLICK UP\n");
            } break;
        }
    }

    /*
    Every time that the nCode is less than 0 we need to CallNextHookEx:
    -> Pass to the next mouseHook
         MSDN: Calling CallNextHookEx is optional, but it is highly recommended;
         otherwise, other applications that have installed hooks will not receive mouseHook notifications and may behave incorrectly as a result.
    */
    return CallNextHookEx(MouseEventListener::Instance().mouseHook, nCode, wParam, lParam);
}