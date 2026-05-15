cmake_minimum_required(VERSION 3.10)
project(OWL VERSION 1.0.0 LANGUAGES C CXX)   # Fixed: no "SIMULATOR"

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(SOURCES
  engine/src/main.cpp
  engine/src/correcao.cpp
  engine/src/textrendering.cpp
  engine/src/Objects.cpp
  engine/src/buildtriangles.cpp
  engine/src/Camera.cpp          # novo
  engine/src/InputHandler.cpp    # novo
  external/glad.c
  external/stb_image.cpp
  external/tiny_obj_loader.cpp
)

# Output directory
if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${PROJECT_SOURCE_DIR}/bin/Debug")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin/Release")
elseif(UNIX)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/Linux")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(EXECUTABLE_NAME main)

# Check source files exist
foreach(source_file IN LISTS SOURCES)
  if(NOT EXISTS ${PROJECT_SOURCE_DIR}/${source_file})
    message(FATAL_ERROR "Missing source: ${PROJECT_SOURCE_DIR}/${source_file}")
  endif()
endforeach()

add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Include directories
target_include_directories(${EXECUTABLE_NAME} BEFORE PRIVATE
  ${PROJECT_SOURCE_DIR}/engine/include
  ${PROJECT_SOURCE_DIR}/external
  ${PROJECT_SOURCE_DIR}/include
)

# Define ASSETS_DIR macro
target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
  ASSETS_DIR="${PROJECT_SOURCE_DIR}/assets"
)

# Windows linking
if(WIN32)
  if(MINGW)
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} "-v"
      ERROR_VARIABLE COMPILER_VERSION_OUTPUT
      RESULT_VARIABLE COMPILER_VERSION_RESULT
    )
    if(COMPILER_VERSION_RESULT EQUAL 0)
      if(COMPILER_VERSION_OUTPUT MATCHES "ucrt64")
        set(LIBGLFW ${PROJECT_SOURCE_DIR}/lib-ucrt-64/libglfw3.a)
      elseif(COMPILER_VERSION_OUTPUT MATCHES "mingw64")
        set(LIBGLFW ${PROJECT_SOURCE_DIR}/lib-mingw-64/libglfw3.a)
      else()
        set(LIBGLFW ${PROJECT_SOURCE_DIR}/lib-mingw-32/libglfw3.a)
      endif()
    else()
      message(FATAL_ERROR "Failed to get MinGW compiler version.")
    endif()
  elseif(MSVC)
    set(LIBGLFW ${PROJECT_SOURCE_DIR}/lib-vc2022/glfw3.lib)
  else()
    message(FATAL_ERROR "Only MINGW or MSVC on Windows.")
  endif()
  target_link_libraries(${EXECUTABLE_NAME} ${LIBGLFW} gdi32 opengl32)
endif()

# Linux linking
if(UNIX)
  target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wno-unused-function)
  find_package(OpenGL REQUIRED)
  find_package(X11 REQUIRED)
  find_library(MATH_LIBRARY m)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  target_link_libraries(${EXECUTABLE_NAME}
    ${CMAKE_DL_LIBS}
    ${MATH_LIBRARY}
    ${PROJECT_SOURCE_DIR}/lib-linux/libglfw3.a
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENGL_LIBRARIES}
    ${X11_LIBRARIES}
    ${X11_Xrandr_LIB}
    ${X11_Xcursor_LIB}
    ${X11_Xinerama_LIB}
  )
  add_custom_target(run
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EXECUTABLE_NAME}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    DEPENDS ${EXECUTABLE_NAME}
    USES_TERMINAL
  )
endif()
