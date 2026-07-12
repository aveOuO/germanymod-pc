cmake_minimum_required(VERSION 3.15)
project(SimpleRoomJoiner CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 简化的配置 - 不依赖MSVC特定的设置
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    # 对于GCC/MinGW
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif()

# 定义源文件
set(SOURCES
    src/simple_joiner.cpp
)

# 包含目录
include_directories(
    ../dependencies/ImGui/src
    ../dependencies/Logger/src
    ../dependencies/TulipHook/src
    ../dependencies/proplamatic-il2cpp/include
    ../header-dependencies
)

# 创建DLL
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME}
    imgui
    TulipHook
    logger
)

# 如果是GCC，添加必要的库
if(NOT MSVC)
    target_link_libraries(${PROJECT_NAME}
        -static-libgcc
        -static-libstdc++
    )
endif()