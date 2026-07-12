@echo off
REM ========================================
REM  编译测试DLL - 验证注入是否工作
REM ========================================

echo === 编译测试DLL ===

set GCC=D:\program\msys64\ucrt64\bin\g++.exe

"%GCC%" -std=c++17 -shared -m64 -static-libgcc -static-libstdc++ -static -s src\test_dll.cpp -o build\test.dll

if %ERRORLEVEL% NEQ 0 (
    echo [错误] 测试DLL编译失败
    pause
    exit /b 1
)

echo.
echo ========================================
echo  测试DLL编译成功！
echo ========================================
echo.
echo 生成的文件：
echo   build\test.dll
echo.
echo 测试方法：
echo   1. 运行游戏
echo   2. 复制 test.dll 到游戏目录
echo   3. 创建一个简单的injector来注入test.dll
echo   4. 检查 D:\test_injection.log 文件
echo   5. 查找标题为 "测试DLL - 注入成功！" 的窗口
echo.
echo ========================================

pause
