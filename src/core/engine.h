#ifndef TK_ENGINE_H
#define TK_ENGINE_H

#include "def.h"
#include "renderer.h"
#include "window.h"
#include "system.h"
#include "../systems/input.h"
#include "../scenes/scene.h"

class tkEngine 
{
    u32 bRunning : 1;
    tkWindow Window;
    tkScene* pCurrentScene{};

    tkDArray<tkScene*> LoadedScenes;

private:
    tkDArray<tsInput*> InputSystems;
    tkDArray<tkUpdateSystem*> UpdateSystems;
    tkDArray<tkSystem*> Systems;

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
    void Update();
    i32 Init();
    i32 MainLoop();
    bool ShouldExit();
};

#endif // TK_ENGINE_H
