#include "Window/GlfwCallbacks.h"
#include "Window/DrawGui.h"
#include "Window/GlfwWindow.h"

void GlfwCallbacks::Register(GLFWwindow* window)
{
    glfwSetKeyCallback(window, Keyboard);
    glfwSetCursorPosCallback(window, MouseCursor);
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetScrollCallback(window, MouseScroll);
    glfwSetFramebufferSizeCallback(window, WindowResize);
    glfwSetDropCallback(window, DragAndDrop);
}
//This function gets called when a key is pressed
void GlfwCallbacks::Keyboard(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
{
    //std::cout << "key : " << key << ", " << char(key) << ", scancode: " << scancode << ", action: " << action << ", mods: " << mods << std::endl;
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case 'r':
        case 'R':

            break;

        case GLFW_KEY_F1:
            DrawGui::HideGui = !DrawGui::HideGui;
            break;

        case GLFW_KEY_ESCAPE:
            //glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;

        case GLFW_KEY_PRINT_SCREEN:
            //screenshot.Grab();
            break;
        }
    }

}

//This function gets called when the mouse moves over the window.
void GlfwCallbacks::MouseCursor(GLFWwindow* glfw_window, double x, double y)
{
    //std::cout << "cursor pos: " << x << ", " << y << std::endl;
}

//This function gets called when a mouse button is pressed.
void GlfwCallbacks::MouseButton(GLFWwindow* glfw_window, int button, int action, int mods)
{
    //std::cout << "button : " << button << ", action: " << action << ", mods: " << mods << std::endl;
}

void GlfwCallbacks::MouseScroll(GLFWwindow* glfw_window, double xoffset, double yoffset)
{
    //std::cout << "xoffset : " << xoffset << ", yoffset: " << yoffset << std::endl;
}

void GlfwCallbacks::WindowResize(GLFWwindow* glfw_window, int width, int height)
{
    //std::cout << "width : " << width << ", height: " << height << std::endl;

    glViewport(0, 0, width, height);
    GlfwWindow::Size[0] = width;
    GlfwWindow::Size[1] = height;
    GlfwWindow::Aspect = float(GlfwWindow::Size[0]) / float(GlfwWindow::Size[1]);

}

void GlfwCallbacks::DragAndDrop(GLFWwindow* window, int count, const char** paths)
{

}