// Copyright 2021 The Dawn & Tint Authors
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

#include "src/dawn/node/binding/GPUBuffer.h"

#include <cassert>
#include <memory>
#include <utility>

#include "src/dawn/node/binding/Converter.h"
#include "src/dawn/node/binding/Errors.h"
#include "src/dawn/node/utils/Debug.h"

namespace wgpu::binding {

////////////////////////////////////////////////////////////////////////////////
// wgpu::bindings::GPUBuffer
////////////////////////////////////////////////////////////////////////////////
GPUBuffer::GPUBuffer(wgpu::Buffer buffer,
                     wgpu::BufferDescriptor desc,
                     wgpu::Device device,
                     std::shared_ptr<AsyncRunner> async)
    : buffer_(std::move(buffer)),
      desc_(desc),
      device_(std::move(device)),
      async_(std::move(async)),
      mapped_(desc.mappedAtCreation),
      label_(desc.label ? desc.label : "") {}

interop::Promise<void> GPUBuffer::mapAsync(Napi::Env env,
                                           interop::GPUMapModeFlags modeIn,
                                           interop::GPUSize64 offset,
                                           std::optional<interop::GPUSize64> size) {
    // Convert the mapMode and reject with the TypeError if it happens.
    Converter conv(env, device_);
    wgpu::MapMode mode;
    if (!conv(mode, modeIn)) {
        return {env, interop::kUnusedPromise};
    }

    // Early rejection when there is already a mapping pending.
    if (pending_map_) {
        auto promise = interop::Promise<void>(env, PROMISE_INFO);
        promise.Reject(Errors::OperationError(env));
        return promise;
    }

    pending_map_.emplace(env, PROMISE_INFO);
    uint64_t rangeSize = size.has_value() ? size.value().value : (desc_.size - offset);

    struct Context {
        Napi::Env env;
        GPUBuffer* self;
        AsyncTask task;
        interop::Promise<void> promise;
    };
    auto ctx = new Context{env, this, AsyncTask(env, async_), *pending_map_};

    buffer_.MapAsync(
        mode, offset, rangeSize,
        [](WGPUBufferMapAsyncStatus status, void* userdata) {
            auto c = std::unique_ptr<Context>(static_cast<Context*>(userdata));

            // The promise may already have been resolved with an AbortError if there was an early
            // destroy() or early unmap().
            if (c->promise.GetState() != interop::PromiseState::Pending) {
                assert(c->promise.GetState() == interop::PromiseState::Rejected);
                return;
            }

            switch (status) {
                case WGPUBufferMapAsyncStatus_Force32:
                    UNREACHABLE("WGPUBufferMapAsyncStatus_Force32");
                    break;
                case WGPUBufferMapAsyncStatus_Success:
                    c->promise.Resolve();
                    c->self->mapped_ = true;
                    break;
                case WGPUBufferMapAsyncStatus_DestroyedBeforeCallback:
                case WGPUBufferMapAsyncStatus_DeviceLost:
                case WGPUBufferMapAsyncStatus_InstanceDropped:
                case WGPUBufferMapAsyncStatus_MappingAlreadyPending:
                case WGPUBufferMapAsyncStatus_OffsetOutOfRange:
                case WGPUBufferMapAsyncStatus_SizeOutOfRange:
                case WGPUBufferMapAsyncStatus_Unknown:
                case WGPUBufferMapAsyncStatus_UnmappedBeforeCallback:
                case WGPUBufferMapAsyncStatus_ValidationError:
                    c->self->async_->Reject(c->env, c->promise, Errors::OperationError(c->env));
                    break;
            }

            // This captured promise is the currently pending mapping, reset it so we can start new
            // mappings.
            assert(*c->self->pending_map_ == c->promise);
            c->self->pending_map_.reset();
        },
        ctx);

    return pending_map_.value();
}

interop::ArrayBuffer GPUBuffer::getMappedRange(Napi::Env env,
                                               interop::GPUSize64 offset,
                                               std::optional<interop::GPUSize64> size) {
    uint64_t s = size.has_value() ? size.value().value : (desc_.size - offset);

    uint64_t start = offset;
    uint64_t end = offset + s;
    for (auto& mapping : mappings_) {
        if (mapping.Intersects(start, end)) {
            Errors::OperationError(env).ThrowAsJavaScriptException();
            return {};
        }
    }

    auto* ptr = (desc_.usage & wgpu::BufferUsage::MapWrite)
                    ? buffer_.GetMappedRange(offset, s)
                    : const_cast<void*>(buffer_.GetConstMappedRange(offset, s));
    if (!ptr) {
        Errors::OperationError(env).ThrowAsJavaScriptException();
        return {};
    }
    auto array_buffer = Napi::ArrayBuffer::New(env, ptr, s);
    // TODO(crbug.com/dawn/1135): Ownership here is the wrong way around.
    mappings_.emplace_back(Mapping{start, end, Napi::Persistent(array_buffer)});
    return array_buffer;
}

void GPUBuffer::unmap(Napi::Env env) {
    DetachMappings(env);
    buffer_.Unmap();
}

void GPUBuffer::destroy(Napi::Env env) {
    DetachMappings(env);
    buffer_.Destroy();
}

interop::GPUSize64Out GPUBuffer::getSize(Napi::Env) {
    return buffer_.GetSize();
}

interop::GPUBufferMapState GPUBuffer::getMapState(Napi::Env env) {
    if (mapped_) {
        return interop::GPUBufferMapState::kMapped;
    }

    if (pending_map_) {
        assert(pending_map_->GetState() == interop::PromiseState::Pending);
        return interop::GPUBufferMapState::kPending;
    }

    return interop::GPUBufferMapState::kUnmapped;
}

interop::GPUFlagsConstant GPUBuffer::getUsage(Napi::Env env) {
    interop::GPUBufferUsageFlags result;

    Converter conv(env);
    if (!conv(result, buffer_.GetUsage())) {
        Napi::Error::New(env, "Couldn't convert usage to a JavaScript value.")
            .ThrowAsJavaScriptException();
        return 0u;  // Doesn't get used.
    }

    return result;
}

void GPUBuffer::DetachMappings(Napi::Env env) {
    mapped_ = false;

    if (pending_map_) {
        pending_map_->Reject(Errors::AbortError(env));
        pending_map_.reset();
    }

    for (auto& mapping : mappings_) {
        mapping.buffer.Value().Detach();
    }
    mappings_.clear();
}

std::string GPUBuffer::getLabel(Napi::Env) {
    return label_;
}

void GPUBuffer::setLabel(Napi::Env, std::string value) {
    buffer_.SetLabel(value.c_str());
    label_ = value;
}

}  // namespace wgpu::binding
