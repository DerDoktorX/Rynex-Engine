#include "rypch.h"
#include "Windows.h"

#include <Platform/GLFW/WindowsWindow.h>

namespace Rynex{


    GLFWwindow* Window::GetWindowGLFWwindowPt(Weak<Window> windowWeak)
    {
        if (Ref<Window> window = windowWeak.lock())
        {
            Ref<WindowsWindow> windowsWindow = std::dynamic_pointer_cast<WindowsWindow, Window>(window);
            RY_CORE_ASSERT(nullptr != windowsWindow, "window was not a WindowsWindow class!");
            GLFWwindow* windowPtr = windowsWindow->GetWindowPtr();
            return windowPtr;
        }
        RY_CORE_ASSERT(false, "windowWeak is already destroyed!");
        return nullptr;
    }

}