# 🎯 最终使用指南 - 游戏进程识别与偏移更新详解

## 📊 系统完成状态

### ✅ 已构建的所有版本

```
simple-joiner/bin/
├── injector.exe (1.08 MB) - DLL注入器
├── enhanced_joiner.dll (1.09 MB) - 增强版房间加入器
├── standalone_joiner.dll (1.08 MB) - 独立版
├── ultimate_joiner.dll (1.09 MB) - 终极版（货币+等级）
└── websocket_joiner.dll (1.15 MB) - WebSocket版
```

## 🔍 游戏进程识别详解

### 1. 识别的目标进程

**Pixel Gun 3D** 游戏进程信息：

#### Windows版进程特征：
- **进程名称**: `PixelGun3D.exe` 或 `PG3D.exe`
- **窗口标题**: `"Pixel Gun 3D"`
- **架构**: x64 (64位)
- **引擎**: Unity + IL2CPP

### 2. 识别机制详解

我们的代码使用**双重识别机制**：

#### 方法1: 窗口标题识别
```cpp
// 在 ultimate_joiner.cpp 和 websocket_joiner.cpp 中
HWND hwnd = FindWindowA(nullptr, "Pixel Gun 3D");
if (hwnd) {
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    // 成功识别到游戏进程
}
```

**优点**:
- ✅ 稳定：游戏更新时窗口标题通常不变
- ✅ 快速：直接查找，无需枚举

**缺点**:
- ❌ 游戏必须在前台运行
- ❌ 窗口标题可能被修改

#### 方法2: 模块枚举识别
```cpp
// 枚举当前进程的所有模块
EnumProcessModules(GetCurrentProcess(), ...);

// 查找包含Unity特征的模块
if (moduleName.find("Unity") != std::string::npos ||
    moduleName.find("Game") != std::string::npos ||
    moduleName.find("Assembly") != std::string::npos ||
    moduleName.find("PG3D") != std::string::npos) {
    // 找到游戏模块
}
```

**Unity模块特征**:
- `UnityPlayer.dll` - Unity引擎核心
- `GameAssembly.dll` - IL2CPP编译的游戏代码
- `Assembly-CSharp.dll` - 游戏逻辑代码

### 3. 进程识别的稳定性

| 更新类型 | 窗口标题 | 进程名 | Unity模块 | 函数偏移 |
|---------|---------|--------|-----------|---------|
| 小版本更新 | ✅ 不变 | ✅ 不变 | ✅ 不变 | ❌ 可能变化 |
| 大版本更新 | ✅ 通常不变 | ✅ 不变 | ✅ 不变 | ❌ 大概率变化 |
| 引擎升级 | ❌ 可能变化 | ❌ 可能变化 | ❌ 可能变化 | ❌ 必然变化 |

## 🔄 偏移更新完全指南

### 游戏更新后如何更新偏移？

#### 🔧 第一步：识别更新

**症状检查**:
```
1. DLL注入成功，但功能不工作
2. 控制台显示"函数未找到"
3. 游戏崩溃或异常
4. 功能调用无效果
```

**确认游戏版本**:
```bash
# 方法1: 查看游戏属性
# 右键游戏快捷方式 → 属性 → 详细信息

# 方法2: 游戏内查看
# 设置 → 关于 → 版本号

# 方法3: 查看游戏文件
# 游戏目录/version.txt 或类似文件
```

#### 🔧 第二步：提取新的偏移信息

##### 方法A: 使用 Il2CppDumper (推荐)

**1. 下载工具**:
```bash
https://github.com/Perfare/Il2CppDumper/releases
```

**2. 找到游戏文件**:
```
游戏目录/
├── GameAssembly.dll        # IL2CPP编译的游戏代码
├── global-metadata.dat      # IL2CPP元数据
└── UnityPlayer.dll         # Unity引擎
```

**3. 运行 Il2CppDumper**:
```bash
Il2CppDumper.exe GameAssembly.dll global-metadata.dat output_dir
```

**4. 生成结果**:
```
output_dir/
├── dump.cs              # 所有类和方法的偏移地址 ⭐
├── script.json          # JSON格式的偏移信息
├── generate.cs          # 用于生成DLL桩代码
└── dummy.dll            # 伪DLL（用于反编译）
```

##### 方法B: 使用 assembly-csharp.dll

**1. 反编译工具**:
```bash
https://github.com/ICSharpCode/ILSpy/releases
```

**2. 反编译**:
```bash
ILSpy.exe assembly-csharp.dll
```

**3. 查找方法**:
```csharp
// 在ILSpy中导航
// 命名空间: 你的游戏命名空间
// 类: ProgressUpdater
// 方法: AddCurrency
```

**4. 分析代码逻辑**:
```csharp
// 原始代码
public void AddCurrency(string curName, int num, int num2, bool b1, bool b2, AnalyticsParamater params)
{
    // 这个方法如何被调用
    // 参数的含义是什么
    // 返回值是什么
}
```

