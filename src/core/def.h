#ifndef TK_DEFINES_H
#define TK_DEFINES_H

#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include "glm/glm.hpp"
#include "glm/ext/quaternion_common.hpp"

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using v2 = glm::vec2;
using v3 = glm::vec3;
using v4 = glm::vec4;

using m3 = glm::mat3;
using m4 = glm::mat4;

using quat = glm::quat;

template<typename T, size_t S>
using tkArray = std::array<T, S>;

template<typename T>
using tkDArray = std::vector<T>;

using tkString = std::string;

#define TK_SUCCESS 0
#define TK_FAILURE 1
#define TK_ERROR -1

#define TK_EXIT_SUCCESS 0
#define TK_EXIT_FAILURE 1

#define TK_ATTEMPT(x) if (x != TK_EXIT_SUCCESS) { return TK_EXIT_FAILURE; }

const i32 kWindowWidth = 400;
const i32 kWindowHeight = 400;

#endif //TK_DEFINES_H

