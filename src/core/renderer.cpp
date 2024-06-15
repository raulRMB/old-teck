#include "renderer.h"
#include "def.h"
#include "system.h"
#include "window.h"
#include "logger.h"
#include <iostream>
#include <stdint.h>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include "../components/shape2d.h"
#include "../systems/sRender2d.h"
#include "reader.h"


tkRenderer& tkRenderer::Get()
{
    static tkRenderer instance;
    return instance;
}

tkRenderer::tkRenderer()
{

}

tkRenderer::~tkRenderer()
{

}

void tkRenderer::GetDevice(void (*callback)(wgpu::Device)) {
    wInstance.RequestAdapter(
        nullptr,
        // TODO(https://bugs.chromium.org/p/dawn/issues/detail?id=1892): Use
        // wgpu::RequestAdapterStatus, wgpu::Adapter, and wgpu::Device.
        [](WGPURequestAdapterStatus status, WGPUAdapter cAdapter,
                      const char* message, void* userdata) {
                if (status != WGPURequestAdapterStatus_Success) {
                        tkLogError("Failed to acquire adapter");
                        exit(0);
                    }
                wgpu::Adapter adapter = wgpu::Adapter::Acquire(cAdapter);
                wgpu::DeviceDescriptor descriptor{};
                adapter.RequestDevice(
                    nullptr,
            [](WGPURequestDeviceStatus status, WGPUDevice cDevice,
                                  const char* message, void* userdata) {
                    wgpu::Device device = wgpu::Device::Acquire(cDevice);
                    device.SetUncapturedErrorCallback(
                                [](WGPUErrorType type, const char* message, void* userdata) {
                                                std::cout << "Error: " << type << " - message: " << message;
                                            },
                        nullptr);
                                reinterpret_cast<void (*)(wgpu::Device)>(userdata)(device);
                            },
            userdata);
            },
        reinterpret_cast<void*>(callback)
    );
}

void tkRenderer::Init(tkWindow& window)
{
    wInstance = wgpu::CreateInstance();

    for(u32 i = 0; i < 4; i++)
    {
      for (u32 j = 0; j < 1000000; j+=4)
      {
        vertices[i].push_back(Vertex{v2(-0.5f, -0.5f) * (f32)i / 1000.f, v3(1.0f, 1.0f, 1.0f)});
        vertices[i].push_back(Vertex{v2(0.5f, -0.5f) * (f32)i / 1000.f, v3(1.0f, 1.0f, 1.0f)});
        vertices[i].push_back(Vertex{v2(0.5f, 0.5f) * (f32)i / 1000.f, v3(1.0f, 1.0f, 1.0f)});
        vertices[i].push_back(Vertex{v2(-0.5f, 0.5f) * (f32)i / 1000.f, v3(1.0f, 1.0f, 1.0f)});

        indices[i].push_back(j);
        indices[i].push_back(j + 1);
        indices[i].push_back(j + 2);
        indices[i].push_back(j + 3);
        indices[i].push_back(j);
        indices[i].push_back(0xFFFF);
      }
    }

    GetDevice([](wgpu::Device dev) {
        Get().wDevice = dev;
    });
    wSurface = wgpu::glfw::CreateSurfaceForWindow(wInstance, window.Window);
    InitGraphics();
}

void tkRenderer::InitGraphics()
{
    SetupSwapChain();
    SetupLineVertexBuffer();
    SetupLineIndexBuffer();
    SetupMVPUniformsBuffer();
    SetupLineBindGroup();
    SetupDepthStencil();
    SetupSampler();

//    SetupLineUniformBuffer();

    SetupPipelines();

    tsRender2d* rw2d = new tsRender2d();
    rw2d->SetupBuffers();
    RegisterRenderSystem(rw2d);
}

void tkRenderer::SetupSwapChain()
{
    wgpu::SwapChainDescriptor scDesc{
        .usage = wgpu::TextureUsage::RenderAttachment,
        .format = wgpu::TextureFormat::BGRA8Unorm,
        .width = kWindowWidth,
        .height = kWindowHeight,
        .presentMode = wgpu::PresentMode::Fifo};
    wSwapChain = wDevice.CreateSwapChain(wSurface, &scDesc);
}

