// =============================================================================
// Pixel Gun 3D - WebSocket 工具(真框架版)
//
// 这是 simple-joiner 的主入口。它不再用"直接调用游戏函数"的假实现,
// 而是接入主项目的完整 WebSocket 框架:
//   IL2CPP::INIT()        → 初始化 IL2CPP 反射(需要 GameAssembly.dll)
//   PointerFunctions::INIT() → 解析所有游戏函数指针(CommandID/Pointer)
//   WebsocketCore::INIT() → Hook AesFacade 加密层 + WebSocketManager::Send
//
// 之后所有"添加武器/货币/统计"功能都通过 WebSocket 注入命令
// (WebsocketCore::QueuePackage → AesFacade 加密 → 服务器),与主项目一致。
// =============================================================================

#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <fstream>

#include <IL2CPP.hpp>
// #include <imgui.h>  // 暂时禁用 ImGui
// #include "framework/backend/UIBackend.hpp"  // 暂时禁用渲染框架
#include "game/import/PointerFunctions.hpp"
#include "game/websocket/WebsocketCore.hpp"
#include "game/websocket/AccountCommands.hpp"
#include "game/websocket/ClanCommands.hpp"
#include "game/websocket/SquadCommands.hpp"

namespace AllInOneJoiner
{
	// WebSocketManager 实例由 SendSocketMessage hook 在游戏首次发消息时捕获。
	// 在它就绪前调用 QueuePackage 会触发 assert,所以菜单里先检查。
	bool IsWebSocketReady()
	{
		return WebsocketCore::WSManagerInstance != nullptr;
	}

	// ImGui 菜单状态
	static bool gMenuShown = true;
	static bool gMenuInitialized = false;

	// 调试输出函数
	void DebugLog(const char* message)
	{
		// 输出到文件以便调试
		static std::ofstream logFile("D:\\mod_debug.log", std::ios::app);
		logFile << "[" << (void*)GetCurrentThreadId() << "] " << message << std::endl;
		logFile.flush();

		// 尝试输出到调试器
		OutputDebugStringA(message);
		OutputDebugStringA("\n");
	}

	void WaitForWebSocket()
	{
		std::cout << "[*] 等待 WebSocket 连接(进入游戏大厅/在线即可触发)..." << std::endl;
		DebugLog("Waiting for WebSocket...");
		int waited = 0;
		while (!IsWebSocketReady() && waited < 60)
		{
			Sleep(1000);
			waited++;
			if (waited % 5 == 0)
				std::cout << "[*] 已等待 " << waited << "s..." << std::endl;
		}
		std::cout << (IsWebSocketReady() ? "[+] WebSocket 就绪!" : "[!] 超时,功能仍可尝试(可能 assert)") << std::endl;
		DebugLog(IsWebSocketReady() ? "WebSocket ready!" : "WebSocket timeout!");
	}

	bool InitFramework()
	{
		std::cout << "[*] 等待 GameAssembly.dll..." << std::endl;
		DebugLog("Waiting for GameAssembly.dll...");
		while (GetModuleHandleA("GameAssembly.dll") == nullptr)
			Sleep(500);
		Sleep(2000); // 等 IL2CPP 运行时初始化

		try
		{
			std::cout << "[*] 初始化 IL2CPP 反射框架..." << std::endl;
			DebugLog("Initializing IL2CPP...");
			IL2CPP::INIT();
			DebugLog("IL2CPP initialized successfully!");

			std::cout << "[*] 解析游戏函数指针(CommandID/Pointer)..." << std::endl;
			DebugLog("Initializing PointerFunctions...");
			PointerFunctions::INIT();
			DebugLog("PointerFunctions initialized successfully!");

			std::cout << "[*] 注册 WebSocket hooks(SendSocketMessage/AesFacade)..." << std::endl;
			DebugLog("Initializing WebsocketCore...");
			WebsocketCore::INIT();
			DebugLog("WebsocketCore initialized successfully!");
		}
		catch (const std::exception& e)
		{
			std::cout << "[!] 框架初始化异常: " << e.what() << std::endl;
			std::string errMsg = std::string("Framework init error: ") + e.what();
			DebugLog(errMsg.c_str());
			return false;
		}

		std::cout << "[+] 框架初始化完成!" << std::endl;
		DebugLog("Framework initialization completed successfully!");
		return true;
	}

