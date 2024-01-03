#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include "opengl.hpp"
#include <Windows.h>
#include "math.h"

class window
{
  private:
    GLFWwindow* mWindow;
    bool mExists;
    ivec2 mPosition;
    const char* mTitle;
    bool mFullscreen;
    ivec2 mDimensions;
    long mWindowStyle;
  public:
    ivec2 GetDimensions() const {return mDimensions;}
    window(int w, int h, const char* window_name, bool visible);
    ~window();
    GLFWwindow* GetHandle() const {return mWindow;}
    bool update();
    void destroy();
    bool GetKey(int key);
};

#endif // __WINDOW_HPP__
