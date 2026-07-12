# 🎯 快速参考指南

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

## 📁 文件组织

### 清晰的目录结构：

```
simple-joiner/
├── src/         # 源码（.cpp文件）
├── docs/        # 文档（.md文件）
├── scripts/     # 脚本（.bat文件）
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

## 🎯 核心文件

### 开发相关
- `src/injector.cpp` - 注入器源码
- `src/all-in-one_joiner.cpp` - 功能DLL源码
- `scripts/package.bat` - 打包脚本 ⭐

### 使用相关
- `release/injector.exe` - 注入器
- `release/gamer.dll` - 游戏工具DLL
- `release/启动.bat` - 启动脚本

### 文档相关
- `docs/README.md` - 完整说明
- `docs/OFFSET_UPDATE_GUIDE.md` - 偏移更新

---

## ⚡ 一句话总结

**运行 `scripts\package.bat` 自动生成 `release\` 目录的所有文件！**