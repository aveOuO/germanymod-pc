#include <windows.h>
#include <iostream>
#include <fstream>

// 简单的测试DLL - 验证注入是否工作
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        // 禁用线程调用以提高稳定性
        DisableThreadLibraryCalls(hModule);

        // 尝试创建日志文件
        std::ofstream logFile("D:\\test_injection.log");
        logFile << "=== DLL_PROCESS_ATTACH ===" << std::endl;
        logFile << "DLL 已成功注入到进程!" << std::endl;
        logFile << "进程ID: " << GetCurrentProcessId() << std::endl;
        logFile << "线程ID: " << GetCurrentThreadId() << std::endl;
        logFile.close();

        // 尝试创建控制台
        if (AllocConsole())
        {
            freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
            freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);

            SetConsoleTitle(TEXT("🔧 测试DLL - 注入成功！"));

            std::cout << "========================================" << std::endl;
            std::cout << "  DLL 注入测试 - 成功！" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "如果你看到这个窗口，说明注入工作正常！" << std::endl;
            std::cout << "========================================" << std::endl;
        }
        else
        {
            // 如果控制台创建失败，写入日志
            std::ofstream errLog("D:\\test_injection.log", std::ios::app);
            errLog << "ERROR: AllocConsole() 失败！" << std::endl;
            errLog.close();
        }
        break;
    }
    case DLL_PROCESS_DETACH:
        FreeConsole();
        break;
    }
    return TRUE;
}

// 简单的导出函数
extern "C" __declspec(dllexport) void TestFunction()
{
    MessageBoxA(NULL, "测试函数被调用！", "DLL测试", MB_OK);
}
