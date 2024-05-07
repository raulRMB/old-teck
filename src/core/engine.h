#ifndef TK_ENGINE_H
#define TK_ENGINE_H

#include "def.h"

class tkEngine 
{
    u32 bRunning : 1;

public:
    static tkEngine& Get();

private:
    tkEngine();
    ~tkEngine();
  
private:

public:
    static i32 Run();
    i32 MainLoop();
    bool ShouldExit();
};

#endif // TK_ENGINE_H
