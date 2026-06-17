# libs/ 目录 —— 依赖说明

本目录用于存放编译插件所需的第三方头文件与链接库，
默认**未附带**任何二进制，请按以下步骤补全：

## 1. QScintilla

- 从 QScintilla 官网（或随 Qt 安装的源码包）获得：
  - 头文件目录 `Qsci/`
  - 编译产物：Windows 上为 `qscintilla2_qt5.lib` + `qscintilla2_qt5.dll`

- 将 `Qsci/` 目录拷贝到 **本目录**，即：
  `libs/Qsci/qsciscintilla.h` 等头文件存在
- 将 `qscintilla2_qt5.lib` 放到本目录（`.dll` 放到运行目录或随插件发布）

> 也可以使用环境变量指向系统已安装的 QScintilla，
> 但需在 `.pro` 或 `CMakeLists.txt` 中取消本地 `INCLUDEPATH` / `LIBS`。

## 2. qmyedit_qt5.lib（可选）

- 某些 NDD 发行包或自定义编译主程序会导出 `qmyedit_qt5` 库，
  其中包含插件侧可用的额外接口。若主程序未提供该库，也可
  直接依赖 Qt 与 QScintilla。
- 把 `qmyedit_qt5.lib` 放入本目录，然后在 `.pro` 的 `win32 { LIBS += ... }` 中
  取消 `-lqmyedit_qt5` 的注释。

## 3. 目录结构示例

```
libs/
├── Qsci/
│   ├── qsciscintilla.h
│   ├── qscilexer.h
│   └── ... (其他 QScintilla 头文件)
├── qscintilla2_qt5.lib
├── qmyedit_qt5.lib          (可选)
└── README.md                (本文件)
```
