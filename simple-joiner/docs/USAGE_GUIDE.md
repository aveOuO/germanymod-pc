# 完整注入系统使用指南

## 🎉 构建成功！

恭喜！完整的注入系统已经成功构建：

### 生成的文件
- **bin/injector.exe** (1.08 MB) - DLL注入器
- **bin/enhanced_joiner.dll** (1.09 MB) - 增强版房间加入器
- **bin/standalone_joiner.dll** (1.08 MB) - 独立版（备用）

## 🚀 使用方法

### 方式1: 命令行注入（推荐）

```bash
# 格式: injector.exe <dll路径> <进程名>
injector.exe enhanced_joiner.dll game.exe

# 示例（假设游戏进程是 PixelGun3D.exe）
injector.exe enhanced_joiner.dll PixelGun3D.exe
```

### 方式2: 交互式注入

```bash
# 直接运行注入器
injector.exe

# 按提示操作：
# 1. 输入DLL路径: enhanced_joiner.dll
# 2. 选择操作1（输入进程名注入）
# 3. 输入游戏进程名
```

### 方式3: 单参数模式

```bash
injector.exe enhanced_joiner.dll
# 然后输入目标进程名
```

## 🎯 增强版DLL功能

### 核心功能（基于你的Git变更）

`enhanced_joiner.dll` 包含了原项目的核心功能：

#### 1. JoinRoomByName 功能
```cpp
void JoinRoomByName(const std::string& roomName)
```
- 通过房间名加入游戏房间
- 基于原项目 `GameplayMain.cpp` 中的实现
- 支持 PhotonNetwork.JoinRoom 调用

#### 2. 游戏函数指针
包含原项目 `PointerFunctions.hpp` 中的重要函数：
- `PhotonNetwork::GetCurrentRoom`
- `PhotonNetwork::GetLocalPlayer`
- `PhotonNetwork::IsMasterClient`
- `AuthManager::GetPlayerID`
- `NetworkStartTable::IsInMatch`
- 等等...

#### 3. 自动模块识别
- 自动检测Unity游戏模块
- 获取模块基址和大小
- 支持多种游戏进程名

## 📋 控制台界面

注入成功后，DLL会自动打开控制台界面：

```
=== 增强版房间加入器 ===
1. 加入房间
2. 显示游戏信息
3. 测试连接
4. 退出
=====================
选择:
```

### 功能说明

**1. 加入房间**
- 输入房间名
- DLL会调用游戏函数加入房间
- 显示操作结果

**2. 显示游戏信息**
- 显示游戏模块信息
- 显示函数初始化状态
- 显示模块基址和大小

**3. 测试连接**
- 验证游戏模块是否正确识别
- 显示游戏基址

## 🔍 技术细节

### 游戏函数指针结构

```cpp
struct GameFunctions {
    // PhotonNetwork相关
    void* (*GetCurrentRoom)() = nullptr;
    void* (*GetLocalPlayer)() = nullptr;
    bool (*IsMasterClient)() = nullptr;
    void (*SetMasterClient)(void* photonPlayer) = nullptr;
    void (*Destroy)(void* object) = nullptr;

    // 玩家相关
    void* (*GetPlayerID)() = nullptr;
    bool (*IsInMatch)() = nullptr;

    // 房间信息
    void* (*GetRoomHashtable)(void* room) = nullptr;

    bool initialized = false;
};
```

### 模块识别逻辑

DLL会自动查找包含以下关键字的模块：
- `Unity`
- `Game`
- `Assembly`
- `mono`

### 内存扫描功能

包含特征码扫描功能，可以用于查找游戏函数：
```cpp
uintptr_t ScanPattern(uintptr_t startAddr, size_t size, const std::string& pattern)
```

## 🛠️ 开发和自定义

### 添加新的游戏函数

在 `enhanced_joiner.cpp` 的 `InitializeGameFunctions()` 中添加：

```cpp
// 示例：添加自定义函数扫描
std::string customPattern = "48 83 EC 28 48 8B 05 ?? ?? ?? ??";
uintptr_t funcAddr = ScanPattern(g_moduleInfo.baseAddress, g_moduleInfo.size, customPattern);
if (funcAddr) {
    g_functions.YourFunction = reinterpret_cast<YourFunctionType>(funcAddr);
    std::cout << "[增强版] 找到自定义函数: 0x" << std::hex << funcAddr << std::endl;
}
```

