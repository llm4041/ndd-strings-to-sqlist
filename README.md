<<<<<<< HEAD
# ndd-strings-to-sqlist —— Notepad-- 插件：字符串批量转 SQL IN 列表

一个 Notepad-- (NDD) 插件，把光标选中（或所在行）的字符串列表
自动转成 SQL `IN (...)` 格式。例如：

```
aaa、bbb、ccc        ->      ('aaa', 'bbb', 'ccc')
It's                  ->      ('It''s')
(每行一项)            ->      ('line1', 'line2', 'line3')
```

## 功能一览

插件在主菜单 `插件 → 字符串转 SQL IN 列表` 下提供三项操作：

1. **按逗号 / 顿号 分割**：识别 `,`、`、`、`;`、`；`、`\t` 为分隔符
2. **按换行分割**：每行一项，自动跳过空行
3. **高级转换...**：弹出自定义对话框，可选
   - 分隔符（预设：逗号族 / 换行 / 空格 / `|` / 自定义）
   - 是否 Trim、是否跳过空项
   - 引号类型（单引号 `'...'` / 双引号 `"..."` / 不加）
   - 是否外层包 `( ... )`
   - 连接符（默认 `,`）

转换后会**原地替换**选中内容；若未选中则替换光标所在行。

## 目录结构

```
ndd-strings-to-sqlist/
├── src/
│   ├── pluginGl.h              # NDD 插件接口结构体 / 导出宏
│   ├── nddpluginapi.h          # 版本标识
│   ├── StringsToSqlListPlugin.h / .cpp   # 核心转换 + 菜单挂接
│   ├── AdvancedDialog.h / .cpp            # 高级转换对话框
│   └── qttestclass.h / .cpp                # NDD_PROC_IDENTIFY / NDD_PROC_MAIN
├── libs/
│   └── README.md               # 放置 QScintilla 头文件与 .lib 的说明
├── ndd-strings-to-sqlist.pro   # QMake 工程（主构建方式）
├── CMakeLists.txt              # CMake（可选）
└── README.md                   # 本文件
```

## 构建步骤（Windows）

### 前提

- Qt 5.15.x（`msvc2019_64` 推荐）
- Visual Studio 2019 / 2022 或 Qt Creator
- QScintilla（Qt5 版本）已编译好的 `qscintilla2_qt5.lib` / `.dll` 与 `Qsci/` 头文件
- 目标 NDD 主程序的 `pluginGl.h`（已在仓库中提供一份，但**请用与你正在编译的主程序版本一致的文件覆盖**）

### 1. 填充依赖

按照 [`libs/README.md`](libs/README.md) 把 QScintilla 头文件与链接库放入 `libs/`。

### 2. 编译

```powershell
# 使用 Qt 命令行（开始菜单 → Qt 5.15.2 (MSVC 2019 64-bit)）
cd 项目路径
qmake ndd-strings-to-sqlist.pro
nmake          # 或 jom / mingw32-make，取决于套件
```

也可直接用 Qt Creator 打开 `ndd-strings-to-sqlist.pro` 并构建 `Release` 配置。

若更习惯 CMake：

```powershell
cmake -S . -B build ^
  -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64/lib/cmake"
cmake --build build --config Release
```

产物位置：
- QMake：`build/release/ndd-strings-to-sqlist.dll`
- CMake：`build/bin/Release/ndd-strings-to-sqlist.dll`

## 安装到 Notepad--

1. 把 `ndd-strings-to-sqlist.dll` 复制到 Notepad-- 安装目录下的 `plugin/` 子目录
   （若不存在则新建）
2. 若插件需要 `qscintilla2_qt5.dll` / Qt 运行库，可使用 `windeployqt` 收集：
   ```powershell
   windeployqt --release --no-translations ndd-strings-to-sqlist.dll
   ```
   但通常 NDD 主程序自身已包含 Qt 与 QScintilla，因此一般无需重复附带。
3. 重新启动 Notepad--，在主菜单 `插件` 下即会出现新菜单项。

若因 DLL 不兼容导致 NDD 启动失败，请删除对应 `plugin/ndd-strings-to-sqlist.dll` 后重启，再排查构建配置（Qt 版本、架构、编译器）。

## 使用示例

### 示例 1：简单逗号列表

输入（选中以下文本）：
```
aaa, bbb, ccc
```
→ 执行 `插件 → 字符串转 SQL IN 列表 → 按逗号 / 顿号 分割`

得到：
```
('aaa', 'bbb', 'ccc')
```

### 示例 2：每行一项

输入：
```
apple
banana
cherry
```
→ 执行 `按换行分割`

得到：
```
('apple', 'banana', 'cherry')
```

### 示例 3：带单引号

输入：
```
It's
O'Neil
```
→ 执行 `按换行分割`

得到（SQL 标准：单引号自动"加倍"）：
```
('It''s', 'O''Neil')
```

### 示例 4：用高级转换切 JSON 数组为 IN 列表

输入：
```
"1001"|"1002"|"1003"
```
→ `高级转换...` → 分隔符 `|`、引号选"双引号"、包括号 → 得到：
```
(""1001"", ""1002"", ""1003"")
```
注意：若输入本身已含双引号，默认会做 SQL 式加倍；根据实际需求可先去除引号再转换。

## 常见问题

- **Q：菜单项未出现？**  
  A：确认 DLL 在 `plugin/` 目录下，Qt 版本与主程序一致（32 vs 64 位、编译器）。
- **Q：替换后中文乱码？**  
  A：源码编译时需保留 UTF-8。`.pro` 中已加 `win32-msvc* { QMAKE_CXXFLAGS += /utf-8 }`，一般无需再改；若仍乱码，请检查原文件编码是否为 UTF-8。
- **Q：想用双引号替代单引号做 IN？**  
  A：选 `高级转换...` → 引号类型选"双引号"即可。
- **Q：如何绑定快捷键？**  
  A：Notepad-- 主程序支持对菜单项绑定快捷键（见 NDD 官方文档中的"快捷键"一节）；在插件内的 QAction 也可以用 `setShortcut` 设置（本 MVP 未加，留给后续增强）。

## 协议

- 与 Notepad-- 主程序保持一致：GPL-3.0。
- 插件代码（本仓库）仅供 NDD 用户自行编译 / 使用，欢迎基于本模板开发更多小工具插件。
=======
# ndd-strings-to-sqlist
ndd--sql格式化插件
>>>>>>> 7193f8853a6d327778288f6ca54372546ecf897d
