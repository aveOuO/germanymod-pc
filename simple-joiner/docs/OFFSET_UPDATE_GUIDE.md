# 游戏进程识别与偏移更新完全指南

## 🎯 游戏进程识别机制

### 1. 游戏进程是什么？

**Pixel Gun 3D** 是一个Unity引擎游戏，它的主进程通常是：
- **Windows版**: `PixelGun3D.exe` 或 `PG3D.exe`
- **进程特征**:
  - 窗口标题: "Pixel Gun 3D"
  - 包含Unity相关模块 (`UnityPlayer.dll`, `GameAssembly.dll` 等)
  - 包含IL2CPP运行时

### 2. 当前识别方法

我们的代码使用**两种方法**识别游戏进程：

#### 方法1: 窗口标题查找
```cpp
HWND hwnd = FindWindowA(nullptr, "Pixel Gun 3D");
if (hwnd) {
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    // 找到游戏进程
}
```

#### 方法2: 模块枚举
```cpp
EnumProcessModules(GetCurrentProcess(), ...);
// 查找包含 "Unity", "Game", "Assembly", "PG3D" 的模块
```

### 3. 游戏更新后如何识别？

游戏更新后，进程特征通常保持不变：
- ✅ **窗口标题**: 通常保持 "Pixel Gun 3D"
- ✅ **进程名称**: 通常保持 `PixelGun3D.exe`
- ✅ **Unity模块**: Unity引擎模块特征不变
- ❌ **函数偏移**: 可能会变化

## 🔄 游戏更新后的偏移更新

### 方法1: 使用dump.cs更新偏移

#### 什么是dump.cs？
`dump.cs` 是使用 **Il2CppDumper** 工具从IL2CPP游戏生成的C#文件，包含：
- 所有类的偏移地址
- 所有方法的偏移地址
- 所有字段的偏移地址

#### 生成dump.cs的步骤：

1. **下载工具**:
   ```
   https://github.com/Perfare/Il2CppDumper/releases
   ```

2. **提取游戏文件**:
   ```
   从游戏目录找到:
   - GameAssembly.dll
   - global-metadata.dat
   ```

3. **运行Il2CppDumper**:
   ```bash
   Il2CppDumper.exe GameAssembly.dll global-metadata.dat output_dir
   ```

4. **生成的文件**:
   - `dump.cs` - 所有类和方法的偏移
   - `script.json` - JSON格式的偏移信息
   - `generate.cs` - 用于生成DLL桩代码

#### 如何使用dump.cs更新偏移？

假设我们要找到 `AddCurrency` 函数的新偏移：

**步骤1**: 在dump.cs中搜索相关类
```csharp
// 在dump.cs中搜索
public class ProgressUpdater // 找到这个类
{
    // ... 其他字段和方法

    // 找到AddCurrency方法
    public void AddCurrency(String curName, Int32 num, Int32 num2, Boolean b1, Boolean b2, AnalyticsParamater params)
    {
        // 在这里会显示方法地址
    }
}
```

**步骤2**: 记录偏移地址
```cpp
// dump.cs会显示类似这样的信息：
// 0x12345678 - ProgressUpdater.AddCurrency

// 在我们的代码中更新：
void* AddCurrency_func = (void*)(baseAddress + 0x12345678);
g_functions.AddCurrency = reinterpret_cast<void(*)(...)>(AddCurrency_func);
```

### 方法2: 使用assembly-csharp.dll更新

#### 什么是assembly-csharp.dll？
这是Unity游戏的主要程序集，包含游戏逻辑代码。

#### 反编译assembly-csharp.dll：

1. **下载工具**:
   ```
   https://github.com/cnpy/ILSpy/releases
   ```

2. **反编译**:
   ```bash
   ILSpy.exe assembly-csharp.dll
   ```

3. **查看方法**:
   ```csharp
   // 在ILSpy中找到 ProgressUpdater 类
   // 查看 AddCurrency 方法的实现
   ```

4. **分析代码逻辑**:
   ```csharp
   public void AddCurrency(string curName, int num, int num2, bool b1, bool b2, AnalyticsParamater params)
   {
       // 分析参数和调用方式
       // 这样我们就知道如何调用这个方法
   }
   ```

### 方法3: 使用特征码扫描（推荐）

当游戏更新后，虽然函数地址可能变化，但函数的**机器码特征**通常保持相对稳定。

#### 如何创建特征码？

1. **使用x64dbg或IDA分析函数**:
   ```
   在旧版本中找到 AddCurrency 函数
   查看函数开头的几条指令
   ```

2. **提取特征码**:
   ```cpp
   // 假设函数开头是这样的：
   // 48 83 EC 28     sub rsp, 0x28
   // 48 8B 05 ?? ?? ?? ??  mov rax, [rip+??]
   // 48 85 C0        test rax, rax

   // 创建特征码（??表示通配符）：
   std::string pattern = "48 83 EC 28 48 8B 05 ?? ?? ?? ?? 48 85 C0";
   ```

3. **在代码中使用特征码扫描**:
   ```cpp
   uintptr_t funcAddr = ScanPattern(g_moduleInfo.baseAddress, g_moduleInfo.size, pattern);
   if (funcAddr) {
       g_functions.AddCurrency = reinterpret_cast<FuncType>(funcAddr);
       std::cout << "找到AddCurrency: 0x" << std::hex << funcAddr << std::endl;
   }
   ```

## 📋 实际更新流程

### 场景：游戏更新后功能失效

