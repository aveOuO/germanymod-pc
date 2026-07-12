#include <windows.h>
#include <iostream>
#include <string>
#include <tlhelp32.h>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cctype>

#pragma comment(lib, "advapi32.lib")

using namespace std;
namespace fs = std::filesystem;

// 简单的DLL注入器
class SimpleInjector {
private:
    string processName;
    string dllPath;

    // 设置控制台UTF-8编码（修复中文乱码）
    void SetupConsoleEncoding() {
        // 设置控制台输出代码页为UTF-8
        SetConsoleOutputCP(65001);
        SetConsoleCP(65001);

        // 设置locale为UTF-8
        setlocale(LC_ALL, ".UTF-8");
    }

    // 检查文件是否存在
    bool FileExists(const string& path) {
        DWORD attrib = GetFileAttributesA(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    // 查找进程ID
    DWORD FindProcessId(const string& procName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(hSnapshot, &pe32)) {
            CloseHandle(hSnapshot);
            return 0;
        }

        do {
            if (procName == pe32.szExeFile) {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe32));

        CloseHandle(hSnapshot);
        return 0;
    }

    // 获取进程名称列表
    vector<string> GetProcessList() {
        vector<string> processes;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return processes;
        }

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe32)) {
            do {
                processes.push_back(pe32.szExeFile);
            } while (Process32Next(hSnapshot, &pe32));
        }

        CloseHandle(hSnapshot);
        return processes;
    }

    // 获取当前执行文件目录
    string GetCurrentExecutableDir() {
        char exePath[MAX_PATH];
        GetModuleFileNameA(nullptr, exePath, MAX_PATH);
        string path(exePath);
        size_t lastSlash = path.find_last_of("\\");
        return (lastSlash != string::npos) ? path.substr(0, lastSlash) : "";
    }

public:
    // 查找gamer.dll（需要设为public以便外部调用）
    string FindGamerDll() {
        // 1. 检查当前目录
        if (FileExists("gamer.dll")) {
            return "gamer.dll";
        }

        // 2. 检查执行文件目录
        string exeDir = GetCurrentExecutableDir();
        if (!exeDir.empty()) {
            string dllInExeDir = exeDir + "\\gamer.dll";
            if (FileExists(dllInExeDir)) {
                return dllInExeDir;
            }

            // 3. 检查bin子目录
            string dllInBin = exeDir + "\\bin\\gamer.dll";
            if (FileExists(dllInBin)) {
                return dllInBin;
            }
        }

        // 4. 检查当前目录的bin子目录
        if (FileExists("bin\\gamer.dll")) {
            return "bin\\gamer.dll";
        }

        return ""; // 未找到
    }

    // 查找游戏进程:先精确 PixelGun3D.exe,失败则模糊找含 "pixel" 的
    // (游戏目录是 "Pixel Gun 3D PC Edition",进程名可能带空格,所以兜底模糊匹配)
    string FindGameProcess() {
        if (FindProcessId("PixelGun3D.exe") != 0) return "PixelGun3D.exe";
        auto procs = GetProcessList();
        for (const auto& p : procs) {
            string lower = p;
            transform(lower.begin(), lower.end(), lower.begin(),
                      [](unsigned char c) { return (char)tolower(c); });
            if (lower.find("pixel") != string::npos) return p;
        }
        return "";
    }