##### 方法C: 使用特征码扫描（自动化）

**创建特征码**:
```cpp
// 1. 使用调试器找到函数地址
// 2. 查看函数开头的机器码
// 3. 转换为特征码模式

// 例如函数开头：
// 48 83 EC 28     sub rsp, 0x28
// 48 8B 05 ?? ?? ?? ??  mov rax, [rip+??]
// 48 85 C0        test rax, rax

// 转换为特征码：
std::string pattern = "48 83 EC 28 48 8B 05 ?? ?? ?? ?? 48 85 C0";
```

**在代码中使用**:
```cpp
// 在 InitializeGameFunctions() 中
uintptr_t funcAddr = ScanPattern(g_moduleInfo.baseAddress, g_moduleInfo.size, pattern);
if (funcAddr) {
    g_functions.AddCurrency = reinterpret_cast<FuncType>(funcAddr);
    std::cout << "自动找到AddCurrency: 0x" << std::hex << funcAddr << std::endl;
}
```

#### 🔧 第三步：更新代码中的偏移

**在 ultimate_joiner.cpp 或 websocket_joiner.cpp 中**:

```cpp
void InitializeGameFunctions() {
    uintptr_t base = g_moduleInfo.baseAddress;

    // 方法1: 使用固定偏移（从dump.cs获取）
    g_functions.AddCurrency = reinterpret_cast<void(*)(...)>(
        base + 0x12345678  // 从dump.cs获取的偏移
    );

    // 方法2: 使用特征码扫描
    std::string pattern = "48 83 EC 28 48 8B 05 ?? ?? ?? ??";
    uintptr_t addr = ScanPattern(base, g_moduleInfo.size, pattern);
    if (addr) {
        g_functions.AddExperience = reinterpret_cast<void(*)(...)>(addr);
    }

    g_functions.initialized = true;
}
```

#### 🔧 第四步：重新编译和测试

```bash
# 重新编译
cd simple-joiner
build_ultimate.bat

# 测试
bin\injector.exe bin\ultimate_joiner.dll PixelGun3D.exe
```

## 📋 具体功能偏移查找方法

### 1. ProgressUpdater::AddCurrency

**在dump.cs中搜索**:
```csharp
// 搜索 "ProgressUpdater" 或 "AddCurrency"
public class ProgressUpdater
{
    // 0xOFFSET_HERE - AddCurrency
    // RVA: 0xOFFSET_HERE (相对虚拟地址)
    public void AddCurrency(String curName, Int32 num, Int32 num2, Boolean b1, Boolean b2, AnalyticsParamater params)
}

// 转换为我们的代码
g_functions.AddCurrency = reinterpret_cast<void(*)(...)>(
    base + 0xOFFSET_HERE
);
```

### 2. ExperienceController::AddExperience

**在dump.cs中搜索**:
```csharp
public class ExperienceController
{
    // 0xOFFSET_HERE - AddExperience
    public void AddExperience(Int32 value, ExpObtainCause reason, Dictionary`2<String,Object> dictionary, Object sex)
}
```

### 3. ModuleData::GetLevel

**在dump.cs中搜索**:
```csharp
public class ModuleData
{
    // 0xOFFSET_HERE - get_Level
    public Int32 get_Level()
}
```

### 4. WebSocket相关函数

**在dump.cs中搜索**:
```csharp
public class WebSocketManager
{
    // 0xOFFSET_HERE - Send
    // 0xOFFSET_HERE - SendAsync
    // 0xOFFSET_HERE - SendEvent
}
```

## 🛠️ 实战示例：游戏更新后的完整流程

### 场景：游戏从 v1.0 更新到 v1.1

#### 第1步：确认问题
```bash
# 注入DLL后测试
injector.exe ultimate_joiner.dll PixelGun3D.exe

# 发现：
# - [错误] ProgressUpdater函数未找到
# - [错误] ExperienceController函数未找到
# 结论：偏移需要更新
```

#### 第2步：提取新偏移
```bash
# 1. 备份旧的游戏文件（如果需要）
# 2. 从游戏目录提取：
#    GameAssembly.dll
#    global-metadata.dat

# 3. 运行 Il2CppDumper
Il2CppDumper.exe GameAssembly.dll global-metadata.dat ./v1.1_dump
```

#### 第3步：查找新偏移
```bash
# 在 v1.1_dump/dump.cs 中搜索：
# ProgressUpdater::AddCurrency
# ExperienceController::AddExperience
# ModuleData::GetLevel
```

#### 第4步：记录偏移变化
```
函数名              v1.0偏移      v1.1偏移      变化
AddCurrency         0x12345678    0x12345680    +0x8
AddExperience       0x87654321    0x87654325    +0x4
GetLevel            0x11112222    0x11112226    +0x4
```

#### 第5步：更新代码
```cpp
// 在 ultimate_joiner.cpp 中
void InitializeGameFunctions() {
    uintptr_t base = g_moduleInfo.baseAddress;

    // 更新为新的偏移
    g_functions.AddCurrency = reinterpret_cast<void(*)(...)>(base + 0x12345680);
    g_functions.AddExperience = reinterpret_cast<void(*)(...)>(base + 0x87654325);
    g_functions.GetLevel = reinterpret_cast<int(*)(void*)>(base + 0x11112226);

    g_functions.initialized = true;
}
```

#### 第6步：重新编译测试
```bash
build_ultimate.bat

