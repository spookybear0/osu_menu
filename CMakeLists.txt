cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
project(osu_menu CXX)

file(GLOB SOURCES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set(CMAKE_SIZEOF_VOID_P 4)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target i386-pc-windows-msvc")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target i386-pc-windows-msvc")
  add_definitions("--target=i386-pc-windows-msvc")
endif()

add_library(cocos2d INTERFACE)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
  include
  include/cocos2dx
  include/cocos2dx/include
  include/cocos2dx/kazmath/include
  include/cocos2dx/platform
  include/cocos2dx/platform/win32
  include/cocos2dx/platform/third_party/win32
  include/cocos2dx/platform/third_party/win32/OGLES
  include/extensions
  include/minhook
  include/minhook/include
  include/gd.h
  include/gd.h/include
)


target_link_libraries(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include/cocos2dx/libcocos2d.lib
  ${CMAKE_CURRENT_LIST_DIR}/include/extensions/libExtensions.lib
  ${CMAKE_CURRENT_LIST_DIR}/include/minhook/minhook.x32.lib
)