void tkRenderer::SetupDepthStencil()
{
        wDepthStencilState = wgpu::DepthStencilState{
            .format = wgpu::TextureFormat::Depth16Unorm,
            .depthWriteEnabled = true,
            .depthCompare = wgpu::CompareFunction::LessEqual,
            .stencilReadMask = 0,
            .stencilWriteMask = 0,
        };

        wgpu::TextureDescriptor depthTextureDescriptor{
                .usage = wgpu::TextureUsage::RenderAttachment,
                .dimension = wgpu::TextureDimension::e2D,
                .size = {kWindowWidth, kWindowHeight, 1},
                .format = wgpu::TextureFormat::Depth16Unorm,
                .mipLevelCount = 1,
                .sampleCount = 1,
                .viewFormatCount = 1,
                .viewFormats = &wDepthStencilState.format,
            };

        wgpu::TextureViewDescriptor depthTextureViewDescriptor{
                .format = wDepthStencilState.format,
                .dimension = wgpu::TextureViewDimension::e2D,
                .baseMipLevel = 0,
                .mipLevelCount = 1,
                .baseArrayLayer = 0,
                .arrayLayerCount = 1,
                .aspect = wgpu::TextureAspect::DepthOnly,
            };

        wDepthTexture = wDevice.CreateTexture(&depthTextureDescriptor);
        wDepthTextureView = wDepthTexture.CreateView(&depthTextureViewDescriptor);
}

void tkRenderer::LoadTextures(const tkString& name)
{

}

void tkRenderer::SetupSampler()
{
    wgpu::SamplerDescriptor samplerDescriptor{
        .addressModeU = wgpu::AddressMode::ClampToEdge,
        .addressModeV = wgpu::AddressMode::ClampToEdge,
        .addressModeW = wgpu::AddressMode::ClampToEdge,
        .magFilter = wgpu::FilterMode::Linear,
        .minFilter = wgpu::FilterMode::Linear,
        .mipmapFilter = wgpu::MipmapFilterMode::Linear,
        .lodMinClamp = 0.0f,
        .lodMaxClamp = 1.0f,
        .compare = wgpu::CompareFunction::Undefined,
        .maxAnisotropy = 1,
    };

    wSampler = wDevice.CreateSampler(&samplerDescriptor);
}

void tkRenderer::SetupPipelines()
{
//    SetupMeshPipeline();
    SetupLinePipeline();
}

void tkRenderer::SetupLinePipeline()
{
    wgpu::ShaderModuleWGSLDescriptor wgslDesc{};
    wgslDesc.code = tkReader::ReadTextFile("shaders/mesh.wgsl");

    wgpu::ShaderModuleDescriptor shaderModuleDesc{
        .nextInChain = &wgslDesc
    };

    shaderModuleDesc.label = "Line";
    wgpu::ShaderModule shaderModule = wDevice.CreateShaderModule(&shaderModuleDesc);

    wgpu::ColorTargetState colorTargetState{
        .format = wgpu::TextureFormat::BGRA8Unorm
    };

    wgpu::FragmentState fragmentState{.module = shaderModule,
        .targetCount = 1,
        .targets = &colorTargetState
    };

    wgpu::PrimitiveState primitiveState;
    primitiveState.topology = wgpu::PrimitiveTopology::LineStrip;
    primitiveState.cullMode = wgpu::CullMode::None;
    primitiveState.stripIndexFormat = wgpu::IndexFormat::Uint16;

    wgpu::PipelineLayoutDescriptor pipelineLayoutDesc{
        .bindGroupLayoutCount = 1,
        .bindGroupLayouts = &wLineBindGroupLayout
    };

    wgpu::RenderPipelineDescriptor desc{
        .label = "Line",
        .layout = wDevice.CreatePipelineLayout(&pipelineLayoutDesc),
        .vertex = {.module = shaderModule},
        .primitive = primitiveState,
        .depthStencil = &wDepthStencilState,
        .fragment = &fragmentState,
    };

    desc.vertex.bufferCount = static_cast<u32>(wVertexBufferLayouts.size());
    desc.vertex.buffers = wVertexBufferLayouts.data();

    wLinePipeline = wDevice.CreateRenderPipeline(&desc);
}

