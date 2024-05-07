#include "engine.h"
#include "def.h"
#include <string>
#include <iostream>

tkEngine& tkEngine::Get()
{
    static tkEngine e;
    return e;      
}

tkEngine::tkEngine()
{
    
}

tkEngine::~tkEngine()
{
    
}

i32 tkEngine::Run()
{
    tkEngine& engine = tkEngine::Get();

    TK_ATTEMPT(engine.MainLoop());    

    return TK_EXIT_SUCCESS;
}

i32 tkEngine::MainLoop()
{
    while(!ShouldExit())
    {
        std::string str;
        std::cin >> str;

        if (str == "exit")
        {
                   
        }
    }
    return TK_EXIT_SUCCESS;
}

bool tkEngine::ShouldExit()
{
    return !bRunning;
}
