#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

// 游戏函数指针结构体（基于原项目的PointerFunctions.hpp）
struct GameFunctions {
    // PhotonNetwork相关
    void* (*GetCurrentRoom)() = nullptr;
    void* (*GetLocalPlayer)() = nullptr;
    bool (*IsMasterClient)() = nullptr;
    void (*SetMasterClient)(void* photonPlayer) = nullptr;
    void (*Destroy)(void* object) = nullptr;

    // 玩家相关
    void* (*GetPlayerID)() = nullptr;
    bool (*IsInMatch)() = nullptr;

    // 房间信息
    void* (*GetRoomHashtable)(void* room) = nullptr;

    // 标记是否已初始化
    bool initialized = false;
} static g_functions;

// 游戏模块信息
struct GameModuleInfo {
    uintptr_t baseAddress;
    size_t size;
    std::string name;
} static g_moduleInfo;

namespace EnhancedJoiner {

    // 获取Unity游戏模块信息
    bool GetGameModuleInfo() {
        HMODULE hModules[1024];
        DWORD cbNeeded;

        if (EnumProcessModules(GetCurrentProcess(), hModules, sizeof(hModules), &cbNeeded)) {
            for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                char szModuleName[MAX_PATH];
                GetModuleFileNameExA(GetCurrentProcess(), hModules[i], szModuleName, sizeof(szModuleName));

                std::string moduleName(szModuleName);

                // 查找Unity相关模块
                if (moduleName.find("Unity") != std::string::npos ||
                    moduleName.find("Game") != std::string::npos ||
                    moduleName.find("Assembly") != std::string::npos ||
                    moduleName.find("mono") != std::string::npos) {

                    MODULEINFO modInfo;
                    if (GetModuleInformation(GetCurrentProcess(), hModules[i], &modInfo, sizeof(modInfo))) {
                        g_moduleInfo.baseAddress = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
                        g_moduleInfo.size = modInfo.SizeOfImage;
                        g_moduleInfo.name = moduleName;

                        std::cout << "[增强版] 找到游戏模块: " << moduleName << std::endl;
                        std::cout << "[增强版] 基址: 0x" << std::hex << g_moduleInfo.baseAddress << std::endl;
                        std::cout << "[增强版] 大小: 0x" << g_moduleInfo.size << std::dec << std::endl;

                        return true;
                    }
                }
            }
        }

