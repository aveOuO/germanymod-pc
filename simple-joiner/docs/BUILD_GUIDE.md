# Simple Room Joiner - 构建指南

## 概述

这里有三个不同复杂程度的版本，从完全独立到使用原项目依赖：

### 版本对比

| 版本 | 文件 | 依赖 | 复杂度 | 功能完整度 |
|------|------|------|--------|------------|
| 完整版 | simple_joiner.cpp | 需要原项目依赖 | 高 | 100% |
| 独立版 | standalone_joiner.cpp | 无外部依赖 | 低 | 模板 |
| 原版整合 | 原项目git diff | 原项目完整构建 | 最高 | 100% |

## 推荐构建流程

### 步骤1: 选择版本

**如果你想要完整功能且能解决依赖问题：**
```bash
cd simple-joiner
build.bat
```

**如果你想要最简单的开始：**
```bash
cd simple-joiner
build_standalone.bat
```

### 步骤2: 环境准备

确保MSYS2路径正确（默认是 `D:\program\msys64\ucrt64\bin`）

编辑 `.bat` 文件修改路径：
```batch
set GCC_PATH=你的MSYS2路径\ucrt64\bin
```

### 步骤3: 构建和测试

**独立版（推荐开始）：**
```bash
cd simple-joiner
build_standalone.bat
```

这会生成 `bin/standalone_joiner.dll`

**使用方法：**
1. 使用任意注入器将DLL注入到游戏进程
2. 控制台窗口会自动出现
3. 按照屏幕提示操作

## 故障排除

### 编译错误 "gcc.exe not found"

**解决方案：**
```bash
# 1. 检查MSYS2安装
ls "D:\program\msys64\ucrt64\bin\gcc.exe"

# 2. 如果不存在，安装MinGW-w64编译器
# 在MSYS2终端中：
pacman -S mingw-w64-ucrt-x86_64-gcc
```

### 编译错误 "undefined reference"

**独立版不应该有此错误，如果出现：**
- 检查是否误用了完整版的构建脚本
- 确保使用 `build_standalone.bat`

### 运行时错误 "DLL failed to load"

**可能原因：**
1. 游戏进程架构不匹配（需要x64）
2. 缺少依赖DLL（独立版不应该有此问题）
3. 注入器问题

## 从独立版到完整版的升级路径

### 第一步：获取函数地址

使用独立版中的内存扫描功能找到关键函数地址：

```cpp
// 在standalone_joiner.cpp中添加
static void FindGameFunctions()
{
    uintptr_t base = GetUnityModuleBase();

    // 扫描PhotonNetwork.JoinRoom函数
    // 这需要你根据游戏版本调整模式
    uintptr_t joinRoomAddr = ScanPattern(base, 0x1000000,
        "48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 30");

    if (joinRoomAddr)
    {
        std::cout << "Found JoinRoom at: 0x" << std::hex << joinRoomAddr << std::endl;
    }
}
```

### 第二步：实现JoinRoom调用

```cpp
void CallJoinRoom(const std::string& roomName)
{
    // 这里需要具体的函数调用代码
    // 取决于找到的函数地址和调用约定
}
```

### 第三步：添加ImGui界面

当基础功能工作后，可以添加ImGui界面。

## 与原项目的集成

如果你想使用原项目的完整功能：

1. **使用原构建系统：**
```bash
cd germanymod-pc
# 需要Visual Studio或解决MSYS2问题
mkdir build && cd build
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
ninja
```

2. **或直接使用git diff：**
```bash
# 查看你的更改
git diff src/runtime/game/GameplayMain.cpp
git diff src/runtime/game/Menu.cpp

# 这些更改已经包含了完整的JoinRoom功能
```

## 核心功能说明

基于你的git变更，核心功能是：

### JoinRoomByName函数（GameplayMain.cpp）
```cpp
void JoinRoomByName(const std::string& roomName)
{
    // 1. 获取PhotonNetwork类
    auto photonNetworkClass = IL2CPP::ClassMapping::GetClass("PhotonNetwork");

    // 2. 找到JoinRoom方法
    IL2CPP::MethodInfo* joinMethod = photonNetworkClass->GetMethod("JoinRoom", paramCount);

    // 3. 创建参数
    auto args = IL2CPP::Array<IL2CPP::Object*>::Create(...);
    args->Set(0, IL2CPP::String::Create(roomName));

    // 4. 调用方法
    MethodBase::Invoke(joinMethod, nullptr, args);
}
```

### ImGui界面（Menu.cpp）
```cpp
// 房间名输入框
StringInput RoomName(&GROUP, "Room Name", "");

// Join按钮
Button JoinRoom(&GROUP, "Join Room");

// 按钮点击事件
JoinRoom.OnClick([&] {
    Global::ExecuteOnGameThread([room]() {
        GameplayMain::JoinRoomByName(room);
    });
});
```

## 最终建议

1. **首先尝试独立版** - 验证基本构建环境
2. **然后使用原项目的更改** - 如果需要完整IL2CPP功能
3. **或者开发混合方案** - 独立版框架 + 特定游戏函数地址

独立版提供了模板，你可以根据具体游戏版本填充函数地址和调用逻辑。