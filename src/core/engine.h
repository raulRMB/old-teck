#ifndef TK_ENGINE_H
#define TK_ENGINE_H

#include "def.h"
#include "renderer.h"
#include "window.h"
#include "system.h"
#include "sys/input.h"

class tkEngine 
{
    u32 bRunning : 1;
    tkWindow Window;
    tkRenderer Renderer;

private:
    tkDArray<tsInput> InputSystems;
    tkDArray<tkSystem> Systems;
    
public:
    static tkEngine& Get();

private:
    tkEngine();
    ~tkEngine();
  
private:

public:
    static i32 Create();
    static i32 Run();
    void PollEvents();
    i32 Init();
    i32 MainLoop();
    bool ShouldExit();
};

#endif // TK_ENGINE_H
