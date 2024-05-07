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

#ifndef SRC_DAWN_FUZZERS_DAWNLPMSERIALIZER_H_
#define SRC_DAWN_FUZZERS_DAWNLPMSERIALIZER_H_

#include "dawn/fuzzers/lpmfuzz/dawn_lpm_autogen.pb.h"
#include "dawn/wire/ChunkedCommandSerializer.h"
#include "dawn/wire/WireCmd_autogen.h"
#include "dawn/wire/WireResult.h"

namespace dawn::wire {

class DawnLPMObjectIdProvider : public ObjectIdProvider {
  private:

    // Implementation of the ObjectIdProvider interface
    {% for type in by_category["object"] %}
        WireResult GetId({{as_cType(type.name)}} object, ObjectId* out) const final {
            *out = reinterpret_cast<uintptr_t>(object);
            return WireResult::Success;
        }
        WireResult GetOptionalId({{as_cType(type.name)}} object, ObjectId* out) const final {
            *out = reinterpret_cast<uintptr_t>(object);
            return WireResult::Success;
        }
    {% endfor %}

};

WireResult SerializedData(const fuzzing::Program& program,
                       dawn::wire::ChunkedCommandSerializer serializer);

}  // namespace dawn::wire

#endif  // SRC_DAWN_FUZZERS_DAWNLPMSERIALIZER_H_