	void ShowMenu()
	{
		std::cout << "\n========================================" << std::endl;
		std::cout << "  WebSocket 状态: " << (IsWebSocketReady() ? "已连接 ✓" : "未连接 ✗") << std::endl;
		std::cout << "========================================" << std::endl;
		std::cout << "【🔫 武器】" << std::endl;
		std::cout << "  1. 解锁所有武器 (Lv65/稀有5)" << std::endl;
		std::cout << "  2. 解锁 RGB 武器套装" << std::endl;
		std::cout << "  3. 解锁 Goofy 武器" << std::endl;
		std::cout << "  4. 解锁所有武器皮肤" << std::endl;
		std::cout << "  5. 添加武器零件" << std::endl;
		std::cout << "【🎒 道具/模块】" << std::endl;
		std::cout << "  6. 解锁所有道具" << std::endl;
		std::cout << "  7. 解锁模块点 x99999" << std::endl;
		std::cout << "  8. 升级所有模块" << std::endl;
		std::cout << "  9. 解锁所有涂鸦" << std::endl;
		std::cout << " 10. 添加免费升级 x999" << std::endl;
		std::cout << "【📊 统计】" << std::endl;
		std::cout << "  11. 添加 100 胜场统计" << std::endl;
		std::cout << "  12. 添加 1 胜场(简单)" << std::endl;
		std::cout << "【🔄 系统】" << std::endl;
		std::cout << "  13. 刷新进度 (Reload)" << std::endl;
		std::cout << "  14. 重新检查 WebSocket 状态" << std::endl;
		std::cout << "  0. 退出" << std::endl;
		std::cout << "========================================" << std::endl;
		std::cout << "选择: ";
	}

	void ConsoleInterface()
	{
		int choice;
		while (true)
		{
			ShowMenu();
			if (!(std::cin >> choice))
				break;

			try
			{
				switch (choice)
				{
				case 1: AccountCommands::UnlockWeapon(65, 5, (size_t)0, (size_t)99999); std::cout << "[+] 解锁所有武器 - 已发送" << std::endl; break;
				case 2: AccountCommands::UnlockRGBWeaponSet(65, 5); std::cout << "[+] RGB 武器 - 已发送" << std::endl; break;
				case 3: AccountCommands::UnlockGoofyWeapons(65, 5); std::cout << "[+] Goofy 武器 - 已发送" << std::endl; break;
				case 4: AccountCommands::UnlockWeaponSkin((size_t)0, (size_t)99999); std::cout << "[+] 武器皮肤 - 已发送" << std::endl; break;
				case 5: AccountCommands::AddParts(); std::cout << "[+] 武器零件 - 已发送" << std::endl; break;
				case 6: AccountCommands::UnlockGadget(65); std::cout << "[+] 道具 - 已发送" << std::endl; break;
				case 7: AccountCommands::UnlockModules(99999); std::cout << "[+] 模块点 - 已发送" << std::endl; break;
				case 8: AccountCommands::UpgradeModules(); std::cout << "[+] 模块升级 - 已发送" << std::endl; break;
				case 9: AccountCommands::UnlockGraffities(); std::cout << "[+] 涂鸦 - 已发送" << std::endl; break;
				case 10: AccountCommands::AddFreeUpgrades(999); std::cout << "[+] 免费升级 - 已发送" << std::endl; break;
				case 11: AccountCommands::AddWinStat(0, 100); std::cout << "[+] 100 胜场 - 已发送" << std::endl; break;
				case 12: AccountCommands::AddMatchStat(true); std::cout << "[+] 1 胜场 - 已发送" << std::endl; break;
				case 13: WebsocketCore::Reload(); std::cout << "[+] 进度刷新 - 已发送" << std::endl; break;
				case 14: std::cout << "[i] WebSocket: " << (IsWebSocketReady() ? "已连接" : "未连接") << std::endl; break;
				case 0: std::cout << "退出" << std::endl; return;
				default: std::cout << "[!] 无效选择" << std::endl; break;
				}
			}
			catch (const std::exception& e)
			{
				std::cout << "[!] 操作异常: " << e.what() << std::endl;
				std::cout << "[!] 通常是因为 WebSocket 还没连接,或游戏版本 pattern 失效。" << std::endl;
			}
		}
	}

