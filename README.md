# Clikor
Clikor clicks for you!

## What is it
This program is designed to automate a sequence of mouse clicks.

## How to use it
Start the program, click "Record", do your clicks, click "Stop". Then, each time
you press the "Start" button, the recorded sequence of clicks will be redone,
excluding the first ("Record") and last ("Stop") ones.

# How to build
Execute the build-vs2022.bat script, then open the project file with Visual
Studio and compile it.

# Some ideas on how to handle mouse events in a cross-plateform way
As I did not find a library yet to do that, I started to implement using the Windows API which is
really unfortunate.

One idea would be to use a transparent widget with Qt https://forum.qt.io/topic/83161/click-through-windows/9
that would occupy the whole screen, letting the user interaction go throught, and capturing the
mouse events.<br>
With FLTK it could be feasible following https://stackoverflow.com/questions/26686382/how-can-i-make-a-window-transparent-in-windows-mac-os-x-and-linux-using-fltk

Another idea is to use wxwidget which has a promising method [void wxWindow::CaptureMouse()](https://docs.wxwidgets.org/3.2/classwx_window.html#a5c72c6260a73ef77bb0b1f7ec85fcfef)
I shall try this one after the Windows only way is implemented.

# TODOs
- try to implement the MouseEventListener in a more portable way (see above)
