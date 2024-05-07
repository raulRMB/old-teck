CMAKE_MINIMUM_REQUIRED(VERSION 3.26)
PROJECT(tech)
set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FILE(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/cpm.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
    EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
)
INCLUDE (${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

IF(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json)
  execute_process(COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)
  message("Copying compile_commands.json")
ENDIF()

FILE(GLOB_RECURSE SRC src/*.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC})

if(EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
  target_link_options(${PROJECT_NAME} PRIVATE
          "-sUSE_WEBGPU=1"
          "-sUSE_GLFW=3"
          "-sALLOW_MEMORY_GROWTH=1"
          "-sASSERTIONS=1"
          "-sSINGLE_FILE=1"
          "--preload-file=${CMAKE_CURRENT_LIST_DIR}/res@res"
          "-sNO_DISABLE_EXCEPTION_CATCHING=1"
          "--shell-file=${CMAKE_CURRENT_LIST_DIR}/src/shell.html")
  target_link_libraries(${PROJECT_NAME} PRIVATE glm tinygltf EnTT)
else()
  set(DAWN_FETCH_DEPENDENCIES ON)
  add_subdirectory("lib/dawn" EXCLUDE_FROM_ALL)
  target_link_libraries(${PROJECT_NAME} PRIVATE webgpu_cpp webgpu_dawn webgpu_glfw glm tinygltf EnTT)
endif()

CPMAddPackage(
  NAME glm
  GITHUB_REPOSITORY g-truc/glm
  GIT_TAG 0.9.9.7
)

CPMAddPackage(
  NAME tiny_gltf
  GITHUB_REPOSITORY syoyo/tinygltf
  GIT_TAG v2.8.20
)

CPMAddPackage(
  NAME EnTT
  GITHUB_REPOSITORY skypjack/entt
  GIT_TAG v3.13.1
)
