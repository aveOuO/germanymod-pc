@echo off
REM ========================================
REM 完整打包脚本 - 生成 injector.exe + gamer.dll
REM 包含 ImGui 渲染框架和所有组件
REM ========================================

echo.
echo ========================================
echo    完整打包脚本（包含 ImGui 渲染）
echo ========================================
echo.

REM 设置编译器路径
set GCC_PATH=D:\program\msys64\ucrt64\bin
set PROJECT_NAME=nazimod

echo [1/5] 清理并创建输出目录...
if exist build rd /s /q build
if exist release rd /s /q release
mkdir build
mkdir release

echo [2/5] 编译 MinHook (C17)...
set MHI=-Idependencies\minhook\include -Idependencies\minhook\src -Idependencies\minhook\src\hde
"%GCC_PATH%\g++.exe" -std=c17 -O2 %MHI% -c dependencies\minhook\src\buffer.c      -o build\mh_buffer.o      || goto :err
"%GCC_PATH%\g++.exe" -std=c17 -O2 %MHI% -c dependencies\minhook\src\hook.c        -o build\mh_hook.o       || goto :err
"%GCC_PATH%\g++.exe" -std=c17 -O2 %MHI% -c dependencies\minhook\src\trampoline.c  -o build\mh_trampoline.o || goto :err
"%GCC_PATH%\g++.exe" -std=c17 -O2 %MHI% -c dependencies\minhook\src\hde\hde64.c   -o build\mh_hde64.o      || goto :err

echo [3/5] 编译完整 gamer.dll (包含 ImGui 渲染框架)...
set INC=-Idependencies\il2cpp -Idependencies -Idependencies\logger -Idependencies\minhook\include -Idependencies\minhook\src -Idependencies\minhook\src\hde -Idependencies\imgui -Isrc\runtime -Isrc\runtime\framework\backend
set DEF=-DLOGGER_DEFINE_MACRO -D_USE_MATH_DEFINES
set LIBS=-lpsapi -ld3d10 -ld3d11 -static-libgcc -static-libstdc++ -static -s

"%GCC_PATH%\g++.exe" -std=c++20 -shared -O2 %INC% %DEF% ^
  dependencies\il2cpp\Main.cpp ^
  dependencies\il2cpp\backend\api-function.cpp ^
  dependencies\il2cpp\backend\vm-functions.cpp ^
  dependencies\il2cpp\service\ClassMapping.cpp ^
  dependencies\il2cpp\service\CommonCShrap.cpp ^
  dependencies\il2cpp\service\Dumper.cpp ^
  dependencies\il2cpp\structures\Class.cpp ^
  dependencies\il2cpp\structures\Domain.cpp ^
  dependencies\il2cpp\structures\FieldInfo.cpp ^
  dependencies\il2cpp\structures\Image.cpp ^
  dependencies\il2cpp\structures\MethodInfo.cpp ^
  dependencies\il2cpp\structures\Object.cpp ^
  dependencies\il2cpp\structures\String.cpp ^
  dependencies\il2cpp\structures\Type.cpp ^
  dependencies\il2cpp\structures\misc\PatternStructs.cpp ^
  dependencies\il2cpp\exceptions\ExceptionBase.cpp ^
  dependencies\logger\Logger.cpp ^
  dependencies\imgui\imgui.cpp ^
  dependencies\imgui\imgui_impl_win32.cpp ^
  dependencies\imgui\imgui_impl_dx10.cpp ^
  dependencies\imgui\imgui_impl_dx11.cpp ^
  src\runtime\framework\backend\Kiero.cpp ^
  src\runtime\framework\backend\UIBackend.cpp ^
  src\runtime\game\import\PointerFunctions.cpp ^
  src\runtime\game\structures\PointerWrapper.cpp ^
  src\runtime\game\structures\DictionaryBuilder.cpp ^
  src\runtime\game\structures\Stacktrace.cpp ^
  src\runtime\game\Functions.cpp ^
  src\runtime\game\websocket\MessageBuilder.cpp ^
  src\runtime\game\websocket\WebsocketCore.cpp ^
  src\runtime\game\websocket\AccountCommands.cpp ^
  src\runtime\game\websocket\ClanCommands.cpp ^
  src\runtime\game\websocket\SquadCommands.cpp ^
  src\runtime\util\HookingUtil.cpp ^
  src\all-in-one_joiner.cpp ^
  build\mh_buffer.o build\mh_hook.o build\mh_trampoline.o build\mh_hde64.o ^
  -o build\gamer.dll %LIBS% || goto :err

echo [4/5] 编译注入器...
"%GCC_PATH%\g++.exe" -std=c++17 -m64 -DPROJECT_NAME=\"%PROJECT_NAME%\" -static-libgcc -static-libstdc++ -static -s src\injector.cpp -o build\injector.exe || goto :err

echo [5/5] 复制文件到 release 目录...
copy build\gamer.dll release\ >nul
copy build\injector.exe release\ >nul

REM 创建启动脚本
echo @echo off > release\启动.bat
echo echo 正在启动游戏工具... >> release\启动.bat
echo echo. >> release\启动.bat
echo echo 按 F1/Insert/右Ctrl/右Alt 打开菜单 >> release\启动.bat
echo echo. >> release\启动.bat
echo cd /d "%%~dp0" >> release\启动.bat
echo injector.exe >> release\启动.bat
echo pause >> release\启动.bat

echo.
echo ========================================
echo 🎉 打包完成！
echo ========================================
echo.
echo 📦 生成的文件：
echo   ├── release\injector.exe    # 注入器
echo   ├── release\gamer.dll       # 游戏工具DLL（完整版）
echo   └── release\启动.bat        # 快速启动脚本
echo.
echo 🚀 使用方法：
echo   方式1: 双击 release\启动.bat
echo   方式2: cd release ^&^& injector.exe
echo.
echo 📊 文件大小：
for %%F in (release\injector.exe release\gamer.dll) do (
    echo   %%~nxF: %%~zF bytes
)
echo.
echo ✨ 功能特性：
echo   ✓ ImGui 图形界面（F1/Insert/右Ctrl/右Alt 切换）
echo   ✓ 控制台菜单
echo   ✓ 完整 IL2CPP 框架
echo   ✓ WebSocket 框架
echo   ✓ DirectX 10/11 渲染支持
echo.
echo ========================================

pause
exit /b 0

:err
echo.
echo [❌ 错误] 打包失败
echo.
pause
exit /b 1
