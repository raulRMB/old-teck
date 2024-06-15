#include "engine.h"
#include "def.h"
#include "entt/entt.hpp"
#include "renderer.h"
#include "../systems/sPhysics2d.h"

tkEngine& tkEngine::Get()
{
  static tkEngine instance;
  return instance;
}

tkEngine::tkEngine() : 
  bRunning(false),
  Window(tkWindow())
{
    
}

tkEngine::~tkEngine()
{
  for(tkScene* pScene : LoadedScenes)
  {
    pScene->CleanUpScene();
    delete pScene;
  }
  LoadedScenes.clear();
}

i32 tkEngine::Create()
{
  tkEngine::Get();
  return TK_SUCCESS;
}

i32 tkEngine::Init()
{
  bRunning = true;

  UpdateSystems.push_back(new tsPhysics2d());

  tkScene* pScene = new tkScene();
  pScene->BeginPlay();
  LoadedScenes.push_back(pScene);
  tkRenderer::Get().Init(Window);
            
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
    Update();
    tkRenderer::Get().Render();
  }
  return TK_EXIT_SUCCESS;
}

bool tkEngine::ShouldExit()
{   
  if (Window.ShouldClose())
  {
    return true;
  }

  for(tsInput* system : InputSystems)
  {
    bRunning |= system->HandleInput();
  }
  return !bRunning;
}

void tkEngine::Update()
{
  for(tkUpdateSystem* system : UpdateSystems)
  {
    system->Update();
  }
}
