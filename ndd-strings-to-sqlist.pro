# ============================================================
# ndd-strings-to-sqlist.pro — QMake 工程文件
# ------------------------------------------------------------
# 说明：
#   本项目是 Notepad--（NDD）的一个插件 DLL，依赖：
#     Qt 5.15.x  (core / gui / widgets)
#     QScintilla (Qsci/qsciscintilla.h + qscintilla2_qt5.lib)
#   QScintilla 以"nmake install"方式安装到 Qt 目录时，
#   qmake 会自动在 Qt 的 include/ 与 lib/ 下找到它。
# ============================================================

QT       += core gui widgets

TARGET   = ndd-strings-to-sqlist
TEMPLATE = lib
CONFIG  += c++17 dll release

DEFINES += NDD_EXPORTDLL _UNICODE UNICODE QT_DEPRECATED_WARNINGS

win32-msvc* {
    QMAKE_CXXFLAGS += /utf-8
}

# ---- 源文件 ----
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

# ---- 依赖：QScintilla ----
# 如果 QScintilla 已通过 "nmake install" 安装到当前 Qt 树，
# 下面这一行就够了（因为 Qt 的 include/lib 已在默认搜索路径）
win32:LIBS += -lqscintilla2_qt5

# ---- 输出目录 ----
DESTDIR = $$PWD/build/release
