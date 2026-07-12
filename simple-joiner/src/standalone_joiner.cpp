#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

namespace StandaloneJoiner
{
    // 内存扫描功能
    static uintptr_t ScanPattern(uintptr_t startAddr, size_t size, const std::string& pattern)
    {
        std::vector<uint8_t> bytes;

        // 转换模式字符串到字节数组
        for (size_t i = 0; i < pattern.size(); i += 3)
        {
            if (pattern[i] == '?')
            {
                bytes.push_back(0x00); // 通配符
            }
            else if (i + 1 < pattern.size())
            {
                std::string byteStr = pattern.substr(i, 2);
                bytes.push_back(static_cast<uint8_t>(strtol(byteStr.c_str(), nullptr, 16)));
            }
        }

        // 扫描内存
        for (size_t i = 0; i < size - bytes.size(); i++)
        {
            bool found = true;
            for (size_t j = 0; j < bytes.size(); j++)
            {
                if (bytes[j] != 0x00) // 不是通配符
                {
                    uint8_t actualByte = *reinterpret_cast<uint8_t*>(startAddr + i + j);
                    if (actualByte != bytes[j])
                    {
                        found = false;
                        break;
                    }
                }
            }

            if (found)
            {
                return startAddr + i;
            }
        }

        return 0;
    }

    // 获取Unity游戏模块基址
    static uintptr_t GetUnityModuleBase()
    {
        HMODULE hModules[1024];
        DWORD cbNeeded;

        if (EnumProcessModules(GetCurrentProcess(), hModules, sizeof(hModules), &cbNeeded))
        {
            for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
            {
                char szModuleName[MAX_PATH];
                GetModuleFileNameExA(GetCurrentProcess(), hModules[i], szModuleName, sizeof(szModuleName));

                std::string moduleName(szModuleName);
                if (moduleName.find("Unity") != std::string::npos ||
                    moduleName.find("Game") != std::string::npos ||
                    moduleName.find("Assembly") != std::string::npos)
                {
                    return reinterpret_cast<uintptr_t>(hModules[i]);
                }
            }
        }

        return 0;
    }

    // 简单的控制台界面
    static void ConsoleInterface()
    {
        system("cls");
        std::cout << "=== Simple Room Joiner ===" << std::endl;
        std::cout << "1. Join Room by Name" << std::endl;
        std::cout << "2. Exit" << std::endl;
        std::cout << "==========================" << std::endl;

        int choice;
        std::cout << "Select option: ";
        std::cin >> choice;

        if (choice == 1)
        {
            std::string roomName;
            std::cout << "Enter room name: ";
            std::cin.ignore();
            std::getline(std::cin, roomName);

            std::cout << "Attempting to join room: " << roomName << std::endl;

            // 这里需要实现具体的JoinRoom逻辑
            // 可能需要找到游戏函数地址并调用

            std::cout << "Note: Full implementation requires game-specific function addresses." << std::endl;
            std::cout << "This is a template for the room joiner." << std::endl;
        }
    }

    // DLL入口点
    extern "C" __declspec(dllexport) void Initialize()
    {
        // 分配控制台
        AllocConsole();
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);

        std::cout << "Simple Room Joiner initialized!" << std::endl;
        std::cout << "Unity Module Base: 0x" << std::hex << GetUnityModuleBase() << std::endl;

        // 显示控制台界面
        ConsoleInterface();
    }

    extern "C" __declspec(dllexport) void JoinRoom(const char* roomName)
    {
        if (roomName == nullptr || strlen(roomName) == 0)
        {
            std::cout << "Invalid room name!" << std::endl;
            return;
        }

        std::cout << "Joining room: " << roomName << std::endl;

        // TODO: 实现具体的JoinRoom逻辑
        // 1. 找到PhotonNetwork.JoinRoom函数地址
        // 2. 准备参数
        // 3. 调用函数
    }
}

// 简化的DLL入口
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, [](LPVOID) -> DWORD
        {
            Sleep(1000); // 等待游戏初始化
            StandaloneJoiner::Initialize();
            return 0;
        }, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        FreeConsole();
        break;
    }
    return TRUE;
}