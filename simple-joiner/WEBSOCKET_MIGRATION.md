# WebSocket 框架迁移说明 (主项目 → simple-joiner)

simple-joiner 原来用"直接调用游戏函数"的假实现,货币/武器修改**不会同步到服务器**。
本次把主项目 (germanymod-pc) 的**完整 WebSocket 框架**移植过来,现在所有功能都走
真正的 WebSocket 注入(Hook AesFacade 加密层),与主项目行为一致。

## 架构

```
用户菜单 ─► AccountCommands ─► ProgressUpdaterHelper ─► MessageBuilder
   │                                                          │ (BuildSnapshot/BuildCommand)
   ▼                                                          ▼
IL2CPP::INIT ─► PointerFunctions::INIT ─► WebsocketCore::INIT (MinHook 注册 3 个 Hook)
                                                              │
                                              Hook SendSocketMessage  → 捕获 WSManagerInstance
                                              Hook AesFacade[0x1]     → 拦截解密后的接收
                                              Hook AesFacade[0x2]     → 拦截加密前的发送(注入命令在这里)
```

## 初始化顺序 (all-in-one_joiner.cpp)

```cpp
IL2CPP::INIT();           // IL2CPP 反射 (需 GameAssembly.dll 已加载)
PointerFunctions::INIT(); // 解析所有游戏函数指针 (CommandID/Pointer)
WebsocketCore::INIT();    // 注册 WebSocket hooks
// 然后等 WSManagerInstance (游戏首次发消息时被 hook 捕获)
```

## 目录结构

```
simple-joiner/
├── dependencies/
│   ├── il2cpp/           # proplamatic-il2cpp 反射框架 (16 个 .cpp)
│   ├── logger/           # Logger (已修 g++ 兼容)
│   ├── minhook/          # MinHook Hook 引擎 (替代 MS Detours)
│   ├── json.hpp          # nlohmann/json 单文件
│   └── detours.h         # 保留(未使用,MinHook 替代)
├── src/
│   ├── runtime/          # 游戏层 (保持原项目目录结构)
│   │   ├── game/import/      # Structs, PointerFunctions
│   │   ├── game/structures/  # PointerWrapper
│   │   ├── game/websocket/   # WebsocketCore, MessageBuilder, AccountCommands...
││   │   ├── game/            # Functions (ContentKeyRegister)
│   │   └── util/             # HookingUtil (改用 MinHook)
│   ├── all-in-one_joiner.cpp # 主入口 (接入真框架)
│   └── injector.cpp          # 注入器 (未改)
└── scripts/build.bat        # 一键构建
```

## MSVC → g++ 兼容性修复清单

移植时改了这些(都是 MSVC 宽松、g++ 严格导致的):

### IL2CPP 框架 (dependencies/il2cpp/)
1. `backend/types.hpp` — 加 `#include <cstdint>` (MSVC 间接带入,g++ 不)
2. `structures/misc/PatternStructs.hpp` — `const const char*` → `const char*` (MSVC 容忍重复 const)
3. `backend/api-function.hpp` — `GetProcAddress()` 返回值强转 `void*` (FARPROC→void*)
4. `structures/Object.hpp` + `Object.cpp` — `GetFieldPtr` 加 `const` (g++ 在模板体里也做 const 检查)
5. `wrapper/MethodWrapper.hpp` — `nullptr_t` → `std::nullptr_t` + `#include <cstddef>`
6. `structures/MethodInfo.cpp` — `methodPointer` 强转 `void*` (函数指针→void*)
7. `misc/CommonGameStructs.hpp` — 加 `M_PI` fallback 定义 (g++ `<cmath>` 藏起 M_PI)

### 游戏层 (src/runtime/)
8. `util/HookingUtil.cpp` — **整文件重写**: Detours → MinHook (detours-x64.lib 是 MSVC COFF,g++ 链接不了)
9. `game/Functions.cpp` — 加 `#include <algorithm>` + `using std::min/max` + `min((size_t)maxSize, to)` 类型统一
10. `game/websocket/WebsocketCore.cpp` — `*it._Ptr` → `*it` (MSVC STL 迭代器内部成员,g++ 没有)

### 依赖库
11. `logger/Logger.hpp` — `, __VA_ARGS__` → `, ##__VA_ARGS__` (MSVC 自动吞尾逗号,g++ 要 ##)
12. `logger/Logger.cpp` — `ofstream(wstring)` → wstring 转 UTF-8 string (MinGW ofstream 不接受 wstring 路径)

## 构建

```bat
scripts\build.bat
```
产物:`build\all-in-one_joiner.dll` + `build\injector.exe`

三阶段:
- Stage 1: MinHook `.c` (c17)
- Stage 2: IL2CPP + 游戏层 `.cpp` (c++20) → DLL
- Stage 3: injector (c++17) → exe

## 使用

```
build\injector.exe build\all-in-one_joiner.dll PixelGun3D.exe
```
注入后会弹控制台,等 WebSocket 连接(进游戏大厅),然后菜单里:
- 解锁武器/道具/模块/皮肤/涂鸦
- 添加零件/免费升级
- 胜场统计
- 刷新进度 (Reload)

## 注意

- 需要 `g++` 在 `D:\program\msys64\ucrt64\bin\` (改路径编辑 build.bat 的 `GCC=`)
- 游戏更新后 IL2CPP pattern 可能失效,需重新对 dump.cs 校准 (见 PointerFunctions.cpp)
- `WSManagerInstance` 在游戏首次发 WebSocket 消息时才被 hook 捕获,在此之前调用注入会 assert