	void Initialize()
	{
		DebugLog("=== Initialize() called ===");

		// 清空旧的日志文件
		std::ofstream("D:\\mod_debug.log").clear();

		if (!AllocConsole())
		{
			DebugLog("FAILED: AllocConsole() returned FALSE");
			return;
		}
		DebugLog("SUCCESS: AllocConsole() succeeded");

		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
		freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
		DebugLog("Console redirection set up");

		// 设置控制台标题和窗口位置，让它更显眼
		if (!SetConsoleTitle(TEXT("🎮 Pixel Gun 3D Mod Tool - 控制台菜单")))
		{
			DebugLog("WARNING: SetConsoleTitle() failed");
		}

		// 获取控制台窗口句柄并置顶
		HWND consoleWindow = GetConsoleWindow();
		if (consoleWindow != NULL)
		{
			DebugLog("Got console window handle, setting position...");
			// 设置窗口位置和大小
			if (SetWindowPos(consoleWindow, HWND_TOPMOST, 100, 100, 600, 400, SWP_SHOWWINDOW))
			{
				DebugLog("SUCCESS: Console window positioned and set to topmost");
			}
			else
			{
				DebugLog("FAILED: SetWindowPos() failed");
			}
		}
		else
		{
			DebugLog("FAILED: GetConsoleWindow() returned NULL");
		}

		// 显示初始消息
		std::cout << "========================================" << std::endl;
		std::cout << "  Pixel Gun 3D - WebSocket 工具(真框架版)" << std::endl;
		std::cout << "========================================" << std::endl;
		std::cout << "编译时间: " << __DATE__ << " " << __TIME__ << std::endl;
		std::cout << "框架: IL2CPP + PointerFunctions + WebsocketCore + MinHook" << std::endl;
		std::cout << "========================================" << std::endl;
		std::cout << "[DEBUG] 如果你看到这个消息，说明控制台已成功创建!" << std::endl;

		DebugLog("Console output started, beginning framework initialization...");

		if (!InitFramework())
		{
			std::cout << "[!] 初始化失败,菜单仍可打开但功能不可用。" << std::endl;
			DebugLog("Framework initialization FAILED");
		}
		else
		{
			WaitForWebSocket();
		}

		// 提示用户查看控制台窗口
		std::cout << "\n[提示] 控制台菜单已打开！请查看标题为 'Pixel Gun 3D Mod Tool' 的窗口" << std::endl;
		std::cout << "[提示] 如果看不到此窗口，请按 Alt+Tab 切换窗口" << std::endl;
		std::cout << "[DEBUG] 控制台窗口应该已经显示在屏幕上方位置" << std::endl;

		DebugLog("Starting console interface...");
		ConsoleInterface();
		DebugLog("Console interface ended");
	}
}

// =============================================================================
// DLL 导出接口(供注入器调用)
// =============================================================================
extern "C" __declspec(dllexport) void Initialize()
{
	DebugLog("=== EXPORT Initialize() called ===");
	AllInOneJoiner::Initialize();
}

extern "C" __declspec(dllexport) void UnlockAllWeapons()
{
	if (AllInOneJoiner::IsWebSocketReady())
		AccountCommands::UnlockWeapon(65, 5, (size_t)0, (size_t)99999);
}

extern "C" __declspec(dllexport) void ReloadProgress()
{
	if (AllInOneJoiner::IsWebSocketReady())
		WebsocketCore::Reload();
}

/*
// ImGui 渲染接口 - 需要在游戏渲染循环中调用
extern "C" __declspec(dllexport) void RenderUI()
{
	AllInOneJoiner::RenderImGuiMenu();
}
*/

// =============================================================================
// DLL 入口
// =============================================================================
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		AllInOneJoiner::DebugLog("=== DLL_PROCESS_ATTACH ===");
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, [](LPVOID) -> DWORD
		{
			AllInOneJoiner::DebugLog("Initialize thread started, sleeping 1 second...");
			Sleep(1000); // 等游戏初始化
			AllInOneJoiner::DebugLog("Calling Initialize()...");
			AllInOneJoiner::Initialize();
			AllInOneJoiner::DebugLog("Initialize() returned, thread ending");
			return 0;
		}, nullptr, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		AllInOneJoiner::DebugLog("=== DLL_PROCESS_DETACH ===");
		FreeConsole();
		break;
	}
	return TRUE;
}
