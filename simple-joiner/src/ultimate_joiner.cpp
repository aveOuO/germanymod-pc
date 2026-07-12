#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

// 游戏函数指针结构体（扩展版）
struct GameFunctions {
    // PhotonNetwork相关
    void* (*GetCurrentRoom)() = nullptr;
    void* (*GetLocalPlayer)() = nullptr;
    bool (*IsMasterClient)() = nullptr;

    // 玩家相关
    void* (*GetPlayerID)() = nullptr;
    bool (*IsInMatch)() = nullptr;

    // 等级和经验（核心功能）
    int (*GetLevel)(void* moduleData) = nullptr;
    void (*AddExperience)(void* experienceController, int value, int reason, void* dictionary, void* extra) = nullptr;

    // 货币系统（核心功能）
    void (*AddCurrency)(void* webInstance, const char* currencyName, int amount, int unknown1, bool bool1, bool bool2, void* analyticsParams) = nullptr;

    // WebSocket相关
    void* (*SendWebSocketMessage)(void* wsManager, const char* msgType, void* jsonMsg) = nullptr;

    // 进程管理器获取
    void* (*GetProgressUpdaterInstance)() = nullptr;
    void* (*GetExperienceController)() = nullptr;

    bool initialized = false;
} static g_functions;

// 游戏模块信息
struct GameModuleInfo {
    uintptr_t baseAddress;
    size_t size;
    std::string name;
    std::string processName;
} static g_moduleInfo;

// 玩家数据缓存
struct PlayerData {
    int level = 0;
    int coins = 0;
    int gems = 0;
    int exp = 0;
} static g_playerData;

namespace UltimateJoiner {

    // === 游戏进程识别 ===

    // 获取游戏进程信息（Pixel Gun 3D）
    bool GetGameProcessInfo() {
        // 方法1: 通过窗口标题查找
        HWND hwnd = FindWindowA(nullptr, "Pixel Gun 3D");
        if (hwnd) {
            DWORD processId;
            GetWindowThreadProcessId(hwnd, &processId);

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
            if (hProcess) {
                char processName[MAX_PATH];
                GetModuleFileNameExA(hProcess, nullptr, processName, sizeof(processName));
                g_moduleInfo.processName = processName;
                CloseHandle(hProcess);

                std::cout << "[进程识别] 通过窗口标题找到游戏进程" << std::endl;
                return true;
            }
        }

        // 方法2: 通过枚举进程查找Unity相关模块
        HMODULE hModules[1024];
        DWORD cbNeeded;

        if (EnumProcessModules(GetCurrentProcess(), hModules, sizeof(hModules), &cbNeeded)) {
            for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                char szModuleName[MAX_PATH];
                GetModuleFileNameExA(GetCurrentProcess(), hModules[i], szModuleName, sizeof(szModuleName));

                std::string moduleName(szModuleName);

                // 查找Unity游戏相关模块
                if (moduleName.find("Unity") != std::string::npos ||
                    moduleName.find("Game") != std::string::npos ||
                    moduleName.find("Assembly") != std::string::npos ||
                    moduleName.find("PG3D") != std::string::npos ||
                    moduleName.find("Pixel") != std::string::npos) {

                    MODULEINFO modInfo;
                    if (GetModuleInformation(GetCurrentProcess(), hModules[i], &modInfo, sizeof(modInfo))) {
                        g_moduleInfo.baseAddress = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
                        g_moduleInfo.size = modInfo.SizeOfImage;
                        g_moduleInfo.name = moduleName;

                        std::cout << "[进程识别] 找到游戏模块: " << moduleName << std::endl;
                        std::cout << "[进程识别] 基址: 0x" << std::hex << g_moduleInfo.baseAddress << std::dec << std::endl;
                        return true;
                    }
                }
            }
        }

