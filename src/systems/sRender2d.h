#ifndef TS_RENDER_WORLD2D_H
#define TS_RENDER_WORLD2D_H

#include "../core/system.h"
#include <webgpu/webgpu_cpp.h>

class tsRender2d : public tkRenderSystem
{ 
  wgpu::Buffer mPointBuffer{};
  tkDArray<v2> mPointData{};
  
public:
  void SetupBuffers();
  void Render(wgpu::Device& device, wgpu::RenderPassEncoder& pass) override;
};

#endif //TS_RENDER_WORLD2D_H
