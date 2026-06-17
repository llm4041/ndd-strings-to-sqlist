// ============================================================================
// StringsToSqlListPlugin.cpp
// ============================================================================

#include "StringsToSqlListPlugin.h"
#include "AdvancedDialog.h"

#include <Qsci/qsciscintilla.h>

#include <QRegularExpression>
#include <QStringList>
#include <QDebug>


// ---------------------------------------------------------------------------
// 构造
// ---------------------------------------------------------------------------
StringsToSqlListPlugin::StringsToSqlListPlugin(
    QWidget* pNotepad,
    std::function<QsciScintilla* ()> getCurEdit,
    QObject* parent)
    : QObject(parent)
    , m_pNotepad(pNotepad)
    , m_getCurEdit(std::move(getCurEdit))
    , m_rootMenu(nullptr)
{
}


// ---------------------------------------------------------------------------
// 菜单挂接
// ---------------------------------------------------------------------------
void StringsToSqlListPlugin::initMenu(QMenu* root)
{
    if (!root) return;
    m_rootMenu = root;

    // 清空已挂接的动作，避免 NDD_PROC_MAIN 被重复调用时追加重复项
    root->clear();

    root->addAction(QObject::tr("按逗号 / 顿号 分割"),
                    this, &StringsToSqlListPlugin::onByComma);
    root->addAction(QObject::tr("按换行分割"),
                    this, &StringsToSqlListPlugin::onByNewLine);
    root->addAction(QObject::tr("高级转换..."),
                    this, &StringsToSqlListPlugin::onAdvanced);
}


// ---------------------------------------------------------------------------
// 三个槽：读入选中文本 -> 转换 -> 写回
// ---------------------------------------------------------------------------
void StringsToSqlListPlugin::onByComma()
{
    QString src = getSelectedTextOrLine();
    if (src.isEmpty()) return;
    replaceSelectedOrLine(convertByComma(src));
}

void StringsToSqlListPlugin::onByNewLine()
{
    QString src = getSelectedTextOrLine();
    if (src.isEmpty()) return;
    replaceSelectedOrLine(convertByNewLine(src));
}

void StringsToSqlListPlugin::onAdvanced()
{
    QString src = getSelectedTextOrLine();
    if (src.isEmpty()) return;

    AdvancedOptions opt;
    AdvancedDialog dlg(m_pNotepad);
    if (dlg.exec() != QDialog::Accepted) return;
    dlg.fillOptions(opt);

    replaceSelectedOrLine(convertAdvanced(src, opt));
}


// ---------------------------------------------------------------------------
// 封装好的三种转换入口
// ---------------------------------------------------------------------------
QString StringsToSqlListPlugin::convertByComma(const QString& input)
{
    // 同时兼容：英文逗号、中文顿号、中英文分号、制表符
    // 中间的空白会通过后续 trimmed 处理
    QStringList parts;
    const static QRegularExpression re(QStringLiteral("[,、;；\\t]+"));
    parts = input.split(re, Qt::KeepEmptyParts);
    return doConvert(parts,
                     /* trim */ true,
                     /* skip empty */ true,
                     QStringLiteral("'"),
                     /* wrap */ true,
                     QStringLiteral(","));
}

QString StringsToSqlListPlugin::convertByNewLine(const QString& input)
{
    QString normalized = input;
    normalized.replace(QStringLiteral("\r\n"), QStringLiteral("\n"))
              .replace(QStringLiteral("\r"), QStringLiteral("\n"));
    QStringList parts = normalized.split(QChar('\n'), Qt::KeepEmptyParts);
    return doConvert(parts,
                     /* trim */ true,
                     /* skip empty */ true,
                     QStringLiteral("'"),
                     /* wrap */ true,
                     QStringLiteral(","));
}

