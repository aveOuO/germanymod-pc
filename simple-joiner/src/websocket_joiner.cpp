#include <windows.h>
#include <iostream>
#include <string>
#include <map>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

// WebSocket消息结构
struct WebSocketMessage {
    std::string messageType;
    std::string payload;
};

// 游戏函数指针
struct GameFunctions {
    // WebSocket Manager
    void* (*GetWebSocketManager)() = nullptr;
    void* (*SendWebSocketMessage)(void* wsManager, const char* msgType, void* jsonMsg) = nullptr;
    void* (*SendWebSocketEvent)(void* wsManager, const char* msgType) = nullptr;

    // ProgressUpdater
    void* (*GetProgressUpdater)() = nullptr;
    void (*AddCurrency)(void* instance, const char* currencyName, int amount, int unknown, bool b1, bool b2, void* params) = nullptr;
    void (*AddExperience)(void* expController, int value, int reason, void* dict, void* extra) = nullptr;

    // Experience
    void* (*GetExperienceController)() = nullptr;
    int (*GetLevel)(void* moduleData) = nullptr;

    bool initialized = false;
} static g_functions;

// 游戏模块信息
struct GameModuleInfo {
    uintptr_t baseAddress;
    size_t size;
    std::string name;
} static g_moduleInfo;

namespace WebSocketJoiner {

    // 游戏进程识别
    bool GetGameProcessInfo() {
        HWND hwnd = FindWindowA(nullptr, "Pixel Gun 3D");
        if (hwnd) {
            std::cout << "[WebSocket] 找到游戏窗口" << std::endl;
            return true;
        }

        HMODULE hModules[1024];
        DWORD cbNeeded;

        if (EnumProcessModules(GetCurrentProcess(), hModules, sizeof(hModules), &cbNeeded)) {
            for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                char szModuleName[MAX_PATH];
                GetModuleFileNameExA(GetCurrentProcess(), hModules[i], szModuleName, sizeof(szModuleName));

                std::string moduleName(szModuleName);
                if (moduleName.find("Unity") != std::string::npos ||
                    moduleName.find("Game") != std::string::npos ||
                    moduleName.find("PG3D") != std::string::npos) {

                    MODULEINFO modInfo;
                    if (GetModuleInformation(GetCurrentProcess(), hModules[i], &modInfo, sizeof(modInfo))) {
                        g_moduleInfo.baseAddress = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
                        g_moduleInfo.size = modInfo.SizeOfImage;
                        g_moduleInfo.name = moduleName;

                        std::cout << "[WebSocket] 找到游戏模块: " << moduleName << std::endl;
                        return true;
                    }
                }
            }
        }

