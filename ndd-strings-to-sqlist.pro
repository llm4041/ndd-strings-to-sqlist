# ============================================================================
# ndd-strings-to-sqlist.pro —— QMake 工程文件
# ----------------------------------------------------------------------------
# 构建目标：Windows 上生成 ndd-strings-to-sqlist.dll
# 使用方法：
#   1) 安装 Qt 5.15.x + Visual Studio 2019（或 Qt Creator + MSVC2019）
#   2) 将 QScintilla 头文件与编译好的 qscintilla2_qt5.lib / .dll
#      放入 ./libs/Qsci 与 ./libs 目录（或在 INCLUDEPATH / LIBS 中调整）
#   3) 若目标 NDD 主程序编译产物中带有 qmyedit_qt5.lib 也放入 ./libs
#   4) 命令行：
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
# 依赖：QScintilla（编辑器）与主程序导出的 qmyedit_qt5.lib
# ----------------------------------------------------------------------------
INCLUDEPATH += $$PWD/src \
               $$PWD/libs \
               $$PWD/libs/Qsci

win32 {
    LIBS += -L$$PWD/libs
    # 以下两行请根据本地情况取消或调整文件名
    # LIBS += -lqmyedit_qt5
    # LIBS += -lqscintilla2_qt5
}

# ----------------------------------------------------------------------------
# 输出目录（便于 windeployqt 收集）
# ----------------------------------------------------------------------------
DESTDIR = $$PWD/build/release