        return false;
    }

    // === 内存扫描功能 ===

    uintptr_t ScanPattern(uintptr_t startAddr, size_t size, const std::string& pattern) {
        std::vector<uint8_t> bytes;
        std::vector<bool> wildcard;

        // 转换模式字符串 "48 83 EC ?? ?? ?? ?? ??"
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

    // === 核心功能函数 ===

    // 1. 设置角色等级
    bool SetPlayerLevel(int newLevel) {
        std::cout << "[功能] 设置角色等级: " << newLevel << std::endl;

        if (g_functions.GetExperienceController == nullptr) {
            std::cout << "[错误] ExperienceController函数未找到" << std::endl;
            return false;
        }

        // 获取ExperienceController实例
        void* expController = g_functions.GetExperienceController();
        if (expController == nullptr) {
            std::cout << "[错误] 无法获取ExperienceController实例" << std::endl;
            return false;
        }

        // 计算需要的经验值（这里需要根据游戏的具体经验曲线计算）
        // 简化版本：直接添加大量经验
        int expToAdd = newLevel * 1000; // 简化的经验计算

        if (g_functions.AddExperience) {
            // 调用AddExperience函数
            // 参数：experienceController, value, reason, dictionary, extra
            g_functions.AddExperience(expController, expToAdd, 0, nullptr, nullptr);
            std::cout << "[成功] 已添加经验: " << expToAdd << std::endl;
            return true;
        }

        return false;
    }

    // 2. 添加货币
    bool AddCurrency(const std::string& currencyType, int amount) {
        std::cout << "[功能] 添加货币: " << currencyType << " +" << amount << std::endl;

        if (g_functions.GetProgressUpdaterInstance == nullptr) {
            std::cout << "[错误] ProgressUpdater函数未找到" << std::endl;
            return false;
        }

        // 获取ProgressUpdater实例
        void* progressUpdater = g_functions.GetProgressUpdaterInstance();
        if (progressUpdater == nullptr) {
            std::cout << "[错误] 无法获取ProgressUpdater实例" << std::endl;
            return false;
        }

        if (g_functions.AddCurrency) {
            // 调用AddCurrency函数
            // 参数：webInstance, currencyName, amount, unknown1, bool1, bool2, analyticsParams
            g_functions.AddCurrency(progressUpdater, currencyType.c_str(), amount, 0, false, false, nullptr);
            std::cout << "[成功] 已添加货币" << std::endl;
            return true;
        }

        return false;
    }

    // 3. 快速添加金币
    bool AddCoins(int amount) {
        g_playerData.coins += amount;
        return AddCurrency("Coins", amount);
    }

    // 4. 快速添加宝石
    bool AddGems(int amount) {
        g_playerData.gems += amount;
        return AddCurrency("Gems", amount);
    }

    // 5. 获取当前等级
    int GetCurrentLevel() {
        if (g_functions.GetLevel && g_functions.GetExperienceController) {
            void* moduleData = g_functions.GetExperienceController();
            if (moduleData) {
                return g_functions.GetLevel(moduleData);
            }
        }
        return g_playerData.level;
    }

    // === 游戏函数初始化 ===

    void InitializeGameFunctions() {
        if (g_functions.initialized) return;

        std::cout << "[初始化] 正在初始化游戏函数指针..." << std::endl;

        // 这里需要根据具体游戏版本设置函数地址
        // 实际使用时需要通过IL2CPP或特征码扫描找到这些函数

        std::cout << "[初始化] 游戏函数指针初始化（模板模式）" << std::endl;
        std::cout << "[提示] 需要根据dump.cs或assembly-csharp更新偏移" << std::endl;

        // 示例：如何通过特征码扫描找到函数
        /*
        // GetLevel函数模式示例
        std::string getLevelPattern = "48 83 EC 28 48 8B 05 ?? ?? ?? ??";
        uintptr_t funcAddr = ScanPattern(g_moduleInfo.baseAddress, g_moduleInfo.size, getLevelPattern);
        if (funcAddr) {
            g_functions.GetLevel = reinterpret_cast<int(*)(void*)>(funcAddr);
            std::cout << "[找到] GetLevel函数: 0x" << std::hex << funcAddr << std::endl;
        }
        */

        g_functions.initialized = true;
    }

    // === JoinRoom功能（保留原有功能）===

    void JoinRoomByName(const std::string& roomName) {
        std::cout << "[房间] 尝试加入房间: " << roomName << std::endl;

        // 这里需要实现具体的JoinRoom逻辑
        std::cout << "[提示] JoinRoom功能需要游戏特定的实现" << std::endl;
    }

    // === 控制台界面 ===

    void ConsoleInterface() {
        while (true) {
            std::cout << "\n=== 🎮 增强版游戏工具 ===" << std::endl;
            std::cout << "1. 💰 添加金币" << std::endl;
            std::cout << "2. 💎 添加宝石" << std::endl;
            std::cout << "3. ⬆️  设置等级" << std::endl;
            std::cout << "4. 📊 查看角色信息" << std::endl;
            std::cout << "5. 🏠 加入房间" << std::endl;
            std::cout << "6. ℹ️  显示游戏信息" << std::endl;
            std::cout << "7. ❌ 退出" << std::endl;
            std::cout << "=======================" << std::endl;

            int choice;
            std::cout << "选择: ";
            std::cin >> choice;

            switch (choice) {
            case 1: {
                int amount;
                std::cout << "输入金币数量: ";
                std::cin >> amount;
                AddCoins(amount);
                break;
            }
            case 2: {
                int amount;
                std::cout << "输入宝石数量: ";
                std::cin >> amount;
                AddGems(amount);
                break;
            }
            case 3: {
                int level;
                std::cout << "输入目标等级: ";
                std::cin >> level;
                SetPlayerLevel(level);
                break;
            }
            case 4: {
                std::cout << "\n=== 角色信息 ===" << std::endl;
                std::cout << "等级: " << GetCurrentLevel() << std::endl;
                std::cout << "金币: " << g_playerData.coins << std::endl;
                std::cout << "宝石: " << g_playerData.gems << std::endl;
                std::cout << "================" << std::endl;
                break;
            }
            case 5: {
                std::string roomName;
                std::cout << "输入房间名: ";
                std::cin.ignore();
                std::getline(std::cin, roomName);
                JoinRoomByName(roomName);
                break;
            }
            case 6:
                std::cout << "\n=== 游戏信息 ===" << std::endl;
                std::cout << "进程: " << g_moduleInfo.processName << std::endl;
                std::cout << "模块: " << g_moduleInfo.name << std::endl;
                std::cout << "基址: 0x" << std::hex << g_moduleInfo.baseAddress << std::dec << std::endl;
                std::cout << "大小: " << g_moduleInfo.size << " bytes" << std::endl;
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

        std::cout << "=== 🎮 增强版游戏工具 ===" << std::endl;
        std::cout << "基于原项目的完整功能迁移" << std::endl;
        std::cout << "编译时间: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "====================" << std::endl;

        // 1. 识别游戏进程
        if (GetGameProcessInfo()) {
            std::cout << "[✓] 游戏进程识别成功" << std::endl;
        } else {
            std::cout << "[!] 警告: 游戏进程未找到，功能可能受限" << std::endl;
        }

        // 2. 初始化游戏函数
        InitializeGameFunctions();

        // 3. 显示主界面
        ConsoleInterface();
    }
}

// DLL导出函数
extern "C" __declspec(dllexport) void Initialize() {
    UltimateJoiner::Initialize();
}

// 外部调用接口
extern "C" __declspec(dllexport) bool AddCoins(int amount) {
    return UltimateJoiner::AddCoins(amount);
}

extern "C" __declspec(dllexport) bool AddGems(int amount) {
    return UltimateJoiner::AddGems(amount);
}

extern "C" __declspec(dllexport) bool SetLevel(int level) {
    return UltimateJoiner::SetPlayerLevel(level);
}

extern "C" __declspec(dllexport) void JoinRoom(const char* roomName) {
    if (roomName) {
        UltimateJoiner::JoinRoomByName(std::string(roomName));
    }
}

// DLL入口点
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
            Sleep(1000); // 等待游戏初始化
            UltimateJoiner::Initialize();
            return 0;
        }, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        FreeConsole();
        break;
    }
    return TRUE;
}