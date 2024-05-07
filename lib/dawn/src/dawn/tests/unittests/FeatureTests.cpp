// Copyright 2019 The Dawn & Tint Authors
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

#include <vector>

#include "dawn/native/Features.h"
#include "dawn/native/Instance.h"
#include "dawn/native/Toggles.h"
#include "dawn/native/null/DeviceNull.h"
#include "gtest/gtest.h"

namespace dawn {
namespace {

class FeatureTests : public testing::Test {
  public:
    FeatureTests()
        : testing::Test(),
          mInstanceBase(native::APICreateInstance(nullptr)),
          mPhysicalDevice(mInstanceBase.Get()),
          mUnsafePhysicalDevice(mInstanceBase.Get()),
          mAdapterBase(&mPhysicalDevice,
                       native::FeatureLevel::Core,
                       native::TogglesState(native::ToggleStage::Adapter),
                       wgpu::PowerPreference::Undefined),
          mUnsafeAdapterBase(&mUnsafePhysicalDevice,
                             native::FeatureLevel::Core,
                             native::TogglesState(native::ToggleStage::Adapter)
                                 .SetForTesting(native::Toggle::AllowUnsafeAPIs, true, true),
                             wgpu::PowerPreference::Undefined) {}

    std::vector<wgpu::FeatureName> GetAllFeatureNames() {
        std::vector<wgpu::FeatureName> allFeatureNames(kTotalFeaturesCount);
        for (size_t i = 0; i < kTotalFeaturesCount; ++i) {
            allFeatureNames[i] = native::ToAPI(static_cast<native::Feature>(i));
        }
        return allFeatureNames;
    }

    static constexpr size_t kTotalFeaturesCount =
        static_cast<size_t>(native::kEnumCount<native::Feature>);

  protected:
    // By default DisallowUnsafeAPIs is enabled in this instance.
    Ref<dawn::native::InstanceBase> mInstanceBase;
    native::null::PhysicalDevice mPhysicalDevice;
    native::null::PhysicalDevice mUnsafePhysicalDevice;
    // The adapter that inherit toggles states from the instance, also have DisallowUnsafeAPIs
    // enabled.
    native::AdapterBase mAdapterBase;
    native::AdapterBase mUnsafeAdapterBase;
};

// Test the creation of a device will fail if the requested feature is not supported on the
// Adapter.
TEST_F(FeatureTests, AdapterWithRequiredFeatureDisabled) {
    const std::vector<wgpu::FeatureName> kAllFeatureNames = GetAllFeatureNames();
    for (size_t i = 0; i < kTotalFeaturesCount; ++i) {
        native::Feature notSupportedFeature = static_cast<native::Feature>(i);

        std::vector<wgpu::FeatureName> featureNamesWithoutOne = kAllFeatureNames;
        featureNamesWithoutOne.erase(featureNamesWithoutOne.begin() + i);

        // Test that the default adapter validates features as expected.
        {
            mPhysicalDevice.SetSupportedFeaturesForTesting(featureNamesWithoutOne);
            native::Adapter adapterWithoutFeature(&mAdapterBase);

            wgpu::DeviceDescriptor deviceDescriptor;
            wgpu::FeatureName featureName = native::ToAPI(notSupportedFeature);
            deviceDescriptor.requiredFeatures = &featureName;
            deviceDescriptor.requiredFeatureCount = 1;

            WGPUDevice deviceWithFeature = adapterWithoutFeature.CreateDevice(
                reinterpret_cast<const WGPUDeviceDescriptor*>(&deviceDescriptor));
            ASSERT_EQ(nullptr, deviceWithFeature);
        }

        // Test that an adapter with AllowUnsafeApis enabled validates features as expected.
        {
            mUnsafePhysicalDevice.SetSupportedFeaturesForTesting(featureNamesWithoutOne);
            native::Adapter adapterWithoutFeature(&mUnsafeAdapterBase);

            wgpu::DeviceDescriptor deviceDescriptor;
            wgpu::FeatureName featureName = ToAPI(notSupportedFeature);
            deviceDescriptor.requiredFeatures = &featureName;
            deviceDescriptor.requiredFeatureCount = 1;

            WGPUDevice deviceWithFeature = adapterWithoutFeature.CreateDevice(
                reinterpret_cast<const WGPUDeviceDescriptor*>(&deviceDescriptor));
            ASSERT_EQ(nullptr, deviceWithFeature);
        }
    }
}

// Test creating device requiring a supported feature can succeed (with DisallowUnsafeApis adapter
// toggle disabled for experimental features), and Device.GetEnabledFeatures() can return the names
// of the enabled features correctly.
TEST_F(FeatureTests, RequireAndGetEnabledFeatures) {
    native::Adapter adapter(&mAdapterBase);
    native::Adapter unsafeAdapterAllow(&mUnsafeAdapterBase);

    for (size_t i = 0; i < kTotalFeaturesCount; ++i) {
        native::Feature feature = static_cast<native::Feature>(i);
        wgpu::FeatureName featureName = ToAPI(feature);

        wgpu::DeviceDescriptor deviceDescriptor;
        deviceDescriptor.requiredFeatures = &featureName;
        deviceDescriptor.requiredFeatureCount = 1;

        // Test with the default adapter.
        {
            native::DeviceBase* deviceBase = native::FromAPI(adapter.CreateDevice(
                reinterpret_cast<const WGPUDeviceDescriptor*>(&deviceDescriptor)));

            // Creating a device with experimental feature requires the adapter enables
            // AllowUnsafeAPIs or disables DisallowUnsafeApis, otherwise expect validation error.
            if (native::kFeatureNameAndInfoList[feature].featureState ==
                native::FeatureInfo::FeatureState::Experimental) {
                ASSERT_EQ(nullptr, deviceBase) << i;
            } else {
                // Requiring stable features should succeed.
                ASSERT_NE(nullptr, deviceBase);
                ASSERT_EQ(1u, deviceBase->APIEnumerateFeatures(nullptr));
                wgpu::FeatureName enabledFeature;
                deviceBase->APIEnumerateFeatures(&enabledFeature);
                EXPECT_EQ(enabledFeature, featureName);
                deviceBase->APIRelease();
            }
        }

        // Test with the adapter with AllowUnsafeApis toggles enabled, creating device should always
        // succeed.
        {
            native::DeviceBase* deviceBase = native::FromAPI(unsafeAdapterAllow.CreateDevice(
                reinterpret_cast<const WGPUDeviceDescriptor*>(&deviceDescriptor)));

            ASSERT_NE(nullptr, deviceBase);
            ASSERT_EQ(1u, deviceBase->APIEnumerateFeatures(nullptr));
            wgpu::FeatureName enabledFeature;
            deviceBase->APIEnumerateFeatures(&enabledFeature);
            EXPECT_EQ(enabledFeature, featureName);
            deviceBase->APIRelease();
        }
    }
}

}  // anonymous namespace
}  // namespace dawn
