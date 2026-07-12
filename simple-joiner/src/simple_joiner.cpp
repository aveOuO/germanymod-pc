#include <windows.h>
#include <string>
#include <vector>
#include <stack>
#include <functional>
#include <imgui.h>
#include <Logger.hpp>
#include "proplamatic-il2cpp/il2cpp-api.h"
#include "proplamatic-il2cpp/il2cpp-class.h"
#include "proplamatic-il2cpp/il2cpp-object.h"
#include "proplamatic-il2cpp/il2cpp-string.h"
#include "proplamatic-il2cpp/il2cpp-method.h"

namespace SimpleJoiner
{
    static std::string currentRoomName = "";
    static bool showWindow = true;
    static char roomInputBuffer[256] = "";

    // 游戏线程执行栈
    static std::stack<std::function<void()>> gameThreadExecutionStack;
    static std::vector<std::function<void()>> updateList;

    // 在游戏线程上执行回调
    void ExecuteOnGameThread(std::function<void()> callback)
    {
        gameThreadExecutionStack.push(callback);
    }

    // 注册更新回调
    void RegisterUpdate(std::function<void()> callback)
    {
        updateList.push_back(callback);
    }

    // 处理待执行的任务（需要在游戏主线程中调用）
    void ProcessGameThreadTasks()
    {
        // 执行注册的更新函数
        for (auto& func : updateList)
        {
            func();
        }

        // 执行栈中的任务
        while (!gameThreadExecutionStack.empty())
        {
            auto func = gameThreadExecutionStack.top();
            func();
            gameThreadExecutionStack.pop();
        }
    }

    // 核心的JoinRoom功能（基于你的实现）
    void JoinRoomByName(const std::string& roomName)
    {
        if (roomName.empty())
        {
            Logger::Log(Logger::LogTag::Warning, "JoinRoomByName: room name is empty");
            return;
        }

        auto photonNetworkClass = IL2CPP::ClassMapping::GetClass("PhotonNetwork");
        if (photonNetworkClass == nullptr)
        {
            Logger::Log(Logger::LogTag::Error, "JoinRoomByName: failed to resolve PhotonNetwork class");
            return;
        }

        IL2CPP::MethodInfo* joinMethod = nullptr;
        for (size_t paramCount : {1, 2, 3})
        {
            joinMethod = photonNetworkClass->GetMethod("JoinRoom", paramCount);
            if (joinMethod != nullptr)
            {
                break;
            }
        }

        if (joinMethod == nullptr)
        {
            Logger::Log(Logger::LogTag::Error, "JoinRoomByName: failed to resolve PhotonNetwork::JoinRoom");
            return;
        }

        auto args = IL2CPP::Array<IL2CPP::Object*>::Create(IL2CPP::DefaultTypeClass::Object, joinMethod->GetParametersCount());
        args->Set(0, IL2CPP::String::Create(roomName));

        if (joinMethod->GetParametersCount() >= 2)
        {
            args->Set(1, IL2CPP::Object::BoxValue(IL2CPP::DefaultTypeClass::Boolean, false));
        }

        if (joinMethod->GetParametersCount() >= 3)
        {
            args->Set(2, IL2CPP::Object::BoxValue(IL2CPP::DefaultTypeClass::Boolean, false));
        }

        IL2CPP::Object* result = MethodBase::Invoke(reinterpret_cast<IL2CPP::Object*>(joinMethod), nullptr, args);
        bool success = result != nullptr && result->Unbox<bool>();
        Logger::Log(Logger::LogTag::Info, "JoinRoomByName: requested room '%s' (result=%s)", roomName.c_str(), success ? "true" : "false");
    }

    // 简单的ImGui界面
    void RenderInterface()
    {
        if (!showWindow) return;

        ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Simple Room Joiner", &showWindow))
        {
            ImGui::Text("Join Room by Name");

            // 房间名输入框
            if (ImGui::InputText("Room Name", roomInputBuffer, sizeof(roomInputBuffer)))
            {
                currentRoomName = roomInputBuffer;
            }

            // Join按钮
            if (ImGui::Button("Join Room"))
            {
                if (!currentRoomName.empty())
                {
                    // 在游戏线程执行
                    ExecuteOnGameThread([]()
                    {
                        JoinRoomByName(currentRoomName);
                    });
                }
            }

            // 显示当前房间名
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Current: %s", currentRoomName.c_str());

            ImGui::End();
        }
    }

    // DLL入口点
    extern "C" __declspec(dllexport) void Initialize()
    {
        Logger::Log(Logger::LogTag::Info, "Simple Room Joiner initialized");
    }

    extern "C" __declspec(dllexport) void RenderUI()
    {
        RenderInterface();
        ProcessGameThreadTasks();  // 每帧处理游戏线程任务
    }

    // 已废弃：ExecuteOnGameThread 已内置实现
    extern "C" __declspec(dllexport) void SetExecuteOnGameThread(void (*callback)(void(*)()))
    {
        // 这个需要由游戏注入器设置 - 已废弃
        Logger::Log(Logger::LogTag::Warning, "SetExecuteOnGameThread is deprecated - ExecuteOnGameThread is now built-in");
    }

    // 导出ProcessGameThreadTasks，允许手动调用
    extern "C" __declspec(dllexport) void Update()
    {
        ProcessGameThreadTasks();
    }
}