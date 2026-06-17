# ============================================================================
# ndd-strings-to-sqlist.pro —— QMake 工程文件
# ----------------------------------------------------------------------------
# 构建目标：Windows 上生成 ndd-strings-to-sqlist.dll
# 使用方法：
#   1) 安装 Qt 5.15.x + Visual Studio 2019（或 Qt Creator + MSVC2019）
#   2) 编译并安装 QScintilla（使其头文件位于 include/Qsci/，.lib 位于 lib/）
#   3) 命令行：
#        qmake ndd-strings-to-sqlist.pro
#        nmake        （或 jom / mingw32-make，视 Qt 套件而定）
# ============================================================================

QT       += core gui widgets

TARGET   = ndd-strings-to-sqlist
TEMPLATE = lib
CONFIG  += c++17 dll release

DEFINES += NDD_EXPORTDLL _UNICODE UNICODE QT_DEPRECATED_WARNINGS

win32-msvc* {
    QMAKE_CXXFLAGS += /utf-8
}

# ----------------------------------------------------------------------------
# 源文件
# ----------------------------------------------------------------------------
HEADERS += \
    src/StringsToSqlListPlugin.h \
    src/AdvancedDialog.h \
    src/qttestclass.h \
    src/pluginGl.h \
    src/nddpluginapi.h

SOURCES += \
    src/StringsToSqlListPlugin.cpp \
    src/AdvancedDialog.cpp \
    src/qttestclass.cpp

# ----------------------------------------------------------------------------
# 依赖：QScintilla（编辑器）
# ----------------------------------------------------------------------------
# 方案 A（GitHub Actions / 本机也装了 QScintilla 到 Qt 目录）：
#   头文件在 $$[QT_INSTALL_HEADERS]/Qsci/，.lib 在 $$[QT_INSTALL_LIBS]/
#   qmake 会自动把 Qt include/lib 加入查找路径，只需写 LIBS +=
INCLUDEPATH += $$PWD/src

win32 {
    # QScintilla: 头文件通过 Qt 的 include 路径已可找到，只需链接 .lib
    LIBS += -lqscintilla2_qt5
}

# 方案 B（本地使用 libs/ 下的 QScintilla 副本）：
#   取消以下注释，并把 Qsci/ 头文件放到 ./libs/Qsci/，.lib 放到 ./libs/
# INCLUDEPATH += $$PWD/libs $$PWD/libs/Qsci
# win32:LIBS += -L$$PWD/libs

# ----------------------------------------------------------------------------
# 输出目录（便于 windeployqt 收集）
# ----------------------------------------------------------------------------
DESTDIR = $$PWD/build/release
