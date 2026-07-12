# 🎯 DLL注入方式详解

## ❓ 常见问题解答

### Q1: DLL必须叫 all-in-one_joiner.dll 吗？
**A: 不是！** 可以叫任何名字，完全自由。

### Q2: 必须把exe和dll放同目录吗？
**A: 不需要！** 可以放在不同位置。

### Q3: 必须双击exe才能注入吗？
**A: 不需要！** 有多种注入方式。

---

## 🎯 DLL命名完全自由

### 你可以命名为任何名字：

```
工具类名称：
├── game_tools.dll          # 游戏工具
├── pg3d_tools.dll          # Pixel Gun 3D 工具
├── my_mod.dll              # 我的模组
├── hack.dll                # 外挂（如果这样叫的话）
└── 随便什么名字.dll        # 完全自由
```

### 重命名方法：

**方法1：使用重命名脚本**
```bash
rename.bat
# 按提示输入新名称
```

**方法2：手动重命名**
```bash
# 直接在文件管理器中重命名
# 或使用命令
ren all-in-one_joiner.dll mytools.dll
```

**方法3：重新编译时改名**
```bash
# 修改编译命令
g++ ... all-in-one_joiner.cpp -o bin/my_custom_name.dll
```

---

## 🚀 注入方式详解

### 方式1：命令行注入（最灵活）⭐

#### 基本语法：
```bash
injector.exe <DLL路径> <游戏进程名>
```

#### 示例：

```bash
# 1. DLL在不同位置
C:\tools\injector.exe C:\mylibs\game_tools.dll PixelGun3D.exe

# 2. 使用相对路径
injector.exe ..\libs\tools.dll PixelGun3D.exe

# 3. DLL在同目录
injector.exe game_tools.dll PixelGun3D.exe

# 4. 使用绝对路径
injector.exe D:\Desktop\mytools.dll PixelGun3D.exe
```

#### 优点：
- ✅ DLL可以在任何位置
- ✅ injector可以在任何位置
- ✅ 支持绝对路径和相对路径
- ✅ 最灵活的方式

---

### 方式2：交互式注入

#### 使用方法：
```bash
# 直接运行injector（不带参数）
injector.exe
```

#### 然后按提示操作：
```
=== Simple DLL Injector ===
DLL路径: mytools.dll
目标进程: PixelGun3D.exe
```

#### 优点：
- ✅ 不需要记住命令格式
- ✅ 可以查看可用进程
- ✅ 交互式操作更安全

---

### 方式3：批处理脚本注入

#### 创建专用脚本：

**快速注入脚本**:
```batch
@echo off
REM 快速注入脚本

REM 设置路径（根据你的需要修改）
set INJECTOR=bin\injector.exe
set DLL=bin\mytools.dll
set GAME=PixelGun3D.exe

REM 执行注入
%INJECTOR% %DLL% %GAME%

pause
```

**高级注入脚本**:
```batch
@echo off
REM 高级注入脚本 - 支持路径配置

echo ========================================
echo   游戏工具 - 快速注入
echo ========================================
echo.

REM 配置区域（根据需要修改）
set INJECTOR_PATH=C:\tools\injector.exe
set DLL_PATH=C:\game_tools\my_custom_dll.dll
set GAME_PROCESS=PixelGun3D.exe

REM 检查文件
if not exist "%INJECTOR_PATH%" (
    echo [错误] 注入器不存在: %INJECTOR_PATH%
    pause
    exit /b 1
)

if not exist "%DLL_PATH%" (
    echo [错误] DLL不存在: %DLL_PATH%
    pause
    exit /b 1
)

echo [信息] 注入器: %INJECTOR_PATH%
echo [信息] DLL: %DLL_PATH%
echo [信息] 游戏进程: %GAME_PROCESS%
echo.

REM 执行注入
%INJECTOR_PATH% %DLL_PATH% %GAME_PROCESS%

if %ERRORLEVEL% EQU 0 (
    echo [成功] 注入完成！
) else (
    echo [失败] 注入失败，错误代码: %ERRORLEVEL%
)

pause
```

---

## 📂 文件组织方式

### 方案1：所有文件同目录（最简单）

```
游戏工具/
├── injector.exe
├── mytools.dll
└── start.bat          # 启动脚本
```

**使用方法：**
```bash
# 双击 start.bat 或直接运行
injector.exe mytools.dll PixelGun3D.exe
```

**优点：**
- ✅ 最简单，文件集中
- ✅ 不需要复杂路径
- ✅ 适合快速部署

---

### 方案2：DLL在独立目录

```
tools/
├── injector.exe
├── dlls/
│   └── mytools.dll
└── start.bat
```

**使用方法：**
```bash
injector.exe dlls\mytools.dll PixelGun3D.exe
```

**优点：**
- ✅ 文件组织更清晰
- ✅ 可以有多个DLL版本
- ✅ 便于管理

---

### 方案3：完全自定义位置