//void tkRenderer::SetupLineUniformBuffer()
//{
//    mPositionData.emplace_back(v2( 1.0,  1.0));
//    mPositionData.emplace_back(v2( 1.0, -1.0));
//    mPositionData.emplace_back(v2(-1.0, -1.0));
//    mPositionData.emplace_back(v2(-1.0,  1.0));
//    mPositionData.emplace_back(v2( 1.0,  1.0));
//
//    wgpu::BufferDescriptor bufferDesc;
//    bufferDesc.label = "Position Buffer";
//    bufferDesc.size = mPositionData.size() * sizeof(v2);
//    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
//    bufferDesc.mappedAtCreation = false;
//    mPositionBuffer = wDevice.CreateBuffer(&bufferDesc);
//    wDevice.GetQueue().WriteBuffer(mPositionBuffer, 0, mPositionData.data(), bufferDesc.size);
//}

//void tkRenderer::SetupMeshPipeline()
//{
//    wgpu::ShaderModuleWGSLDescriptor wgslDesc{};
//    wgslDesc.code = tkReader::ReadTextFile("shaders/mesh.wgsl");
//
//    wgpu::ShaderModuleDescriptor shaderModuleDesc{
//        .nextInChain = &wgslDesc
//    };
//
//    shaderModuleDesc.label = "Mesh Shader Module";
//    wgpu::ShaderModule shaderModule = wDevice.CreateShaderModule(&shaderModuleDesc);
//
//    wgpu::ColorTargetState colorTargetState{
//        .format = wgpu::TextureFormat::BGRA8Unorm
//    };
//
//    wgpu::FragmentState fragmentState{.module = shaderModule,
//        .targetCount = 1,
//        .targets = &colorTargetState
//    };
//
//    wgpu::RenderPipelineDescriptor desc{
//        .label = "Mesh",
//        .vertex = {.module = shaderModule},
//        .depthStencil = &wDepthStencilState,
//        .fragment = &fragmentState,
//    };
//
//    desc.vertex.bufferCount = static_cast<u32>(wVertexBufferLayouts.size());
//    desc.vertex.buffers = wVertexBufferLayouts.data();
//
//    wMeshPipeline = wDevice.CreateRenderPipeline(&desc);
//}

void tkRenderer::SetupLineVertexBuffer()
{
    wVertexAttributes.resize(2, {});

    wVertexAttributes[0].shaderLocation = 0;
    wVertexAttributes[0].format = wgpu::VertexFormat::Float32x2;
    wVertexAttributes[0].offset = offsetof(Vertex, Position);

    wVertexAttributes[1].shaderLocation = 1;
    wVertexAttributes[1].format = wgpu::VertexFormat::Float32x3;
    wVertexAttributes[1].offset = offsetof(Vertex, Color);

    wVertexBufferLayouts.resize(1, {});

    wVertexBufferLayouts[0].attributeCount = 2;
    wVertexBufferLayouts[0].arrayStride = sizeof(Vertex);
    wVertexBufferLayouts[0].attributes = wVertexAttributes.data();
    wVertexBufferLayouts[0].stepMode = wgpu::VertexStepMode::Vertex;

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.label = "Line Vertex Buffer";
    bufferDesc.size = vertices.size() * sizeof(Vertex);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
    bufferDesc.mappedAtCreation = false;
    mLineVertexBuffer = wDevice.CreateBuffer(&bufferDesc);
    for (u32 i = 0; i < 4; i++)
    {
        wDevice.GetQueue().WriteBuffer(mLineVertexBuffer, i * bufferDesc.size, vertices.data(), bufferDesc.size);
    }
}

void tkRenderer::AddRect(tcRect& rect, v2& pos)
{
    v2 topLeft = pos + v2(-rect.Dimensions.x, -rect.Dimensions.y);
    v2 topRight = pos + v2(+rect.Dimensions.x, -rect.Dimensions.y);
    v2 bottomRight = pos + v2(+rect.Dimensions.x, +rect.Dimensions.y);
    v2 bottomLeft = pos + v2(-rect.Dimensions.x, +rect.Dimensions.y);

    mPositionData.push_back(topLeft);
    mPositionData.push_back(topRight);
    mPositionData.push_back(bottomRight);
    mPositionData.push_back(bottomLeft);
    mPositionData.push_back(topLeft);
}

