@echo off
REM ====================================================================
REM  gamer.dll build (real WebSocket framework version)
REM  Output is named gamer.dll so the injector finds it automatically.
REM  Compiler: g++ (MSYS2 UCRT64)
REM  Stage 1: MinHook (.c, c17)        - hook engine (replaces MS Detours)
REM  Stage 2: IL2CPP + game layer (.cpp, c++20) -> gamer.dll
REM  Stage 3: injector.exe (c++17)
REM ====================================================================
setlocal
set GCC=D:\program\msys64\ucrt64\bin\g++.exe
set GCC_C=D:\program\msys64\ucrt64\bin\gcc.exe
set ROOT=%~dp0..
pushd "%ROOT%"
set OUT=build
if not exist "%OUT%" mkdir "%OUT%"

echo === Stage 1: MinHook (c17) ===
set MHI=-Idependencies\minhook\include -Idependencies\minhook\src -Idependencies\minhook\src\hde
"%GCC_C%" -std=c17 -O2 %MHI% -c dependencies\minhook\src\buffer.c      -o %OUT%\mh_buffer.o     || goto :err
"%GCC_C%" -std=c17 -O2 %MHI% -c dependencies\minhook\src\hook.c        -o %OUT%\mh_hook.o       || goto :err
"%GCC_C%" -std=c17 -O2 %MHI% -c dependencies\minhook\src\trampoline.c  -o %OUT%\mh_trampoline.o || goto :err
"%GCC_C%" -std=c17 -O2 %MHI% -c dependencies\minhook\src\hde\hde64.c   -o %OUT%\mh_hde64.o      || goto :err

echo === Stage 2: IL2CPP + game layer + main (c++20) -^> gamer.dll ===
set INC=-Idependencies\il2cpp -Idependencies -Idependencies\logger -Idependencies\minhook\include -Idependencies\minhook\src -Idependencies\minhook\src\hde -Isrc\runtime
set DEF=-DLOGGER_DEFINE_MACRO -D_USE_MATH_DEFINES
set LIBS=-lpsapi -static-libgcc -static-libstdc++ -static -s

"%GCC%" -std=c++20 -shared -O2 %INC% %DEF% ^
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
  src\all-in-one_joiner.cpp ^
  %OUT%\mh_buffer.o %OUT%\mh_hook.o %OUT%\mh_trampoline.o %OUT%\mh_hde64.o ^
  -o %OUT%\gamer.dll %LIBS% || goto :err

echo === Stage 3: injector (c++17) -^> exe ===
"%GCC%" -std=c++17 -m64 -DPROJECT_NAME=\"nazimod\" -static-libgcc -static-libstdc++ -static -s src\injector.cpp -o %OUT%\injector.exe || goto :err

echo.
echo ========================================
echo  Build OK
echo  %OUT%\gamer.dll        - the mod (auto-found by injector)
echo  %OUT%\injector.exe     - the injector
echo ========================================
for %%F in (%OUT%\gamer.dll) do echo  gamer.dll: %%~zF bytes
for %%F in (%OUT%\injector.exe) do echo  injector.exe: %%~zF bytes
echo  Exports: Initialize / UnlockAllWeapons / ReloadProgress
echo  Run: start game, then double-click injector.exe (it auto-finds gamer.dll)
echo  Press F1/Insert/Right Ctrl/Right Alt to toggle menu
popd
exit /b 0

:err
echo.
echo [ERROR] Build failed
popd
exit /b 1
