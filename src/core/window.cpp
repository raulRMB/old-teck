#include "window.h"
#include "GLFW/glfw3.h"
#include "def.h"

tkWindow::tkWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
  Window = glfwCreateWindow(kWindowWidth, kWindowHeight,  "Tech 0", nullptr, nullptr);
  glfwMakeContextCurrent(Window);
  glfwSwapInterval(0);
}

tkWindow::~tkWindow()
{
  glfwDestroyWindow(Window);
  glfwTerminate();
}

void tkWindow::PollEvents()
{
  glfwPollEvents();
}

bool tkWindow::ShouldClose()
{
  return glfwWindowShouldClose(Window);
}
