#include "sRender2d.h"
#include "../components/transform2d.h"
#include "../core/renderer.h"
#include "../components/shape2d.h"

void tsRender2d::Render(wgpu::Device& device, wgpu::RenderPassEncoder& pass)
{
  const u32 restartIndex = 0xFFFFFFFF;
  mPointData.clear();
  mPointData.reserve(100000 * 6);
  for(auto entity : GetView<tcTransform2d, tcRect>())
  {
    tcTransform2d& t = GetComponent<tcTransform2d>(entity);
    tcRect& r = GetComponent<tcRect>(entity);

    mPointData.push_back(v2(-r.Dimensions.x, -r.Dimensions.y) + t.Position);
    mPointData.push_back(v2( r.Dimensions.x, -r.Dimensions.y) + t.Position);
    mPointData.push_back(v2( r.Dimensions.x,  r.Dimensions.y) + t.Position);
    mPointData.push_back(v2(-r.Dimensions.x,  r.Dimensions.y) + t.Position);
    mPointData.push_back(v2(-r.Dimensions.x, -r.Dimensions.y) + t.Position);

    mPointData.push_back((v2&)restartIndex);
  }
  device.GetQueue().WriteBuffer(mPointBuffer, 0, mPointData.data(), mPointData.size() * sizeof(v2));
  pass.SetVertexBuffer(0, mPointBuffer, 0, mPointData.size() * sizeof(v2));
  pass.Draw(mPointData.size());
}

void tsRender2d::SetupBuffers()
{
  wgpu::BufferDescriptor desc;
  desc.size = sizeof(v2) * 5 * 100000;
  desc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
  mPointBuffer = tkRenderer::GetDevice().CreateBuffer(&desc);
}