public:
    SimpleInjector(const string& dll) : dllPath(dll) {
        SetupConsoleEncoding();
    }

    // 注入DLL到指定进程
    bool Inject(const string& targetProcess) {
        processName = targetProcess;
        DWORD processId = FindProcessId(processName);

        if (processId == 0) {
            cerr << "[错误] 找不到进程: " << processName << endl;
            return false;
        }

        cout << "[信息] 找到进程 " << processName << " (PID: " << processId << ")" << endl;

        // 打开进程
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (hProcess == nullptr) {
            cerr << "[错误] 无法打开进程，需要管理员权限" << endl;
            return false;
        }

        // 在目标进程中分配内存
        size_t dllPathSize = dllPath.length() + 1;
        LPVOID pRemoteMemory = VirtualAllocEx(hProcess, nullptr, dllPathSize,
                                              MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (pRemoteMemory == nullptr) {
            cerr << "[错误] 内存分配失败" << endl;
            CloseHandle(hProcess);
            return false;
        }

        // 写入DLL路径
        SIZE_T bytesWritten;
        if (!WriteProcessMemory(hProcess, pRemoteMemory, dllPath.c_str(),
                               dllPathSize, &bytesWritten)) {
            cerr << "[错误] 写入内存失败" << endl;
            VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        // 获取LoadLibraryA地址
        LPVOID pLoadLibrary = reinterpret_cast<LPVOID>(GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"));
        if (pLoadLibrary == nullptr) {
            cerr << "[错误] 无法获取LoadLibraryA地址" << endl;
            VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        // 创建远程线程
        HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
                                           (LPTHREAD_START_ROUTINE)pLoadLibrary,
                                           pRemoteMemory, 0, nullptr);
        if (hThread == nullptr) {
            cerr << "[错误] 创建远程线程失败" << endl;
            VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        cout << "[信息] 正在注入DLL..." << endl;

        // 等待线程完成
        WaitForSingleObject(hThread, INFINITE);

        // 清理
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);

        cout << "[成功] DLL注入完成！" << endl;
        return true;
    }

    // 显示可用进程列表
    void ShowProcessList() {
        auto processes = GetProcessList();
        cout << "\n=== 可用进程列表 ===" << endl;

        for (const auto& proc : processes) {
            cout << "  " << proc << endl;
        }
        cout << "=====================\n" << endl;
    }

    // 交互式注入
    void InteractiveInject() {
        cout << "=== 简单DLL注入器 ===" << endl;
        cout << "DLL路径: " << dllPath << endl;
        cout << "\n选择操作:" << endl;
        cout << "1. 输入进程名注入" << endl;
        cout << "2. 查看可用进程" << endl;
        cout << "3. 退出" << endl;

        int choice;
        cout << "选择: ";
        cin >> choice;

        if (choice == 1) {
            string procName;
            cout << "输入进程名 (例如: PixelGun3D.exe): ";
            cin >> procName;
            Inject(procName);
        } else if (choice == 2) {
            ShowProcessList();
            InteractiveInject(); // 重新显示菜单
        }
    }
};

int main(int argc, char* argv[]) {
    // 设置控制台编码
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    cout << "=== Simple DLL Injector ===" << endl;
    cout << "Compile Time: " << __DATE__ << " " << __TIME__ << endl;
    cout << "Based on: " << PROJECT_NAME << " project" << endl;

    string dllPath;
    string processName;

    if (argc >= 3) {
        // 命令行模式: injector.exe dll.dll process.exe
        dllPath = argv[1];
        processName = argv[2];
    } else if (argc == 2) {
        // 单参数模式
        dllPath = argv[1];
        cout << "Using DLL: " << dllPath << endl;
        cout << "Enter target process name: ";
        cin >> processName;
    } else {
        // 自动检测gamer.dll
        cout << "[信息] 正在查找 gamer.dll..." << endl;

        SimpleInjector tempInjector(""); // 临时实例用于查找
        string gamerDll = tempInjector.FindGamerDll();

        if (!gamerDll.empty()) {
            dllPath = gamerDll;
            cout << "[成功] 找到 gamer.dll: " << dllPath << endl;
        } else {
            cout << "[提示] 未找到 gamer.dll" << endl;
            cout << "\n请输入DLL路径 (支持相对/绝对路径): " << endl;
            cout << "示例:" << endl;
            cout << "  - mytools.dll" << endl;
            cout << "  - bin\\mytools.dll" << endl;
            cout << "  - C:\\path\\to\\mytools.dll" << endl;
            cout << "\nDLL路径: ";
            getline(cin, dllPath);

            // 去除首尾空格
            dllPath.erase(0, dllPath.find_first_not_of(" \t\n\r"));
            dllPath.erase(dllPath.find_last_not_of(" \t\n\r") + 1);

            if (dllPath.empty()) {
                cout << "[错误] DLL路径为空" << endl;
                system("pause");
                return 1;
            }
        }

        SimpleInjector injector(dllPath);
        // 默认游戏进程,不手动输入(精确 PixelGun3D.exe,失败则模糊找含 pixel 的)
        string processName = injector.FindGameProcess();
        if (processName.empty()) {
            cout << "[错误] 未找到游戏进程(含 pixel 的)。请确认游戏已启动," << endl;
            cout << "       或用命令行指定: injector.exe gamer.dll 进程名.exe" << endl;
            system("pause");
            return 1;
        }
        cout << "[信息] 目标进程: " << processName << endl;
        injector.Inject(processName);
        cout << "\n按任意键退出..." << endl;
        system("pause");
        return 0;
    }

    SimpleInjector injector(dllPath);
    if (injector.Inject(processName)) {
        cout << "\nPress any key to exit..." << endl;
        cin.get();
        return 0;
    } else {
        cout << "\nInjection failed, press any key to exit..." << endl;
        cin.get();
        return 1;
    }
}