QString StringsToSqlListPlugin::convertAdvanced(const QString& input,
                                                const AdvancedOptions& opt)
{
    if (opt.delimiter.isEmpty()) return input;

    // 魔法值："__COMMA_FAMILY__" 代表多分隔符（逗号/顿号/分号/制表符）
    if (opt.delimiter == QStringLiteral("__COMMA_FAMILY__")) {
        const static QRegularExpression re(QStringLiteral("[,、;；\\t]+"));
        QStringList parts = input.split(re, Qt::KeepEmptyParts);
        return doConvert(parts,
                         opt.trimItems,
                         opt.skipEmpty,
                         opt.quoteChar,
                         opt.wrapParens,
                         opt.joiner.isEmpty() ? QStringLiteral(",") : opt.joiner);
    }

    QStringList parts;
    if (opt.delimiter == QStringLiteral("\\n")) {
        QString normalized = input;
        normalized.replace(QStringLiteral("\r\n"), QStringLiteral("\n"))
                  .replace(QStringLiteral("\r"), QStringLiteral("\n"));
        parts = normalized.split(QChar('\n'), Qt::KeepEmptyParts);
    } else if (opt.delimiter == QStringLiteral("\\t")) {
        parts = input.split(QChar('\t'), Qt::KeepEmptyParts);
    } else {
        parts = input.split(opt.delimiter, Qt::KeepEmptyParts);
    }
    return doConvert(parts,
                     opt.trimItems,
                     opt.skipEmpty,
                     opt.quoteChar,
                     opt.wrapParens,
                     opt.joiner.isEmpty() ? QStringLiteral(",") : opt.joiner);
}


// ---------------------------------------------------------------------------
// 通用实现（外部使用字符串版，内部走 QStringList 版）
// ---------------------------------------------------------------------------
QString StringsToSqlListPlugin::doConvert(const QString& input,
                                          const QString& delimiter,
                                          bool trimItems,
                                          bool skipEmpty,
                                          const QString& quoteChar,
                                          bool wrapParens,
                                          const QString& joiner)
{
    if (delimiter.isEmpty()) return input;
    QStringList parts = input.split(delimiter, Qt::KeepEmptyParts);
    return doConvert(parts, trimItems, skipEmpty, quoteChar, wrapParens, joiner);
}

QString StringsToSqlListPlugin::doConvert(const QStringList& parts,
                                          bool trimItems,
                                          bool skipEmpty,
                                          const QString& quoteChar,
                                          bool wrapParens,
                                          const QString& joiner)
{
    QStringList out;
    out.reserve(parts.size());

    for (QString item : parts) {
        if (trimItems) item = item.trimmed();
        if (skipEmpty && item.isEmpty()) continue;
        if (!quoteChar.isEmpty()) {
            item = quoteChar + escapeQuote(item, quoteChar) + quoteChar;
        }
        out.append(item);
    }

    QString body = out.join(joiner + QStringLiteral(" "));
    if (wrapParens) {
        body.prepend(QStringLiteral("("));
        body.append(QStringLiteral(")"));
    }
    return body;
}

QString StringsToSqlListPlugin::escapeQuote(const QString& token,
                                            const QString& quoteChar)
{
    if (quoteChar.isEmpty()) return token;
    // SQL 约定：字符串字面量内部的引号以"加倍"方式转义
    // 例如：It's  ->  'It''s'
    //       He said "hi"  ->  "He said ""hi"""
    QString escaped = token;
    escaped.replace(quoteChar, quoteChar + quoteChar);
    return escaped;
}


// ---------------------------------------------------------------------------
// 编辑框读写
// ---------------------------------------------------------------------------
QString StringsToSqlListPlugin::getSelectedTextOrLine()
{
    if (!m_getCurEdit) return {};
    QsciScintilla* pEdit = m_getCurEdit();
    if (!pEdit) return {};

    QString sel = pEdit->selectedText();
    // Scintilla 有时会把换行转成 "\r"，这里统一成 "\n"
    sel.replace(QStringLiteral("\r\n"), QStringLiteral("\n"))
       .replace(QStringLiteral("\r"), QStringLiteral("\n"));
    if (!sel.isEmpty()) return sel;

    // 未选中时，读取光标所在的整行
    int line = pEdit->lineFromPosition(pEdit->currentPosition());
    QString lineText = pEdit->text(line);
    // pEdit->text(line) 往往末尾附换行，这里去掉
    while (!lineText.isEmpty() &&
           (lineText.endsWith(QChar('\n')) || lineText.endsWith(QChar('\r')))) {
        lineText.chop(1);
    }
    return lineText;
}

void StringsToSqlListPlugin::replaceSelectedOrLine(const QString& text)
{
    if (!m_getCurEdit) return;
    QsciScintilla* pEdit = m_getCurEdit();
    if (!pEdit) return;

    if (!pEdit->selectedText().isEmpty()) {
        pEdit->replaceSelectedText(text);
    } else {
        int line = pEdit->lineFromPosition(pEdit->currentPosition());
        int pos  = pEdit->positionFromLine(line);
        int len  = pEdit->lineLength(line);
        pEdit->setSelection(pos, pos + len);
        pEdit->replaceSelectedText(text);
    }
}
