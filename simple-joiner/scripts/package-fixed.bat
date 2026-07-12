@echo off
REM ========================================
REM 一键打包脚本 - 生成 injector.exe + gamer.dll
REM ========================================

echo.
echo ========================================
echo    一键打包脚本
echo ========================================
echo.

REM 设置编译器路径（使用正斜杠兼容性更好）
set GCC_PATH=D:/program/msys64/ucrt64/bin
set PROJECT_NAME=nazimod

echo [1/4] 清理并创建输出目录...
if exist release rd /s /q release
mkdir release

echo [2/4] 编译注入器...
"%GCC_PATH:/=\%/g++.exe" -std=c++17 -m64 -DPROJECT_NAME="%PROJECT_NAME%" -static-libgcc -static-libstdc++ -static -s src\injector.cpp -o release\injector.exe 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo [错误] 注入器编译失败
    pause
    exit /b 1
)

echo [3/4] 编译并打包DLL...
"%GCC_PATH:/=\%/g++.exe" -std=c++17 -shared -m64 -DPROJECT_NAME="%PROJECT_NAME%" -static-libgcc -static-libstdc++ -static -s src\all-in-one_joiner.cpp -o release\gamer.dll 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo [错误] DLL编译失败
    pause
    exit /b 1
)

echo [4/4] 创建启动脚本...
echo @echo off > release\启动.bat
echo echo 正在启动游戏工具... >> release\启动.bat
echo echo. >> release\启动.bat
echo cd /d "%%~dp0" >> release\启动.bat
echo injector.exe >> release\启动.bat
echo pause >> release\启动.bat

echo.
echo ========================================
echo [成功] 打包完成！
echo ========================================
echo.
echo 生成的文件：
echo   - injector.exe     (注入器)
echo   - gamer.dll        (游戏工具DLL)
echo   - 启动.bat         (快速启动脚本)
echo.
echo 使用方法：
echo   方式1: 双击 启动.bat
echo   方式2: 运行 injector.exe
echo.
echo ========================================

pause
