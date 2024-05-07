#include "engine.h"
#include "def.h"
#include "sys/input.h"

tkEngine& tkEngine::Get()
{
    static tkEngine e;
    return e;
}

tkEngine::tkEngine() : 
    bRunning(false),
    Window(tkWindow()),
    Renderer(tkRenderer())
{
    
}

tkEngine::~tkEngine()
{
    
}

i32 tkEngine::Create()
{
    tkEngine::Get();
    return TK_SUCCESS;
}

i32 tkEngine::Init()
{
    bRunning = true;
    
    return TK_SUCCESS;
}

i32 tkEngine::Run()
{
    TK_ATTEMPT(tkEngine::Create());
    
    TK_ATTEMPT(tkEngine::Get().Init());
    TK_ATTEMPT(tkEngine::Get().MainLoop());

    return TK_SUCCESS;
}

void tkEngine::PollEvents()
{
    Window.PollEvents();
}

i32 tkEngine::MainLoop()
{    
    while(!ShouldExit())
    {
        PollEvents();
    }
    return TK_EXIT_SUCCESS;
}

bool tkEngine::ShouldExit()
{   
    if (Window.ShouldClose())
    {
        return true;    
    }
    
    for(tsInput& system : InputSystems)
    {
        bRunning |= system.HandleInput();
    }
    return !bRunning;
}
