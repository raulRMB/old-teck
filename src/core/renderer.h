#ifndef TK_RENDERER_H
#define TK_RENDERER_H

#include "def.h"
#include "system.h"
#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_glfw.h>

struct Vertex
{
  v2 Position;
  v3 Color;
};


struct MVPUniforms
{
  m4 Model;
  m4 View;
  m4 Projection;
};

class tkRenderer
{
  wgpu::Instance wInstance;
  wgpu::Device wDevice;
  wgpu::Surface wSurface;
  wgpu::SwapChain wSwapChain;
  wgpu::RenderPipeline wLinePipeline;
  wgpu::RenderPipeline wMeshPipeline;

  wgpu::DepthStencilState wDepthStencilState;
  wgpu::Texture wDepthTexture;
  wgpu::TextureView wDepthTextureView;

  wgpu::Sampler wSampler;

  tkDArray<wgpu::VertexBufferLayout> wVertexBufferLayouts;
  tkDArray<wgpu::VertexAttribute> wVertexAttributes;

  tkDArray<v2> mPositionData;
  wgpu::Buffer mPositionBuffer;

  MVPUniforms mMvpUniforms;

  wgpu::Buffer mLineVertexBuffer;
  wgpu::Buffer mLineIndexBuffer;

  wgpu::BindGroupLayout wLineBindGroupLayout;
  wgpu::BindGroup wLineBindGroup;
  wgpu::Buffer wMVPUniformsBuffer;

  tkDArray<tkRenderSystem*> mRenderSystems;
public:
  static tkRenderer& Get();
  static wgpu::Device& GetDevice();
  
private:
  tkRenderer();
  ~tkRenderer();

  friend class tkEngine;
  friend class tkRenderSystem;
  friend class tsRender2d;

private:
  void Init(class tkWindow& window);

  static void RegisterRenderSystem(tkRenderSystem* system);

  void GetDevice(void (*callback)(wgpu::Device));

  void InitGraphics();
  
  void SetupSwapChain();

  void SetupDepthStencil();

  void LoadTextures(const tkString& name);
  void SetupSampler();

  void SetupLineVertexBuffer();
  void SetupLineIndexBuffer();

  void SetupMVPUniformsBuffer();
  void SetupLineBindGroup();

//  void SetupLineUniformBuffer();
  void SetupLinePipeline();
  
  void SetupPipelines();

//  void SetupMeshPipeline();

  void Render();
  
  void IterateRenderSystems(wgpu::RenderPassEncoder& pass);

public:
  void AddRect(struct tcRect& rect, v2& position);

};

#endif// TK_RENDERER_H
