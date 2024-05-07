// Copyright 2023 The Dawn & Tint Authors
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "dawn/native/d3d11/DeviceInfoD3D11.h"

#include <utility>

#include "dawn/native/d3d/D3DError.h"
#include "dawn/native/d3d11/PlatformFunctionsD3D11.h"

namespace dawn::native::d3d11 {

ResultOrError<DeviceInfo> GatherDeviceInfo(IDXGIAdapter3* adapter,
                                           const ComPtr<ID3D11Device>& device) {
    DeviceInfo info = {};

    D3D11_FEATURE_DATA_D3D11_OPTIONS2 options2;
    DAWN_TRY(CheckHRESULT(
        device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS2, &options2, sizeof(options2)),
        "D3D11_FEATURE_D3D11_OPTIONS2"));

    info.isUMA = options2.UnifiedMemoryArchitecture;
    info.supportsROV = options2.ROVsSupported;

    info.shaderModel = 50;
    // Profiles are always <stage>s_<minor>_<major> so we build the s_<minor>_major and add
    // it to each of the stage's suffix.
    info.shaderProfiles[SingleShaderStage::Vertex] = L"vs_5_0";
    info.shaderProfiles[SingleShaderStage::Fragment] = L"ps_5_0";
    info.shaderProfiles[SingleShaderStage::Compute] = L"cs_5_0";

    // Runtime of the created texture (D3D11 device) and OpenSharedHandle runtime (Dawn's
    // D3D12 device) must agree on resource sharing capability. For NV12 formats, D3D11
    // requires at-least D3D11_SHARED_RESOURCE_TIER_2 support.
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_shared_resource_tier
    D3D11_FEATURE_DATA_D3D11_OPTIONS5 featureOptions5{};
    DAWN_TRY(CheckHRESULT(device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS5,
                                                      &featureOptions5, sizeof(featureOptions5)),
                          "D3D11_FEATURE_D3D11_OPTIONS5"));
    info.supportsSharedResourceCapabilityTier2 =
        featureOptions5.SharedResourceTier >= D3D11_SHARED_RESOURCE_TIER_2;

    DXGI_ADAPTER_DESC adapterDesc;
    DAWN_TRY(CheckHRESULT(adapter->GetDesc(&adapterDesc), "IDXGIAdapter3::GetDesc"));
    info.dedicatedVideoMemory = adapterDesc.DedicatedVideoMemory;
    info.sharedSystemMemory = adapterDesc.SharedSystemMemory;

    return std::move(info);
}

}  // namespace dawn::native::d3d11