#### 第1步：识别游戏版本
```bash
# 检查游戏版本信息
# 游戏目录通常有版本文件
# 或者在游戏客户端查看版本号
```

#### 第2步：生成新的dump.cs
```bash
Il2CppDumper.exe GameAssembly.dll global-metadata.dat .\
```

#### 第3步：查找新的偏移
```csharp
// 在dump.cs中搜索关键函数
// 查找 ProgressUpdater 类
// 记录 AddCurrency, AddExperience 等方法的偏移
```

#### 第4步：更新代码
```cpp
// 在 InitializeGameFunctions() 中更新偏移
void InitializeGameFunctions() {
    uintptr_t base = g_moduleInfo.baseAddress;

    // 根据dump.cs更新偏移
    g_functions.AddCurrency = reinterpret_cast<void(*)(...)>(base + NEW_OFFSET_AddCurrency);
    g_functions.AddExperience = reinterpret_cast<void(*)(...)>(base + NEW_OFFSET_AddExperience);
    g_functions.GetLevel = reinterpret_cast<int(*)(void*)>(base + NEW_OFFSET_GetLevel);
    // ... 其他函数
}
```

#### 第5步：测试验证
```bash
# 重新编译DLL
g++ -std=c++17 -shared -m64 ultimate_joiner.cpp -o ultimate_joiner.dll

# 注入游戏测试
injector.exe ultimate_joiner.dll PixelGun3D.exe

# 测试各项功能是否正常
```

## 🔍 常用函数的dump.cs查找方法

### 1. ProgressUpdater::AddCurrency
```csharp
// 在dump.cs中搜索 "ProgressUpdater"
public class ProgressUpdater
{
    // 找到这个方法
    // 0xOFFSET_HERE - AddCurrency
    public void AddCurrency(String curName, Int32 num, Int32 num2, Boolean b1, Boolean b2, AnalyticsParamater params)
}

// 更新偏移：
g_functions.AddCurrency = (void*)(base + 0xOFFSET_HERE);
```

### 2. ExperienceController::AddExperience
```csharp
// 搜索 "ExperienceController"
public class ExperienceController
{
    // 0xOFFSET_HERE - AddExperience
    public void AddExperience(Int32 value, ExpObtainCause reason, Dictionary`2<String,Object> dictionary, Object sex)
}

// 更新偏移：
g_functions.AddExperience = (void*)(base + 0xOFFSET_HERE);
```

### 3. ModuleData::GetLevel
```csharp
// 搜索 "ModuleData"
public class ModuleData
{
    // 0xOFFSET_HERE - GetLevel
    public Int32 get_Level()
}

// 更新偏移：
g_functions.GetLevel = (int(*)(void*))(base + 0xOFFSET_HERE);
```

## ⚡ 快速自动化更新

### 创建偏移配置文件

创建 `offsets.json`:
```json
{
    "version": "1.2.3",
    "last_updated": "2026-07-12",
    "offsets": {
        "ProgressUpdater.AddCurrency": "0x12345678",
        "ExperienceController.AddExperience": "0x87654321",
        "ModuleData.GetLevel": "0x11112222",
        "ProgressUpdater.GetInstance": "0x33334444"
    }
}
```

### 自动化更新脚本

```cpp
// 从JSON加载偏移
void LoadOffsetsFromJSON() {
    // 读取offsets.json
    // 解析JSON
    // 更新g_functions中的所有偏移
}
```

## 🛠️ 故障排除

### Q: 游戏更新后无法识别进程？
**A**:
1. 检查窗口标题是否仍为 "Pixel Gun 3D"
2. 检查进程名称是否变化
3. 查看Unity模块名称是否变化

### Q: 找到了进程但功能不工作？
**A**:
1. 函数偏移可能已更新
2. 使用Il2CppDumper重新生成dump.cs
3. 更新相关函数的偏移地址

### Q: dump.cs中没有找到方法？
**A**:
1. 确认方法名称是否正确
2. 可能在父类或接口中定义
3. 方法可能被混淆或内联

### Q: 特征码扫描找不到函数？
**A**:
1. 函数可能已被大幅修改
2. 特征码可能太具体，尝试更通用的模式
3. 使用dump.cs方法直接获取偏移

## 📊 偏移更新记录表

| 游戏版本 | 更新日期 | AddCurrency偏移 | AddExperience偏移 | GetLevel偏移 | 备注 |
|---------|---------|----------------|------------------|--------------|------|
| 1.0.0   | 2026-01-01 | 0x12345678 | 0x87654321 | 0x11112222 | 初始版本 |
| 1.1.0   | 2026-02-01 | 0x12345680 | 0x87654325 | 0x11112226 | 小更新 |
| 1.2.0   | 2026-07-12 | ??? | ??? | ??? | 需要更新 |

## 🎯 总结

### 游戏进程识别
- ✅ **窗口标题查找** - 稳定，游戏更新通常不变
- ✅ **模块枚举** - Unity特征保持稳定
- ✅ **自动化识别** - 无需手动配置

### 偏移更新
- 🔧 **dump.cs方法** - 最准确，需要手动查找
- 🔧 **assembly-csharp方法** - 理解逻辑，辅助分析
- 🔧 **特征码扫描** - 自动化，有一定容错性

### 推荐工作流程
1. 游戏更新 → 重新生成dump.cs
2. 查找关键函数新偏移
3. 更新代码中的偏移值
4. 重新编译测试
5. 记录到偏移更新表

---

**重要提示**: 游戏反外挂系统可能检测DLL注入和修改行为，请谨慎使用并遵守游戏条款。