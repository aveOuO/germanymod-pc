#pragma once
#include <IL2CPP.hpp>
#include <minhook/include/MinHook.h>
#include <stdexcept>

// 初始化 MinHook
inline void InitializeHooking()
{
    static bool initialized = false;
    if (!initialized)
    {
        if (MH_Initialize() != MH_OK)
        {
            throw std::runtime_error("Failed to initialize MinHook");
        }
        initialized = true;
    }
}

// 简单的 Detour 包装
inline void Detour(void* targetPtr, void* replacementFunc)
{
    InitializeHooking();

    if (MH_CreateHook(targetPtr, replacementFunc, nullptr) != MH_OK)
    {
        throw std::runtime_error("Failed to create hook");
    }

    if (MH_EnableHook(targetPtr) != MH_OK)
    {
        throw std::runtime_error("Failed to enable hook");
    }
}

// 带原始函数保存的 AttachHook
inline void AttachHook(void* targetPtr, void* replacementFunc, void** origFunc)
{
    InitializeHooking();

    if (MH_CreateHook(targetPtr, replacementFunc, origFunc) != MH_OK)
    {
        throw std::runtime_error("Failed to create hook with original");
    }

    if (MH_EnableHook(targetPtr) != MH_OK)
    {
        throw std::runtime_error("Failed to enable hook with original");
    }
}

// IL2CPP 方法重载
inline void AttachHook(const IL2CPP::MethodInfo* method, void* replacementFunc, void** origFunc)
{
    if (method == nullptr)
    {
        throw std::runtime_error("Invalid IL2CPP method");
    }

    // 使用 GetPointer() 方法来访问方法指针
    void* methodPtr = method->GetPointer();
    if (methodPtr == nullptr)
    {
        throw std::runtime_error("IL2CPP method pointer is null");
    }

    AttachHook(methodPtr, replacementFunc, origFunc);
}

// 宏定义保持兼容
#define $Hook(returnType, name, param) \
namespace HookOrigs             \
{                               \
    returnType (*name) param;   \
}                               \
namespace Hooks                 \
{                               \
    returnType name param;      \
}                               \
returnType Hooks::name param

#define $CallOrig(name, ...)   \
HookOrigs::name (__VA_ARGS__)

#define $RegisterHook(name, ptr) \
AttachHook(ptr, (void*)Hooks::name, (void**)&HookOrigs::name)
