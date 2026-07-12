@echo off
REM 测试注入是否真的工作

echo === 备份原始 gamer.dll ===
copy build\gamer.dll build\gamer.dll.backup

echo === 使用 test.dll 替换 gamer.dll ===
copy build\test.dll build\gamer.dll

echo.
echo 现在运行 injector.exe
echo 如果看到 "测试DLL - 注入成功！" 窗口，说明注入工作正常
echo 如果还是没反应，说明注入器有问题
echo.
pause

echo === 恢复原始 gamer.dll ===
copy build\gamer.dll.backup build\gamer.dll

echo === 检查日志文件 ===
if exist D:\test_injection.log (
    echo 找到日志文件！内容：
    type D:\test_injection.log
) else (
    echo 没有找到日志文件 D:\test_injection.log
)

echo.
echo === 测试完成 ===
pause