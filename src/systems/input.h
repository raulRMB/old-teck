#ifndef TS_INPUT_H
#define TS_INPUT_H

#include "../core/system.h"

class tsInput : public tkSystem
{
  friend class tkEngine;
  bool HandleInput();  
  void KeyCallback(struct GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif //TS_INPUT_H
