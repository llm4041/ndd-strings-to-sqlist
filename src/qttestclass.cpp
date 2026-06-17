// ============================================================================
// qttestclass.cpp —— 插件导出入口（NDD_PROC_IDENTIFY / NDD_PROC_MAIN）
// ============================================================================

#include "qttestclass.h"
#include "StringsToSqlListPlugin.h"
#include "nddpluginapi.h"

#include <QWidget>
#include <functional>

class QsciScintilla;


// ---------------------------------------------------------------------------
// NDD_PROC_IDENTIFY：主程序扫描插件时调用，用于注册插件元信息
// ---------------------------------------------------------------------------
extern "C" NDD_EXPORT bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData)
{
    if (!pProcData) return false;

    pProcData->m_strPlugName = QObject::tr("字符串转 SQL IN 列表");
    pProcData->m_strComment  = QObject::tr(
        "将 aaa、bbb、ccc 批量转换为 ('aaa','bbb','ccc') 格式，"
        "支持按逗号 / 换行 / 自定义分隔符，以及单双引号切换。");
    pProcData->m_version     = QString(NDD_PLUGIN_API_VERSION_STR);
    pProcData->m_auther      = QString("ndd-plugin-community");
    pProcData->m_menuType    = 1;   // 1 = 插件自己在 m_rootMenu 挂子菜单
    pProcData->m_rootMenu    = nullptr;

    return true;
}


// ---------------------------------------------------------------------------
// NDD_PROC_MAIN：用户在"插件"菜单点击本插件时被调用
// ---------------------------------------------------------------------------
extern "C" NDD_EXPORT int NDD_PROC_MAIN(
    QWidget* pNotepad,
    const QString& /*strFileName*/,
    std::function<QsciScintilla* ()> getCurEdit,
    NDD_PROC_DATA* pProcData)
{
    if (!pProcData) return 1;

    // 关键点：把传入的数据拷贝保存为 static，因为主程序随后会释放
    // pProcData 指针；但 m_rootMenu 的生命周期跟随主窗口，可放心使用。
    static NDD_PROC_DATA s_procData = *pProcData;

    // 插件实例（也以 static 方式持有，避免被销毁）
    static StringsToSqlListPlugin* s_plugin = nullptr;
    if (!s_plugin) {
        s_plugin = new StringsToSqlListPlugin(pNotepad, std::move(getCurEdit));
    }
    s_plugin->initMenu(s_procData.m_rootMenu);
    return 0;
}
