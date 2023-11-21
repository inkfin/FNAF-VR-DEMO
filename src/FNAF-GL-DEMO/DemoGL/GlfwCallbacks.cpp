//
// Created by 11096 on 11/19/2023.
//

#include <Shader.h>
#include "Scene.h"
#include "Camera.h"

#include <Window/GlfwCallbacks.h>
#include <Window/DrawGui.h>
#include <Game/GlobalObjects.h>

void GlfwCallbacks::Register(GLFWwindow* window)
{
    glfwSetKeyCallback(window, Keyboard);
    glfwSetCursorPosCallback(window, MouseCursor);
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetScrollCallback(window, MouseScroll);
    glfwSetFramebufferSizeCallback(window, WindowResize);
    glfwSetDropCallback(window, DragAndDrop);
}

// This function gets called when a key is pressed
void GlfwCallbacks::Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // std::cout << "key : " << key << ", " << char(key) << ", scancode: " << scancode << ", action: " << action << ", mods: " << mods << std::endl;

    // Update Camera
    auto* pCamera = dynamic_cast<Camera*>(Scene::camera.get());
    if (pCamera) {
        switch (key) {
        case 'r':
        case 'R':
            if (action == GLFW_PRESS)
                Shader::sReloadAll();
            break;
            // Use wasd to move the camera
        case 'w':
        case 'W':
            if (action == GLFW_PRESS)
                pCamera->move_forward_flag = true;
            else if (action == GLFW_RELEASE)
                pCamera->move_forward_flag = false;
            break;
        case 'a':
        case 'A':
            if (action == GLFW_PRESS)
                pCamera->move_left_flag = true;
            else if (action == GLFW_RELEASE)
                pCamera->move_left_flag = false;
            break;
        case 's':
        case 'S':
            if (action == GLFW_PRESS)
                pCamera->move_back_flag = true;
            else if (action == GLFW_RELEASE)
                pCamera->move_back_flag = false;
            break;
        case 'd':
        case 'D':
            if (action == GLFW_PRESS)
                pCamera->move_right_flag = true;
            else if (action == GLFW_RELEASE)
                pCamera->move_right_flag = false;
            break;
        case 'e':
        case 'E':
            if (action == GLFW_PRESS)
                pCamera->move_up_flag = true;
            else if (action == GLFW_RELEASE)
                pCamera->move_up_flag = false;
            break;
        case 'q':
        case 'Q':
            if (action == GLFW_PRESS)
                pCamera->move_down_flag = true;
            else if (action == GLFW_RELEASE)
                pCamera->move_down_flag = false;
            break;
        case GLFW_KEY_UP:
            if (action == GLFW_PRESS)
                pCamera->ProcessMouseMovement(0.f, 1000.f);
            break;
        case GLFW_KEY_DOWN:
            if (action == GLFW_PRESS)
                pCamera->ProcessMouseMovement(0.f, -1000.f);
            break;
        case GLFW_KEY_LEFT:
            if (action == GLFW_PRESS)
                pCamera->ProcessMouseMovement(-1000.f, 0.f);
            break;
        case GLFW_KEY_RIGHT:
            if (action == GLFW_PRESS)
                pCamera->ProcessMouseMovement(1000.f, 0.f);
            break;
        default:
            break;
        }
    }

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            if (Scene::captureCursor) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                Scene::captureCursor = false;
                Scene::firstMouseEnter = true;
            } else {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            break;

        case GLFW_KEY_F1:
            DrawGui::HideGui = !DrawGui::HideGui;
            break;
        }
    }
}

// This function gets called when the mouse moves over the window.
void GlfwCallbacks::MouseCursor(GLFWwindow* window, double xpos, double ypos)
{
    // std::cout << "cursor pos: " << xpos << ", " << ypos << std::endl;
    if (!Scene::captureCursor)
        return;
    if (Scene::firstMouseEnter) {
        Scene::mouse_last_x = xpos;
        Scene::mouse_last_y = ypos;
        Scene::firstMouseEnter = false;
        return;
    }
    // std::cout << "cursor pos: " << x << ", " << y << std::endl;
    auto* pCamera = dynamic_cast<Camera*>(Scene::camera.get());
    if (pCamera) {
        pCamera->ProcessMouseMovement(xpos - Scene::mouse_last_x, -(ypos - Scene::mouse_last_y));
        Scene::mouse_last_x = xpos;
        Scene::mouse_last_y = ypos;
    }
}

// This function gets called when a mouse button is pressed.
void GlfwCallbacks::MouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!Scene::captureCursor) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            Scene::captureCursor = true;
            return;
        }
    }
    // std::cout << "button : "<< button << ", action: " << action << ", mods: " << mods << std::endl;
}

void GlfwCallbacks::MouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* pCamera = dynamic_cast<Camera*>(Scene::camera.get());
    if (pCamera) {
        pCamera->ProcessMouseScroll(yoffset);
    }
}

void GlfwCallbacks::WindowResize(GLFWwindow* glfw_window, int width, int height)
{
    // std::cout << "width : " << width << ", height: " << height << std::endl;

    GlfwWindow::Size[0] = width;
    GlfwWindow::Size[1] = height;
    GlfwWindow::Aspect = float(GlfwWindow::Size[0]) / float(GlfwWindow::Size[1]);
}

void GlfwCallbacks::DragAndDrop(GLFWwindow* window, int count, const char** paths)
{
}
