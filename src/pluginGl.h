#pragma once
// ============================================================================
// pluginGl.h —— NDD (Notepad--) 插件通用接口头文件
// ----------------------------------------------------------------------------
// 说明：
//   这是插件方对 NDD 主程序接口的"最小可用镜像"。结构体与宏定义
//   必须与 NDD 主程序 src/plugin/ 目录下对应文件保持一致（ABI 兼容）。
//   建议开发时直接从主程序仓库 https://gitee.com/cxasm/notepad--
//   的 src/plugin/ 目录拷贝权威版本覆盖此文件。
// ============================================================================

#include <QString>
#include <QMenu>
#include <QWidget>
#include <functional>

class QsciScintilla;   // 前向声明（Qsci 头文件在 include 路径中）

// 导出宏 —— Windows DLL 场景
#if defined(_WIN32)
#  if defined(NDD_EXPORTDLL)
#    define NDD_EXPORT __declspec(dllexport)
#  else
#    define NDD_EXPORT __declspec(dllimport)
#  endif
#else
#  define NDD_EXPORT __attribute__((visibility("default")))
#endif

// 插件元信息结构体
struct NDD_PROC_DATA {
    QString m_strPlugName;   // 插件名称（将显示在"插件"菜单下）
    QString m_strComment;    // 插件描述/说明
    QString m_version;       // 版本号，如 "v1.0"
    QString m_auther;        // 作者名
    int     m_menuType;      // 菜单类型：0=一级菜单项，1=二级菜单根节点（插件自己挂子菜单）
    QMenu*  m_rootMenu;      // 插件根菜单指针（m_menuType=1 时由主程序分配）

    NDD_PROC_DATA()
        : m_menuType(0)
        , m_rootMenu(nullptr)
    {}
};

// 插件生命周期回调
extern "C" {
    NDD_EXPORT bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData);
    NDD_EXPORT int  NDD_PROC_MAIN(QWidget* pNotepad,
                                  const QString& strFileName,
                                  std::function<QsciScintilla* ()> getCurEdit,
                                  NDD_PROC_DATA* pProcData);
}