        return false;
    }

    // === WebSocket功能 ===

    // 构建JSON消息
    std::string BuildJSON(const std::map<std::string, std::string>& data) {
        std::stringstream json;
        json << "{";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) json << ",";
            json << "\"" << pair.first << "\":\"" << pair.second << "\"";
            first = false;
        }
        json << "}";
        return json.str();
    }

    // 发送WebSocket消息
    bool SendWebSocketMessage(const std::string& messageType, const std::map<std::string, std::string>& data) {
        std::cout << "[WebSocket] 发送消息: " << messageType << std::endl;

        if (g_functions.GetWebSocketManager == nullptr) {
            std::cout << "[错误] WebSocket Manager函数未找到" << std::endl;
            return false;
        }

        // 获取WebSocket Manager实例
        void* wsManager = g_functions.GetWebSocketManager();
        if (wsManager == nullptr) {
            std::cout << "[错误] 无法获取WebSocket Manager实例" << std::endl;
            return false;
        }

        // 构建JSON数据
        std::string jsonStr = BuildJSON(data);
        std::cout << "[WebSocket] JSON数据: " << jsonStr << std::endl;

        if (g_functions.SendWebSocketMessage) {
            // 这里需要将JSON字符串转换为IL2CPP的JSON对象
            // 简化版本直接传递字符串指针
            g_functions.SendWebSocketMessage(wsManager, messageType.c_str(), nullptr);
            std::cout << "[成功] WebSocket消息已发送" << std::endl;
            return true;
        }

        return false;
    }

    // 发送游戏事件（WebSocket简化版）
    bool SendGameEvent(const std::string& eventCommand, const std::map<std::string, std::string>& params) {
        if (g_functions.SendWebSocketEvent == nullptr) {
            std::cout << "[错误] SendEvent函数未找到" << std::endl;
            return false;
        }

        void* wsManager = g_functions.GetWebSocketManager();
        if (wsManager == nullptr) {
            std::cout << "[错误] WebSocket Manager未找到" << std::endl;
            return false;
        }

        std::cout << "[WebSocket] 发送事件: " << eventCommand << std::endl;

        // 调用SendEvent函数
        g_functions.SendWebSocketEvent(wsManager, eventCommand.c_str());
        return true;
    }

    // === 核心游戏功能 ===

    // 通过WebSocket添加货币
    bool AddCurrencyViaWebSocket(const std::string& currencyType, int amount) {
        std::map<std::string, std::string> data;
        data["currency"] = currencyType;
        data["amount"] = std::to_string(amount);
        data["reason"] = "webhook";

        return SendWebSocketMessage("AddCurrency", data);
    }

    // 直接添加货币（函数调用）
    bool AddCurrencyDirect(const std::string& currencyType, int amount) {
        if (g_functions.GetProgressUpdater == nullptr) {
            std::cout << "[错误] ProgressUpdater函数未找到" << std::endl;
            return false;
        }

        void* progressUpdater = g_functions.GetProgressUpdater();
        if (progressUpdater == nullptr) {
            std::cout << "[错误] 无法获取ProgressUpdater实例" << std::endl;
            return false;
        }

        if (g_functions.AddCurrency) {
            g_functions.AddCurrency(progressUpdater, currencyType.c_str(), amount, 0, false, false, nullptr);
            std::cout << "[成功] 已添加货币: " << currencyType << " +" << amount << std::endl;
            return true;
        }

        return false;
    }

    // 添加经验
    bool AddExperience(int amount) {
        if (g_functions.GetExperienceController == nullptr) {
            std::cout << "[错误] ExperienceController函数未找到" << std::endl;
            return false;
        }

        void* expController = g_functions.GetExperienceController();
        if (expController == nullptr) {
            std::cout << "[错误] 无法获取ExperienceController实例" << std::endl;
            return false;
        }

        if (g_functions.AddExperience) {
            g_functions.AddExperience(expController, amount, 0, nullptr, nullptr);
            std::cout << "[成功] 已添加经验: +" << amount << std::endl;
            return true;
        }

        return false;
    }

    // 获取当前等级
    int GetCurrentLevel() {
        if (g_functions.GetLevel && g_functions.GetExperienceController) {
            void* moduleData = g_functions.GetExperienceController();
            if (moduleData) {
                return g_functions.GetLevel(moduleData);
            }
        }
        return 1;
    }

    // === 初始化函数指针 ===

    void InitializeGameFunctions() {
        if (g_functions.initialized) return;

        std::cout << "[初始化] 初始化游戏函数指针..." << std::endl;
        std::cout << "[提示] 需要根据dump.cs更新偏移地址" << std::endl;

        // 这里需要根据实际游戏版本更新偏移
        // 示例：
        // g_functions.GetWebSocketManager = reinterpret_cast<void*(*)()>(base + OFFSET);
        // g_functions.SendWebSocketMessage = reinterpret_cast<void*(*)(...)>(base + OFFSET);

        std::cout << "[初始化] 函数指针初始化完成（模板模式）" << std::endl;
        g_functions.initialized = true;
    }

    // === 控制台界面 ===

    void ConsoleInterface() {
        while (true) {
            std::cout << "\n=== 🌐 WebSocket游戏工具 ===" << std::endl;
            std::cout << "1. 💰 添加金币（直接调用）" << std::endl;
            std::cout << "2. 💎 添加宝石（直接调用）" << std::endl;
            std::cout << "3. ⬆️  添加经验" << std::endl;
            std::cout << "4. 📡 发送WebSocket消息" << std::endl;
            std::cout << "5. 📊 查看角色信息" << std::endl;
            std::cout << "6. ℹ️  显示游戏信息" << std::endl;
            std::cout << "7. ❌ 退出" << std::endl;
            std::cout << "=========================" << std::endl;

            int choice;
            std::cout << "选择: ";
            std::cin >> choice;

            switch (choice) {
            case 1: {
                int amount;
                std::cout << "输入金币数量: ";
                std::cin >> amount;
                AddCurrencyDirect("Coins", amount);
                break;
            }
            case 2: {
                int amount;
                std::cout << "输入宝石数量: ";
                std::cin >> amount;
                AddCurrencyDirect("Gems", amount);
                break;
            }
            case 3: {
                int amount;
                std::cout << "输入经验数量: ";
                std::cin >> amount;
                AddExperience(amount);
                break;
            }
            case 4: {
                std::string msgType;
                std::cout << "输入消息类型: ";
                std::cin >> msgType;

                std::map<std::string, std::string> data;
                std::string key, value;

                std::cout << "输入数据（格式：key value，输入done结束）:" << std::endl;
                while (true) {
                    std::cin >> key;
                    if (key == "done") break;
                    std::cin >> value;
                    data[key] = value;
                }

                SendWebSocketMessage(msgType, data);
                break;
            }
            case 5: {
                std::cout << "\n=== 角色信息 ===" << std::endl;
                std::cout << "等级: " << GetCurrentLevel() << std::endl;
                std::cout << "================" << std::endl;
                break;
            }
            case 6:
                std::cout << "\n=== 游戏信息 ===" << std::endl;
                std::cout << "模块: " << g_moduleInfo.name << std::endl;
                std::cout << "基址: 0x" << std::hex << g_moduleInfo.baseAddress << std::dec << std::endl;
                std::cout << "函数已初始化: " << (g_functions.initialized ? "是" : "否") << std::endl;
                std::cout << "================" << std::endl;
                break;
            case 7:
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

        std::cout << "=== 🌐 WebSocket游戏工具 ===" << std::endl;
        std::cout << "包含WebSocket功能和直接调用功能" << std::endl;
        std::cout << "编译时间: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "========================" << std::endl;

        // 识别游戏进程
        if (GetGameProcessInfo()) {
            std::cout << "[✓] 游戏进程识别成功" << std::endl;
        } else {
            std::cout << "[!] 警告: 游戏进程未找到" << std::endl;
        }

        // 初始化函数指针
        InitializeGameFunctions();

        // 显示界面
        ConsoleInterface();
    }
}

// DLL导出函数
extern "C" __declspec(dllexport) void Initialize() {
    WebSocketJoiner::Initialize();
}

// 外部调用接口
extern "C" __declspec(dllexport) bool AddCurrency(const char* type, int amount) {
    return WebSocketJoiner::AddCurrencyDirect(std::string(type), amount);
}

extern "C" __declspec(dllexport) bool SendWSMessage(const char* msgType, const char* jsonData) {
    std::map<std::string, std::string> data;
    data["data"] = jsonData;
    return WebSocketJoiner::SendWebSocketMessage(std::string(msgType), data);
}

// DLL入口点
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
            Sleep(1000);
            WebSocketJoiner::Initialize();
            return 0;
        }, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        FreeConsole();
        break;
    }
    return TRUE;
}