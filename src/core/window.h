#ifndef TK_WINDOW_H
#define TK_WINDOW_H

#include <GLFW/glfw3.h>

class tkWindow
{
    GLFWwindow* Window;

private:
    tkWindow();
    ~tkWindow();

    friend class tkEngine;
    friend class tkRenderer;

private:
    void PollEvents();
    bool ShouldClose();

public: 

};

#endif