void tkRenderer::Render()
{
    wgpu::RenderPassColorAttachment attachment{
        .view = wSwapChain.GetCurrentTextureView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = wgpu::Color(0.059, 0.059, 0.059, 1.0)};

    wgpu::RenderPassDepthStencilAttachment depthStencilAttachment{};
    depthStencilAttachment.view = wDepthTextureView;
    depthStencilAttachment.depthClearValue = 1.0f;
    depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
    depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Store;
    depthStencilAttachment.depthReadOnly = false;
    depthStencilAttachment.stencilClearValue = 0;
    depthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Undefined;
    depthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Undefined;
    depthStencilAttachment.stencilReadOnly = true;

    wgpu::RenderPassDescriptor renderpassDesc{.colorAttachmentCount = 1,
        .colorAttachments = &attachment,
        .depthStencilAttachment = &depthStencilAttachment};

    wgpu::CommandEncoder encoder = wDevice.CreateCommandEncoder();
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpassDesc);

    pass.SetPipeline(wLinePipeline);
    pass.SetBindGroup(0, wLineBindGroup);

  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  f32 time = std::chrono::duration<f32, std::chrono::seconds::period>(currentTime - startTime).count();


    mMvpUniforms.Model = glm::rotate(m4(1.f), time * glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    mMvpUniforms.View = glm::lookAt(v3(2.f, 2.f, 200.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
    mMvpUniforms.Projection = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 100000.f);

    wDevice.GetQueue().WriteBuffer(wMVPUniformsBuffer, 0, &mMvpUniforms, sizeof(MVPUniforms));
//    wDevice.GetQueue().WriteBuffer(mLineVertexBuffer, 0, vertices.data(), vertices.size() * sizeof(Vertex));

    pass.SetVertexBuffer(0, mLineVertexBuffer, 0);
    pass.SetBindGroup(0, wLineBindGroup);
    pass.SetIndexBuffer(mLineIndexBuffer, wgpu::IndexFormat::Uint16, 0, indices.size() * sizeof(u16));
    pass.DrawIndexed(indices.size(), 1, 0, 0, 0);

    pass.End();
    wgpu::CommandBuffer commands = encoder.Finish();
    wDevice.GetQueue().Submit(1, &commands);

    wSwapChain.Present();
    wInstance.ProcessEvents();
}

void tkRenderer::IterateRenderSystems(wgpu::RenderPassEncoder& pass)
{
    for(tkRenderSystem* sys : mRenderSystems)
    {
        sys->Render(wDevice, pass);
    }
}

void tkRenderer::RegisterRenderSystem(tkRenderSystem *system)
{
    Get().mRenderSystems.push_back(system);
}

wgpu::Device &tkRenderer::GetDevice()
{
  return Get().wDevice;
}

void tkRenderer::SetupLineIndexBuffer()
{
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.label = "Line Index Buffer";
    bufferDesc.size = indices.size() * sizeof(u16);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
    bufferDesc.mappedAtCreation = false;
    mLineIndexBuffer = wDevice.CreateBuffer(&bufferDesc);
    wDevice.GetQueue().WriteBuffer(mLineIndexBuffer, 0, indices.data(), bufferDesc.size);
}

void tkRenderer::SetupLineBindGroup()
{
    wgpu::BindGroupLayoutEntry bindGroupLayoutEntry{
        .binding = 0,
        .visibility = wgpu::ShaderStage::Vertex,
        .buffer = {.type = wgpu::BufferBindingType::Uniform, .hasDynamicOffset = false, .minBindingSize = sizeof(MVPUniforms)}
    };

    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{
        .entryCount = 1,
        .entries = &bindGroupLayoutEntry
    };

    wLineBindGroupLayout = wDevice.CreateBindGroupLayout(&bindGroupLayoutDesc);

    wgpu::BindGroupEntry bindGroupEntry{
        .binding = 0,
        .buffer = wMVPUniformsBuffer,
        .offset = 0,
        .size = sizeof(MVPUniforms),
    };

    wgpu::BindGroupDescriptor bindGroupDesc{
        .layout = wLineBindGroupLayout,
        .entryCount = 1,
        .entries = &bindGroupEntry
    };

    wLineBindGroup = wDevice.CreateBindGroup(&bindGroupDesc);
}

void tkRenderer::SetupMVPUniformsBuffer()
{
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.label = "MVP Uniforms Buffer";
    bufferDesc.size = sizeof(MVPUniforms);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = false;
    wMVPUniformsBuffer = wDevice.CreateBuffer(&bufferDesc);
}
