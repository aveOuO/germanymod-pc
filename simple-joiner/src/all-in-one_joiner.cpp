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
		std::cout << "  WebSocket Status: " << (IsWebSocketReady() ? "Connected" : "Not Connected") << std::endl;
		std::cout << "========================================" << std::endl;
		std::cout << "[Weapons]" << std::endl;
		std::cout << "  1. Unlock All Weapons (Lv65/Rare5)" << std::endl;
		std::cout << "  2. Unlock RGB Weapon Set" << std::endl;
		std::cout << "  3. Unlock Goofy Weapons" << std::endl;
		std::cout << "  4. Unlock All Weapon Skins" << std::endl;
		std::cout << "  5. Add Weapon Parts" << std::endl;
		std::cout << "[Items/Modules]" << std::endl;
		std::cout << "  6. Unlock All Gadgets" << std::endl;
		std::cout << "  7. Unlock Module Points x99999" << std::endl;
		std::cout << "  8. Upgrade All Modules" << std::endl;
		std::cout << "  9. Unlock All Graffities" << std::endl;
		std::cout << "  10. Add Free Upgrades x999" << std::endl;
		std::cout << "[Statistics]" << std::endl;
		std::cout << "  11. Add 100 Win Stats" << std::endl;
		std::cout << "  12. Add 1 Win (Simple)" << std::endl;
		std::cout << "[System]" << std::endl;
		std::cout << "  13. Reload Progress" << std::endl;
		std::cout << "  14. Recheck WebSocket Status" << std::endl;
		std::cout << "  0. Exit" << std::endl;
		std::cout << "========================================" << std::endl;
		std::cout << "Choice: ";
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
				case 1: AccountCommands::UnlockWeapon(65, 5, (size_t)0, (size_t)99999); std::cout << "[+] Unlock All Weapons - Sent" << std::endl; break;
				case 2: AccountCommands::UnlockRGBWeaponSet(65, 5); std::cout << "[+] RGB Weapons - Sent" << std::endl; break;
				case 3: AccountCommands::UnlockGoofyWeapons(65, 5); std::cout << "[+] Goofy Weapons - Sent" << std::endl; break;
				case 4: AccountCommands::UnlockWeaponSkin((size_t)0, (size_t)99999); std::cout << "[+] Weapon Skins - Sent" << std::endl; break;
				case 5: AccountCommands::AddParts(); std::cout << "[+] Weapon Parts - Sent" << std::endl; break;
				case 6: AccountCommands::UnlockGadget(65); std::cout << "[+] Gadgets - Sent" << std::endl; break;
				case 7: AccountCommands::UnlockModules(99999); std::cout << "[+] Module Points - Sent" << std::endl; break;
				case 8: AccountCommands::UpgradeModules(); std::cout << "[+] Module Upgrade - Sent" << std::endl; break;
				case 9: AccountCommands::UnlockGraffities(); std::cout << "[+] Graffities - Sent" << std::endl; break;
				case 10: AccountCommands::AddFreeUpgrades(999); std::cout << "[+] Free Upgrades - Sent" << std::endl; break;
				case 11: AccountCommands::AddWinStat(0, 100); std::cout << "[+] 100 Wins - Sent" << std::endl; break;
				case 12: AccountCommands::AddMatchStat(true); std::cout << "[+] 1 Win - Sent" << std::endl; break;
				case 13: WebsocketCore::Reload(); std::cout << "[+] Progress Reloaded - Sent" << std::endl; break;
				case 14: std::cout << "[INFO] WebSocket: " << (IsWebSocketReady() ? "Connected" : "Not Connected") << std::endl; break;
				case 0: std::cout << "Exiting..." << std::endl; return;
				default: std::cout << "[!] Invalid choice" << std::endl; break;
				}
			}
			catch (const std::exception& e)
			{
				std::cout << "[!] Operation Error: " << e.what() << std::endl;
				std::cout << "[!] Usually because WebSocket not connected or game version pattern failed." << std::endl;
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

		// 设置控制台编码为中文 GBK (Windows 中文系统默认)
		SetConsoleOutputCP(936);
		SetConsoleCP(936);

		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
		freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
		DebugLog("Console redirection set up");

		// 设置控制台标题和窗口位置，让它更显眼
		if (!SetConsoleTitle(TEXT("Pixel Gun 3D Mod Tool - Console Menu")))
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
		std::cout << "  Pixel Gun 3D - Mod Tool (Console Version)" << std::endl;
		std::cout << "========================================" << std::endl;
		std::cout << "Build Time: " << __DATE__ << " " << __TIME__ << std::endl;
		std::cout << "Framework: IL2CPP + PointerFunctions + WebsocketCore + MinHook" << std::endl;
		std::cout << "========================================" << std::endl;
		std::cout << "[SUCCESS] Console created successfully!" << std::endl;

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
		std::cout << "\n[TIP] Console menu is now open! Look for window titled 'Pixel Gun 3D Mod Tool'" << std::endl;
		std::cout << "[TIP] If you don't see this window, press Alt+Tab to switch windows" << std::endl;
		std::cout << "[INFO] Console window should be displayed at top of screen" << std::endl;

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
	AllInOneJoiner::DebugLog("=== EXPORT Initialize() called ===");
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
		}, hModule, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		AllInOneJoiner::DebugLog("=== DLL_PROCESS_DETACH ===");
		FreeConsole();
		break;
	}
	return TRUE;
}
