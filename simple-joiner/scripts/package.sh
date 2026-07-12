#!/bin/bash
# ========================================
# 一键打包脚本 - 生成 injector.exe + gamer.dll
# ========================================

echo ""
echo "========================================"
echo "   一键打包脚本"
echo "========================================"
echo ""

# 设置编译器路径
GCC_PATH="D:/program/msys64/ucrt64/bin"
PROJECT_NAME="nazimod"

echo "[1/4] 清理并创建输出目录..."
rm -rf release
mkdir -p release

echo "[2/4] 编译注入器..."
"${GCC_PATH}/g++.exe" -std=c++17 -m64 -DPROJECT_NAME=${PROJECT_NAME} -static-libgcc -static-libstdc++ -static -s src/injector.cpp -o release/injector.exe 2>&1

if [ $? -ne 0 ]; then
    echo "[错误] 注入器编译失败"
    read -p "按回车键退出..."
    exit 1
fi

echo "[3/4] 编译并打包DLL..."
"${GCC_PATH}/g++.exe" -std=c++17 -shared -m64 -DPROJECT_NAME=${PROJECT_NAME} -static-libgcc -static-libstdc++ -static -s src/all-in-one_joiner.cpp -o release/gamer.dll 2>&1

if [ $? -ne 0 ]; then
    echo "[错误] DLL编译失败"
    read -p "按回车键退出..."
    exit 1
fi

echo "[4/4] 创建启动脚本..."
cat > release/启动.bat << 'EOF'
@echo off
echo 正在启动游戏工具...
echo.
cd /d "%~dp0"
injector.exe
pause
EOF

echo ""
echo "========================================"
echo "🎉 打包完成！"
echo "========================================"
echo ""
echo "📦 生成的文件："
echo "  ├── injector.exe     # 注入器"
echo "  ├── gamer.dll        # 游戏工具DLL (自动检测)"
echo "  └── 启动.bat         # 快速启动脚本"
echo ""
echo "🚀 使用方法："
echo "  方式1: 双击 启动.bat"
echo "  方式2: 运行 injector.exe"
echo ""
echo "📊 文件大小："
ls -lh release/
echo ""
echo "💡 提示："
echo "  - 直接运行 injector.exe 即可自动检测 gamer.dll"
echo "  - 中文显示已修复"
echo "  - 支持绝对路径和相对路径"
echo ""
echo "========================================"
echo ""

read -p "按回车键退出..."
