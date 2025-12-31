cmake_minimum_required (VERSION 3.16)
project (cv_test_prj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(_USE_MATH_DEFINES)

set(THREADS_PREFER_PTHREAD_FLAG ON)
option(BUILD_GUI_TEST_PROGRAM "Build GUI test program" ON)
find_package(Threads REQUIRED)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  include_directories(
    /usr/local/include
    /opt/local/include
    /opt/homebrew/include
  )

  
  find_library(OPENGL_LIBRARY OpenGL REQUIRED)
  find_library(GLEW_LIBRARY GLEW REQUIRED)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
  find_library(IOKIT_LIBRARY IOKit REQUIRED)
  find_library(COREVID_LIBRARY CoreVideo REQUIRED)

  set(EXTRA_LIBRARIES
      ${OPENGL_LIBRARY}
      ${GLEW_LIBRARY}
      ${COCOA_LIBRARY}
      ${IOKIT_LIBRARY}
      ${COREVID_LIBRARY}
      glfw
      )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include  ${CMAKE_CURRENT_SOURCE_DIR}/third-party/stb ${CMAKE_CURRENT_SOURCE_DIR}/third-party/voronoi/src)
include_directories(${OpenCV_INCLUDE_DIRS})

# imGui
if(BUILD_GUI_TEST_PROGRAM)

  find_package(GLEW CONFIG REQUIRED)
  find_package(glfw3 CONFIG REQUIRED)
  find_package(OpenGL REQUIRED)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  find_package(glad CONFIG REQUIRED)
  find_package(glm REQUIRED)
  endif()

  add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)

  include_directories(${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${glfw3_INCLUDE_DIR} third-party/imgui)

  set(imgui_SRCS 
    third-party/imgui/imgui.cpp
    third-party/imgui/imgui_demo.cpp
    third-party/imgui/imgui_draw.cpp
    third-party/imgui/imgui_widgets.cpp
    third-party/imgui/imgui_tables.cpp
    third-party/imgui/backends/imgui_impl_glfw.cpp
    third-party/imgui/backends/imgui_impl_opengl3.cpp
  )

  set(shadertoy_SRCS src/shadertoy.cpp)
  add_executable(shadertoy ${shadertoy_SRCS} ${imgui_SRCS})
  target_include_directories(shadertoy PUBLIC include third-party/imgui)
  set_target_properties(shadertoy PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries(shadertoy PRIVATE GLEW::GLEW glfw glm::glm ${OPENGL_gl_LIBRARY})
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  target_link_libraries(shadertoy ${EXTRA_LIBRARIES})
  endif()

endif(BUILD_GUI_TEST_PROGRAM)
