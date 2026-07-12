# 🎮 Pixel Gun 3D 游戏工具

**文件已重新组织，结构清晰！**

## 🚀 立即开始

### 你要运行哪个？

```bash
# 运行这个脚本！
cd simple-joiner\scripts
package.bat
```

**这会自动生成 `release/` 目录，包含：**
- `injector.exe` - 注入器
- `gamer.dll` - 游戏工具DLL  
- `启动.bat` - 启动脚本

---

## 📁 目录结构

```
simple-joiner/
├── src/         # 源码（.cpp文件）
├── docs/        # 文档（.md文件）
├── scripts/     # 脚本（.bat文件）⭐
├── release/     # 发布版本（最终使用）
└── build/       # 开发构建（临时文件）
```

---

## 💡 常用操作

### 打包
```bash
cd scripts
package.bat
```

### 使用
```bash
cd release
启动.bat           # 或直接运行 injector.exe
```

### 查看文档
```bash
cd docs
# 查看需要的文档
```

---

## 📦 生成的文件

### release/ 目录包含：

```
release/
├── injector.exe    # 注入器 (1.1 MB)
├── gamer.dll       # 游戏工具DLL (1.2 MB)
└── 启动.bat         # 快速启动脚本
```

### 使用方法：

```bash
cd release

# 方式1: 双击 启动.bat
# 方式2: 运行 injector.exe
```

---

## 🎯 包含的功能

- ✅ **房间加入** - 基于你的Git变更
- ✅ **货币修改** - 金币、宝石添加
- ✅ **等级设置** - 经验和等级控制
- ✅ **WebSocket** - 完整消息系统
- ✅ **游戏识别** - 自动检测Pixel Gun 3D
- ✅ **中文界面** - 修复了编码问题
- ✅ **自动检测** - 优先查找gamer.dll

---

## 📚 详细文档

- [docs/QUICK_REF.md](docs/QUICK_REF.md) - 快速参考 ⭐
- [docs/README.md](docs/README.md) - 完整项目说明
- [docs/OFFSET_UPDATE_GUIDE.md](docs/OFFSET_UPDATE_GUIDE.md) - 偏移更新指南
- [docs/INJECTION_WAYS.md](docs/INJECTION_WAYS.md) - 注入方式详解
- [docs/ORGANIZATION.md](docs/ORGANIZATION.md) - 目录结构说明

---

## 🎊 总结

### 文件结构优势：

- ✅ **源码分离** - `src/` 清晰管理
- ✅ **文档独立** - `docs/` 便于查阅
- ✅ **脚本集中** - `scripts/` 统一管理
- ✅ **发布分离** - `release/` 用于分发
- ✅ **构建分离** - `build/` 开发使用

### 使用流程：

```
运行 scripts\package.bat → 使用 release\ 文件
```

---

**文件结构已优化！使用更清晰！** 🎮