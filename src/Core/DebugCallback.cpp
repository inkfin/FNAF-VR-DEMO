#include "DebugCallback.h"
#include <iostream>


void RegisterDebugCallback()
{
#if _DEBUG
   if (glDebugMessageCallback)
   {
      std::cout << "Register OpenGL debug callback " << std::endl;
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(openglCallbackFunction, nullptr);
      GLuint unusedIds = 0;
      glDebugMessageControl(GL_DONT_CARE,
         GL_DONT_CARE,
         GL_DONT_CARE,
         0,
         &unusedIds,
         true);
   }
   else
   {
      std::cout << "glDebugMessageCallback not available" << std::endl;
   }
#endif
}


#ifdef WIN32
/* Only run this code on WindowsAPI systems, otherwise use cout */

// C-based callback implementation

/* Reverse of SetConsoleTextAttribute */
WORD GetConsoleTextAttribute(HANDLE hConsoleOutput)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
    return csbi.wAttributes;
}

/* Basic colors */
#define FMT_BLACK   0
#define FMT_BLUE    FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define FMT_RED     FOREGROUND_RED|FOREGROUND_INTENSITY
#define FMT_MAROON  FOREGROUND_RED
#define FMT_GREEN   FOREGROUND_BLUE|FOREGROUND_INTENSITY
/* Combination colors */
#define FMT_MAGENTA FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define FMT_CYAN    FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define FMT_YELLOW  FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define FMT_GOLD  FOREGROUND_RED|FOREGROUND_GREEN
#define FMT_WHITE   FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define FMT_GRAY  FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE

/* Simplified windows API for color console printing */
#define WriteConsoleColorA(hConsoleOutput, lpBuffer, numberOfCharsToWrite, color) \
SetConsoleTextAttribute(hConsoleOutput, color), \
WriteConsoleA(hConsoleOutput, lpBuffer, numberOfCharsToWrite, NULL, NULL)

/* This macro makes it less verbose */
#define WriteConsoleNewlineA() WriteConsoleA(hStdOut, "\r\n", 2, NULL, NULL)

void APIENTRY openglCallbackFunction(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const GLvoid* userParam)
{
    static const char format[][58] = {
    "---------------------OpenGL-callback-start------------\n",
    "Message: ",
    "Type: ",
    "Id: ",
    "Severity: ",
    "---------------------OpenGL-callback-end--------------\n" };
    static const char eTypes[][24] = { "ERROR", "DEPRECATED_BEHAVIOR",
        "UNDEFINED_BEHAVIOR", "PORTABILITY", "PERFORMANCE", "OTHER" };
    static const unsigned short eTypesC[] = { FMT_RED, FMT_GRAY,
        FMT_MAROON, FMT_GOLD, FMT_GREEN, FMT_MAGENTA };
    static const char eSeverities[][16] = { "HIGH", 
        "MEDIUM", "LOW", };
    static const unsigned short eSeveritiesC[] = { FMT_RED, 
        FMT_GOLD, FMT_GREEN };

    unsigned char eTypeIdx, eSeverityIdx;
    HANDLE hStdOut;
    WORD hStdOutAttr;
    char buffer[16];

    eTypeIdx = type - GL_DEBUG_TYPE_ERROR;
    eSeverityIdx = severity - GL_DEBUG_SEVERITY_HIGH;

    /* Do not close this handle with CloseHandle, it's process-wide */
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    /* Save previous colors/attributes to not distrub existing colors */
    hStdOutAttr = GetConsoleTextAttribute(hStdOut);

    WriteConsoleColorA(hStdOut, format[0], sizeof(*format), FMT_YELLOW);

    /* Message: <String> */
    WriteConsoleColorA(hStdOut, format[1], sizeof(*format), FMT_YELLOW);
    WriteConsoleColorA(hStdOut, message, strlen(message), FMT_CYAN);
    WriteConsoleNewlineA();

    /* Type: <Enum> */
    WriteConsoleColorA(hStdOut, format[2], sizeof(*format), FMT_YELLOW);
    WriteConsoleColorA(hStdOut, eTypes[eTypeIdx], 
        sizeof(*eTypes), eTypesC[eTypeIdx]);
    WriteConsoleNewlineA();

    /* Id: <Hexadecimal Int> */
    WriteConsoleColorA(hStdOut, format[3], sizeof(*format), FMT_YELLOW);
    buffer[0] = '0', buffer[1] = 'x';
    _itoa_s(id, buffer + 2, 8, 16);
    WriteConsoleColorA(hStdOut, buffer, strlen(buffer), FMT_CYAN);
    WriteConsoleNewlineA();

    /* Severity: <Enum>*/
    WriteConsoleColorA(hStdOut, format[4], sizeof(*format), FMT_YELLOW);
    if (eSeverityIdx < sizeof(eSeverities)/sizeof(eSeverities[0])) {
        WriteConsoleColorA(hStdOut, eSeverities[eSeverityIdx], 
            sizeof(*eSeverities), eSeveritiesC[eSeverityIdx]);
    }
    else {
        WriteConsoleColorA(hStdOut, "N/A", 3, FMT_MAGENTA);
    }
    WriteConsoleNewlineA();

    WriteConsoleColorA(hStdOut, format[5], sizeof(*format), FMT_YELLOW);

    /* Restore previous colors so other functions can continue printing */
    SetConsoleTextAttribute(hStdOut, hStdOutAttr);

   if(severity == GL_DEBUG_SEVERITY_HIGH || 
      type == GL_DEBUG_TYPE_ERROR)
   {
      static bool previous_break = false;
      if (previous_break == false)
      {
         DebugBreak();  //Check text console for error messages 
         previous_break = true; //This allows execution to continue after the break. Breaks won't happen for subsequent errors
      }
   }

}

#undef WriteConsoleColorA
#undef WriteConsoleNewlineA

#else /* WIN32 */

void APIENTRY openglCallbackFunction(GLenum source,
   GLenum type,
   GLuint id,
   GLenum severity,
   GLsizei length,
   const GLchar* message,
   GLvoid* userParam)
{
   using namespace std;

   cout << "---------------------opengl-callback-start------------" << endl;
   cout << "message: " << message << endl;
   cout << "type: ";
   switch (type) {
   case GL_DEBUG_TYPE_ERROR:
      cout << "ERROR";
      break;
   case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      cout << "DEPRECATED_BEHAVIOR";
      break;
   case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      cout << "UNDEFINED_BEHAVIOR";
      break;
   case GL_DEBUG_TYPE_PORTABILITY:
      cout << "PORTABILITY";
      break;
   case GL_DEBUG_TYPE_PERFORMANCE:
      cout << "PERFORMANCE";
      break;
   case GL_DEBUG_TYPE_OTHER:
      cout << "OTHER";
      break;
   }
   cout << endl;

   cout << "id: " << id << endl;
   cout << "severity: ";
   switch (severity) {
   case GL_DEBUG_SEVERITY_LOW:
      cout << "LOW";
      break;
   case GL_DEBUG_SEVERITY_MEDIUM:
      cout << "MEDIUM";
      break;
   case GL_DEBUG_SEVERITY_HIGH:
      cout << "HIGH";
      break;
   }
   cout << endl;
   cout << "---------------------opengl-callback-end--------------" << endl;
}

#endif /* WIN32 */