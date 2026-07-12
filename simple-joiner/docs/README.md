# 🎮 Pixel Gun 3D 工具 - 简化整合版

## ✅ 简化完成！

**之前**: 5个不同的DLL版本  
**现在**: 1个DLL + 1个注入器 = **只需要2个文件**

## 📦 文件清单

```
simple-joiner/
├── bin/
│   ├── injector.exe              # DLL注入器 (1.08 MB)
│   └── all-in-one_joiner.dll     # 全功能整合版 (1.17 MB)
│
├── SIMPLE_GUIDE.md               # 快速开始指南 ⭐
├── OFFSET_UPDATE_GUIDE.md        # 偏移更新完全指南
├── build.bat                     # 一键构建脚本
└── all-in-one_joiner.cpp         # 整合版源码
```

## 🚀 立即使用

### 方法1: 命令行注入（推荐）

```bash
cd simple-joiner
bin\injector.exe bin\all-in-one_joiner.dll PixelGun3D.exe
```

### 方法2: 交互式注入

```bash
bin\injector.exe
# 按提示选择DLL和进程
```

## 🎯 所有功能整合

### 💰 货币修改
- 添加金币
- 添加宝石

### 📊 角色功能
- 设置等级
- 添加经验  
- 查看角色信息

### 🏠 房间功能
- 加入房间（基于你的Git变更）

### 🌐 网络功能
- WebSocket消息发送
- 游戏事件发送

### 🔍 游戏识别
- 自动识别Pixel Gun 3D进程
- 获取游戏模块基址
- 显示详细游戏信息

## 📋 功能菜单

注入成功后会显示：

```
========================================
   🎮 Pixel Gun 3D - 全能工具箱 🎮   
========================================
【💰 货币功能】
  1. 添加金币
  2. 添加宝石
【📊 角色功能】
  3. 设置等级
  4. 添加经验
  5. 查看角色信息
【🏠 房间功能】
  6. 加入房间
【🌐 网络功能】
  7. 发送WebSocket消息
  8. 发送游戏事件
【ℹ️ 信息】
  9. 显示游戏信息
 10. 显示使用帮助
【❌ 系统】
  0. 退出
========================================
```

## 🔧 游戏更新处理

### 当游戏更新后功能失效时：

#### 快速修复步骤：

1. **提取游戏文件**:
   ```
   游戏目录/GameAssembly.dll
   游戏目录/global-metadata.dat
   ```

2. **生成dump.cs**:
   ```bash
   Il2CppDumper.exe GameAssembly.dll global-metadata.dat ./dump
   ```

3. **查找新偏移**:
   ```csharp
   // 在 dump.cs 中搜索
   public class ProgressUpdater {
       // 0x12345678 - AddCurrency  ⭐ 记录这个偏移
   }
   ```

4. **更新源码**:
   ```cpp
   // 在 all-in-one_joiner.cpp 中更新
   g_functions.AddCurrency = reinterpret_cast<void(*)(...)>(
       base + 0x12345678  // 新偏移
   );
   ```

5. **重新编译**:
   ```bash
   build.bat
   ```

### 详细指南：

查看 [OFFSET_UPDATE_GUIDE.md](OFFSET_UPDATE_GUIDE.md) 获取完整的偏移更新教程。

## 🎯 基于你的Git变更

### 核心功能保留：

✅ **JoinRoomByName** - 来自 `GameplayMain.cpp`  
✅ **货币修改** - 来自 `ProgressUpdater::AddCurrency`  
✅ **等级设置** - 来自 `ExperienceController::AddExperience`  
✅ **WebSocket** - 来自 `WebsocketCore.hpp`  
✅ **进程识别** - 自动检测Pixel Gun 3D

### 游戏进程识别：

- **目标进程**: `PixelGun3D.exe`
- **窗口标题**: `"Pixel Gun 3D"`  
- **识别方法**: 窗口查找 + Unity模块枚举
- **稳定性**: 游戏更新时通常保持稳定 ✅

## 📊 版本对比

| 特性 | 之前 | 现在 |
|------|------|------|
| **DLL数量** | 5个 | **1个** ⭐ |
| **文件总数** | 7个 | **2个** ⭐ |
| **总大小** | ~5.5 MB | **~2.2 MB** ⭐ |
| **使用复杂度** | 需要选择版本 | **直接使用** ⭐ |
| **功能完整度** | 分散在多个版本 | **全部整合** ⭐ |

## 🛠️ 重新构建

如果需要修改源码后重新构建：

```bash
# 方法1: 使用构建脚本
build.bat

# 方法2: 手动编译
cd simple-joiner
export PATH="/d/program/msys64/ucrt64/bin:$PATH"
g++ -std=c++17 -shared -m64 -DPROJECT_NAME=\"nazimod\" \
    -static-libgcc -static-libstdc++ -static -s \
    all-in-one_joiner.cpp -o bin/all-in-one_joiner.dll
```

## 📚 文档导航

### 快速开始
- **[SIMPLE_GUIDE.md](SIMPLE_GUIDE.md)** - 5分钟快速开始 ⭐

### 技术文档  
- **[OFFSET_UPDATE_GUIDE.md](OFFSET_UPDATE_GUIDE.md)** - 偏移更新完全指南
- **[USAGE_GUIDE.md](USAGE_GUIDE.md)** - 详细使用指南
- **[FINAL_GUIDE.md](FINAL_GUIDE.md)** - 技术详解

### 源码
- **[all-in-one_joiner.cpp](all-in-one_joiner.cpp)** - 整合版源码
- **[injector.cpp](injector.cpp)** - 注入器源码

## ⚡ 快速命令参考

### 使用命令
```bash
# 注入到游戏
bin\injector.exe bin\all-in-one_joiner.dll PixelGun3D.exe

# 重新构建
build.bat

# 查看帮助
选择菜单中的选项 10
```

### 功能测试
```bash
# 测试添加1000金币
# 在菜单中选择 1，然后输入 1000

# 测试设置等级50
# 在菜单中选择 3，然后输入 50

# 测试加入房间
# 在菜单中选择 6，然后输入房间名
```

## 🎊 总结

### ✅ 简化成果：

- **从5个DLL → 1个DLL** ✅
- **从7个文件 → 2个文件** ✅  
- **从5.5MB → 2.2MB** ✅
- **功能完全整合** ✅
- **使用更加简单** ✅

### 🎮 立即开始：

```bash
cd simple-joiner
bin\injector.exe bin\all-in-one_joiner.dll PixelGun3D.exe
```

**就这么简单！** 🎉

---

## 📞 常见问题

### Q: 为什么整合成一个版本？
**A**: 为了简化使用，避免选择困难，所有功能整合在一个DLL中。

### Q: 功能完整吗？
**A**: 完整！包含之前所有版本的功能：房间加入、货币修改、等级设置、WebSocket。

### Q: 文件更小了吗？
**A**: 是的！从5.5MB减少到2.2MB，优化了编译和去除了重复代码。

### Q: 如何更新偏移？
**A**: 查看 [OFFSET_UPDATE_GUIDE.md](OFFSET_UPDATE_GUIDE.md)，有完整的步骤说明。

### Q: 游戏更新后怎么办？
**A**: 重新生成dump.cs，更新偏移，重新编译即可。

---

**简化完成！享受你的整合版游戏工具吧！** 🎮