# 测试
injector.exe ultimate_joiner.dll PixelGun3D.exe

# 验证功能
# [成功] 已添加金币: 1000
# [成功] 已添加经验: 500
```

## 🎯 WebSocket功能详解

### WebSocket通信原理

**游戏中的WebSocket**:
```
游戏客户端 ←→ WebSocket服务器 ←→ 数据库
     ↓                                ↓
  本地修改                       服务器同步
```

### 两种修改方式对比

| 方式 | 优点 | 缺点 | 可靠性 |
|------|------|------|--------|
| **直接函数调用** | 立即生效 | 仅本地可见 | 中等 |
| **WebSocket消息** | 服务器同步 | 可能被检测 | 低 |

### WebSocket使用场景

```cpp
// 1. 添加货币（直接函数调用 - 本地）
AddCurrencyDirect("Coins", 1000);

// 2. 添加货币（WebSocket - 服务器）
SendGameEvent("AddCurrency", {{"currency", "Coins"}, {"amount", "1000"}});

// 3. 添加经验（直接函数调用）
AddExperience(500);

// 4. 自定义WebSocket消息
SendWebSocketMessage("CustomEvent", {{"key1", "value1"}, {"key2", "value2"}});
```

## ⚡ 自动化偏移更新

### 创建偏移配置文件

**offsets_config.json**:
```json
{
    "game_version": "1.1.0",
    "last_updated": "2026-07-12",
    "base_address_note": "动态获取，每次注入时重新计算",
    "offsets": {
        "ProgressUpdater.AddCurrency": "0x12345680",
        "ExperienceController.AddExperience": "0x87654325",
        "ModuleData.GetLevel": "0x11112226",
        "ProgressUpdater.GetInstance": "0x33334444",
        "ExperienceController.Get": "0x55556666"
    }
}
```

### 自动化更新代码

```cpp
// 可以添加到 ultimate_joiner.cpp 中
void LoadOffsetsFromJSON() {
    // 读取 offsets_config.json
    // 解析JSON
    // 自动更新 g_functions 中的所有偏移

    std::cout << "[自动化] 从JSON加载偏移配置..." << std::endl;
    // ... JSON解析代码
}
```

## 🎊 最终总结

### ✅ 完整的游戏工具系统

你现在拥有**5个完整的DLL版本**：

1. **injector.exe** - 通用DLL注入器
2. **enhanced_joiner.dll** - 房间加入功能
3. **standalone_joiner.dll** - 独立基础版
4. **ultimate_joiner.dll** - 终极版（货币+等级+房间）⭐
5. **websocket_joiner.dll** - WebSocket完整版

### 🎯 游戏进程识别

- **目标**: Pixel Gun 3D (`PixelGun3D.exe`)
- **方法**: 窗口标题 + Unity模块枚举
- **稳定性**: 游戏更新时通常保持稳定
- **基址**: 每次运行时动态获取

### 🔄 偏移更新机制

- **工具**: Il2CppDumper + ILSpy
- **输入**: GameAssembly.dll + global-metadata.dat
- **输出**: dump.cs（包含所有偏移）
- **频率**: 游戏大更新后需要更新
- **自动化**: 可以使用特征码扫描部分自动化

### 🚀 立即使用

```bash
# 使用终极版（推荐）
cd simple-joiner
bin\injector.exe bin\ultimate_joiner.dll PixelGun3D.exe

# 功能菜单：
# 1. 💰 添加金币
# 2. 💎 添加宝石
# 3. ⬆️  设置等级
# 4. 📊 查看角色信息
# 5. 🏠 加入房间
# 6. ℹ️  显示游戏信息
```

### 📚 详细文档

- [OFFSET_UPDATE_GUIDE.md](OFFSET_UPDATE_GUIDE.md) - 详细偏移更新指南
- [USAGE_GUIDE.md](USAGE_GUIDE.md) - 使用指南
- [README.md](README.md) - 项目总览

---

## 🎮 祝你游戏愉快！

记住：
- ⚠️ 仅用于学习和个人使用
- ⚠️ 遵守游戏服务条款
- ⚠️ 游戏更新后需要更新偏移
- ⚠️ 建议在测试环境中使用

🎊 **完整的游戏工具系统已经准备就绪！** 🎊