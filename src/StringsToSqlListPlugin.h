#pragma once
// ============================================================================
// StringsToSqlListPlugin —— 字符串批量转 SQL IN 列表
// ----------------------------------------------------------------------------
// 功能：
//   1. 按逗号 / 顿号 / 空格 / 制表符 / 分号 分割，转为 ('a','b','c')
//   2. 按换行分割
//   3. 高级转换（自定义分隔符、引号类型、是否包括号等）
// ============================================================================

#include <QObject>
#include <QString>
#include <QWidget>
#include <QMenu>
#include <functional>

class QsciScintilla;

struct AdvancedOptions {
    QString delimiter;   // 分隔符（固定字符串），示例："," 或 "\n"
    bool    trimItems;   // 是否 trim 每个 item
    bool    skipEmpty;   // 是否跳过空项
    QString quoteChar;   // "'" 或 "\"" 或 ""（表示不加引号）
    bool    wrapParens;  // 是否包一层 ()
    QString joiner;      // 连接符，默认 ","

    AdvancedOptions()
        : delimiter(",")
        , trimItems(true)
        , skipEmpty(true)
        , quoteChar("'")
        , wrapParens(true)
        , joiner(",")
    {}
};

class StringsToSqlListPlugin : public QObject {
    Q_OBJECT
public:
    StringsToSqlListPlugin(QWidget* pNotepad,
                           std::function<QsciScintilla* ()> getCurEdit,
                           QObject* parent = nullptr);
    ~StringsToSqlListPlugin() override = default;

    // 初始化（挂接）插件根菜单
    void initMenu(QMenu* root);

    // 核心转换（公开以便单元测试/对话框直接调用）
    static QString convertByComma(const QString& input);
    static QString convertByNewLine(const QString& input);
    static QString convertAdvanced(const QString& input,
                                   const AdvancedOptions& opt);

private slots:
    void onByComma();
    void onByNewLine();
    void onAdvanced();

private:
    // 编辑框读写助手
    QString getSelectedTextOrLine();
    void    replaceSelectedOrLine(const QString& text);

    // 通用实现：将 parts 按给定分隔符切分并拼装
    static QString doConvert(const QStringList& parts,
                             bool trimItems,
                             bool skipEmpty,
                             const QString& quoteChar,
                             bool wrapParens,
                             const QString& joiner);

    // 转义：将 token 内出现的 quoteChar 字符"加倍"（SQL 约定）
    static QString escapeQuote(const QString& token, const QString& quoteChar);

private:
    QWidget*                              m_pNotepad;
    std::function<QsciScintilla* ()>      m_getCurEdit;
    QMenu*                                m_rootMenu;
};
