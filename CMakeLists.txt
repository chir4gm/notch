# CMakeList.txt : CMake project for notch, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("notch")

# Add source to this project's executable.
add_executable (notch "notch.cpp"  "util.h" "common.h" "util.cpp")
add_subdirectory("deps/glfw")
file(COPY "assets" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "shaders" DESTINATION ${CMAKE_BINARY_DIR})
include_directories("deps/include")
add_subdirectory("deps/glew/build/cmake")
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})
target_link_libraries(notch glew_s)
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})
add_compile_definitions(GLEW_STATIC)
add_dependencies(notch glew_s)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
target_link_libraries(notch glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(notch OpenGL::GL)
target_include_directories(notch PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/deps/glew/include")
target_include_directories(notch PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include")
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET notch PROPERTY CXX_STANDARD 20)
endif()
include(CMakePrintHelpers)
cmake_print_variables(GLEW_INCLUDE_DIRS)
cmake_print_variables(GLFW_INCLUDE_DIRS)
# TODO: Add tests and install targets if needed.
