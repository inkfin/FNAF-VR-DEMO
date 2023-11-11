#pragma once

#include <windows.h>
#include <GL/glew.h>


/*
OpenGL debug message callbacks are a more convenient and performant alternative to glGetError(...).
When you register a debug message callback the openglCallbackFunction() below will be automatically called
by OpenGL when errors or warnings are detected.

More details about this feature are at: https://blog.nobel-joergensen.com/2013/02/17/debugging-opengl-part-2-using-gldebugmessagecallback/
*/

//Follow the following steps to add the debug callback to your application:

/* 1. Create an OpenGL context with debugging enabled in debug builds of the application. 
(Check Preprocessor options - _DEBUG should be defined in debug builds)
Include the following before the window is created.

#ifdef _DEBUG
       glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
*/

/* 2. Register the debug callback function when you are running a debug build.
You can do this by calling RegisterCallback(); after glewInit(). Be sure to include this header file.

*/

/* 3. You will see a lot of output on the text console when it is working. Not only errors are reported
by default. You will also see a lot of performance warnings and other messages (specifics depend on driver).
*/
void RegisterDebugCallback();

void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);