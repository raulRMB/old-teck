#include "window.h"

tkWindow::tkWindow()
{
  glfwInit();
  Window = glfwCreateWindow(400, 400,  "Tech 0", nullptr, nullptr);
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