        return false;
    }

    // 内存扫描功能
    uintptr_t ScanPattern(uintptr_t startAddr, size_t size, const std::string& pattern) {
        std::vector<uint8_t> bytes;
        std::vector<bool> wildcard;

        // 转换模式字符串
        for (size_t i = 0; i < pattern.size(); i += 3) {
            if (i + 2 < pattern.size()) {
                std::string byteStr = pattern.substr(i, 2);
                if (byteStr == "??") {
                    bytes.push_back(0x00);
                    wildcard.push_back(true);
                } else {
                    bytes.push_back(static_cast<uint8_t>(strtol(byteStr.c_str(), nullptr, 16)));
                    wildcard.push_back(false);
                }
            }
        }

        // 扫描内存
        for (size_t i = 0; i < size - bytes.size(); i++) {
            bool found = true;
            for (size_t j = 0; j < bytes.size(); j++) {
                if (!wildcard[j]) {
                    uint8_t actualByte = *reinterpret_cast<uint8_t*>(startAddr + i + j);
                    if (actualByte != bytes[j]) {
                        found = false;
                        break;
                    }
                }
            }

            if (found) {
                return startAddr + i;
            }
        }

        return 0;
    }

    // 初始化游戏函数指针（基于原项目的IL2CPP类名）
    void InitializeGameFunctions() {
        if (g_functions.initialized) return;

        std::cout << "[增强版] 正在初始化游戏函数..." << std::endl;

        // 这里需要根据具体游戏版本设置偏移
        // 以下是一些常见的模式（需要根据实际游戏调整）

        // PhotonNetwork::GetCurrentRoom 模式
        // 这只是一个示例，实际需要游戏分析工具找到正确的模式
        std::string getCurrentRoomPattern = "48 83 EC 28 48 8B 05 ?? ?? ?? ??"; // 示例模式

        // 在实际使用中，你需要：
        // 1. 使用调试工具（如x64dbg, IDA）找到函数地址
        // 2. 记录函数特征码
        // 3. 在这里添加扫描逻辑

        std::cout << "[增强版] 函数指针初始化需要具体游戏偏移" << std::endl;
        std::cout << "[增强版] 当前使用IL2CPP动态查找方式" << std::endl;

        g_functions.initialized = true;
    }

    // 核心的JoinRoom功能（基于你的git变更）
    void JoinRoomByName(const std::string& roomName) {
        if (roomName.empty()) {
            std::cout << "[警告] 房间名为空" << std::endl;
            return;
        }

        std::cout << "[增强版] 尝试加入房间: " << roomName << std::endl;

        // 检查游戏模块是否已加载
        if (g_moduleInfo.baseAddress == 0) {
            std::cout << "[错误] 游戏模块未找到" << std::endl;
            return;
        }

        // 这里需要实现具体的JoinRoom逻辑
        // 有两种方式：

        // 方式1: 使用IL2CPP接口（如果游戏支持）
        /*
        auto photonNetworkClass = IL2CPP::ClassMapping::GetClass("PhotonNetwork");
        if (photonNetworkClass) {
            IL2CPP::MethodInfo* joinMethod = photonNetworkClass->GetMethod("JoinRoom", 1);
            if (joinMethod) {
                auto args = IL2CPP::Array<IL2CPP::Object*>::Create(...);
                args->Set(0, IL2CPP::String::Create(roomName));
                MethodBase::Invoke(joinMethod, nullptr, args);
            }
        }
        */

        // 方式2: 直接调用游戏函数（需要找到地址）
        /*
        if (g_functions.JoinRoomFunction) {
            // 调用游戏函数
            g_functions.JoinRoomFunction(roomName.c_str());
        }
        */

        std::cout << "[增强版] JoinRoom功能需要游戏特定的实现" << std::endl;
        std::cout << "[增强版] 请根据游戏版本添加函数地址" << std::endl;
    }

    // 显示游戏信息
    void ShowGameInfo() {
        std::cout << "\n=== 游戏信息 ===" << std::endl;
        std::cout << "模块: " << g_moduleInfo.name << std::endl;
        std::cout << "基址: 0x" << std::hex << g_moduleInfo.baseAddress << std::dec << std::endl;
        std::cout << "大小: " << g_moduleInfo.size << " bytes" << std::endl;
        std::cout << "函数已初始化: " << (g_functions.initialized ? "是" : "否") << std::endl;
        std::cout << "==================\n" << std::endl;
    }

    // 控制台界面
    void ConsoleInterface() {
        while (true) {
            std::cout << "\n=== 增强版房间加入器 ===" << std::endl;
            std::cout << "1. 加入房间" << std::endl;
            std::cout << "2. 显示游戏信息" << std::endl;
            std::cout << "3. 测试连接" << std::endl;
            std::cout << "4. 退出" << std::endl;
            std::cout << "=====================" << std::endl;

            int choice;
            std::cout << "选择: ";
            std::cin >> choice;

            switch (choice) {
            case 1: {
                std::string roomName;
                std::cout << "输入房间名: ";
                std::cin.ignore();
                std::getline(std::cin, roomName);
                JoinRoomByName(roomName);
                break;
            }
            case 2:
                ShowGameInfo();
                break;
            case 3:
                std::cout << "[测试] 游戏模块基址: 0x" << std::hex << g_moduleInfo.baseAddress << std::dec << std::endl;
                break;
            case 4:
                std::cout << "退出..." << std::endl;
                return;
            default:
                std::cout << "无效选择" << std::endl;
            }
        }
    }

    // DLL初始化
    void Initialize() {
        AllocConsole();
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);

        std::cout << "=== 增强版房间加入器 ===" << std::endl;
        std::cout << "基于原项目 " << PROJECT_NAME << " 的git变更" << std::endl;
        std::cout << "编译时间: " << __DATE__ << " " << __TIME__ << std::endl;

        // 获取游戏模块信息
        if (GetGameModuleInfo()) {
            std::cout << "[成功] 游戏模块已识别" << std::endl;
        } else {
            std::cout << "[警告] 未找到游戏模块" << std::endl;
        }

        // 初始化函数指针
        InitializeGameFunctions();

        // 显示界面
        ConsoleInterface();
    }
}

// DLL导出函数
extern "C" __declspec(dllexport) void Initialize() {
    EnhancedJoiner::Initialize();
}

extern "C" __declspec(dllexport) void JoinRoom(const char* roomName) {
    if (roomName) {
        EnhancedJoiner::JoinRoomByName(std::string(roomName));
    }
}

extern "C" __declspec(dllexport) void ShowInfo() {
    EnhancedJoiner::ShowGameInfo();
}

// DLL入口点
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
            Sleep(1000); // 等待游戏初始化
            EnhancedJoiner::Initialize();
            return 0;
        }, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        FreeConsole();
        break;
    }
    return TRUE;
}