### 修改房间加入逻辑

编辑 `JoinRoomByName()` 函数：

```cpp
void JoinRoomByName(const std::string& roomName) {
    // 方式1: 使用IL2CPP接口（如果游戏支持）
    // 方式2: 直接调用游戏函数
    // 方式3: 发送网络消息

    // 根据你的游戏选择合适的方式
}
```

## 🔧 故障排除

### 注入失败

**问题**: `[错误] 找不到进程`
- **解决**: 确认游戏进程名称正确
- **提示**: 使用选项2查看可用进程列表

**问题**: `[错误] 无法打开进程，需要管理员权限`
- **解决**: 以管理员身份运行注入器
- **方法**: 右键injector.exe → 以管理员身份运行

**问题**: `[错误] 内存分配失败`
- **解决**: 检查游戏是否在运行，DLL路径是否正确

### DLL功能不工作

**问题**: 控制台没有出现
- **解决**: 检查DLL是否正确加载，查看注入器输出

**问题**: `[警告] 未找到游戏模块`
- **解决**: 游戏可能不是Unity引擎，或模块名称不匹配

**问题**: JoinRoom功能无效
- **解决**: 需要根据具体游戏实现函数调用逻辑

## 📊 与原项目对比

| 功能 | 原项目完整版 | 增强版 |
|------|-------------|--------|
| **构建方式** | MSVC + CMake | 单个g++命令 |
| **注入方式** | nazimod-loader.exe | injector.exe |
| **DLL大小** | ~5MB+ | ~1MB |
| **界面** | ImGui 图形界面 | 控制台界面 |
| **函数支持** | 完整IL2CPP集成 | 核心函数指针 |
| **编译时间** | 几分钟 | 几秒钟 |
| **依赖项** | 复杂（ImGui, Logger等） | 无外部依赖 |

## 🎯 推荐使用场景

**使用增强版当：**
- ✅ 想要完整的注入+功能解决方案
- ✅ 需要独立于原项目构建
- ✅ 希望快速测试和部署
- ✅ 想要控制台界面

**使用原版当：**
- ✅ 需要ImGui图形界面
- ✅ 需要完整的IL2CPP支持
- ✅ 项目能正常构建
- ✅ 需要所有原功能

## 📝 快速命令参考

### 重新构建（如需修改代码）

```bash
cd simple-joiner
export PATH="/d/program/msys64/ucrt64/bin:$PATH"

# 构建增强版DLL
g++ -std=c++17 -shared -m64 -DPROJECT_NAME=\"nazimod\" \
    -static-libgcc -static-libstdc++ -static -s \
    enhanced_joiner.cpp -o bin/enhanced_joiner.dll

# 构建注入器
g++ -std=c++17 -m64 -DPROJECT_NAME=\"nazimod\" \
    -static-libgcc -static-libstdc++ -static -s \
    injector.cpp -o bin/injector.exe
```

### 快速测试

```bash
# 1. 启动游戏
# 2. 注入DLL
./bin/injector.exe bin/enhanced_joiner.dll YourGame.exe

# 3. 在控制台中选择操作
# 4. 输入房间名进行测试
```

## 🎊 恭喜！

你现在拥有了一个：
- ✅ 完全独立的注入系统
- ✅ 基于你Git变更的核心功能
- ✅ 无需Visual Studio编译
- ✅ 包含游戏函数指针支持
- ✅ 自动模块识别
- ✅ 控制台交互界面

**下一步**: 根据具体游戏调整JoinRoom实现细节！

---

## 🔗 相关文件

- [QUICKSTART.md](QUICKSTART.md) - 快速开始指南
- [BUILD_GUIDE.md](BUILD_GUIDE.md) - 详细构建说明
- [injector.cpp](injector.cpp) - 注入器源码
- [enhanced_joiner.cpp](enhanced_joiner.cpp) - 增强版DLL源码
- [build_all.bat](build_all.bat) - 一键构建脚本

**问题反馈**: 如果遇到问题，检查构建输出和游戏进程名称。