```
C:\tools\
├── injector.exe
└── game_mods\
    └── my_custom_dll.dll

D:\games\
└── PixelGun3D.exe
```

**使用方法：**
```bash
C:\tools\injector.exe C:\tools\game_mods\my_custom_dll.dll PixelGun3D.exe
```

**优点：**
- ✅ 最大灵活性
- ✅ 适合高级用户
- ✅ 便于集中管理

---

## 🎯 实用示例

### 场景1：桌面快捷方式

**创建快捷方式：**
```
目标：C:\tools\injector.exe C:\tools\mytools.dll PixelGun3D.exe
工作目录：C:\tools\
```

**或者使用批处理：**
```batch
@echo off
cd C:\tools\
injector.exe mytools.dll PixelGun3D.exe
```

### 场景2：游戏目录部署

**把文件复制到游戏目录：**
```
Pixel Gun 3D/
├── PixelGun3D.exe          # 游戏主程序
├── injector.exe             # 注入器
├── game_tools.dll           # 你的DLL
└── start.bat                # 启动脚本
```

**start.bat内容：**
```batch
@echo off
REM 等待游戏启动
timeout /t 5

REM 注入DLL
injector.exe game_tools.dll PixelGun3D.exe
```

### 场景3：USB便携部署

**USB上的结构：**
```
USB_DRIVE/
├── injector.exe
├── mytools.dll
└── run.bat
```

**run.bat内容：**
```batch
@echo off
REM 自动检测游戏进程并注入

REM 查找游戏进程
tasklist | find "PixelGun3D.exe"

if %ERRORLEVEL% EQU 0 (
    echo [信息] 游戏正在运行，开始注入...
    injector.exe mytools.dll PixelGun3D.exe
) else (
    echo [错误] 游戏未运行，请先启动游戏
    pause
)
```

---

## 🔧 注入器参数详解

### 完整参数格式：

```bash
injector.exe [DLL路径] [进程名]
```

### 参数说明：

1. **DLL路径**（可选）
   - 完整路径：`C:\path\to\mytools.dll`
   - 相对路径：`bin\mytools.dll`
   - 仅文件名：`mytools.dll`（同目录）

2. **进程名**（可选）
   - 完整进程名：`PixelGun3D.exe`
   - 不含扩展名：`PixelGun3D`

### 无参数模式：

```bash
# 直接运行进入交互模式
injector.exe

# 会提示输入：
# DLL路径: mytools.dll
# 进程名: PixelGun3D.exe
```

---

## 📋 快速参考

### 最常用的3种注入方式：

```bash
# 1. 命令行（最灵活）
injector.exe mytools.dll PixelGun3D.exe

# 2. 交互式（最安全）
injector.exe

# 3. 批处理（最方便）
start.bat
```

### DLL命名建议：

| 用途 | 建议名称 | 说明 |
|------|---------|------|
| 通用工具 | `game_tools.dll` | 清晰明了 |
| 游戏专用 | `pg3d_tools.dll` | 包含游戏名 |
| 隐蔽使用 | `system_helper.dll` | 看起来像系统文件 |
| 测试版本 | `test_tools.dll` | 测试用 |
| 正式版本 | `release_tools.dll` | 正式发布 |

---

## ⚡ 最佳实践建议

### 1. 文件组织
```
推荐结构：
tools/
├── injector.exe
├── dlls/
│   ├── v1.0.dll      # 不同版本
│   └── v2.0.dll
└── scripts/
    ├── inject_v1.bat
    └── inject_v2.bat
```

### 2. 命名规范
```
建议格式：<用途>_<版本>.dll
示例：
- pg3d_tools_v1.0.dll
- game_helper_v2.3.dll
- pixelgun_mod.dll
```

### 3. 路径处理
```bash
# 使用引号处理路径空格
injector.exe "C:\Program Files\my tools.dll" PixelGun3D.exe

# 使用相对路径简化命令
injector.exe ..\dlls\tools.dll PixelGun3D.exe
```

---

## 🎊 总结

### 回答你的问题：

**Q: DLL必须叫 all-in-one_joiner.dll 吗？**
**A: 不！** 可以叫任何名字：`mytools.dll`, `game.dll`, 等等

**Q: 必须exe和dll放同目录吗？**
**A: 不！** 可以在任何位置，支持绝对路径和相对路径

**Q: 必须双击exe才能注入吗？**
**A: 不！** 有命令行、交互式、批处理等多种方式

### 最简单的使用方式：

```bash
# 1. 重命名DLL（可选）
ren all-in-one_joiner.dll mytools.dll

# 2. 注入到游戏
injector.exe mytools.dll PixelGun3D.exe
```

### 最灵活的使用方式：

```bash
# DLL和injector可以在任何位置
C:\path\to\injector.exe D:\another\path\to\dll.dll PixelGun3D.exe
```

---

**完全自由！按照你的喜好组织文件吧！** 🎮