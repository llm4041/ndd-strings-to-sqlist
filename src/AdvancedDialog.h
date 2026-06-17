#pragma once
// ============================================================================
// AdvancedDialog —— 高级转换对话框
// ----------------------------------------------------------------------------
// 允许用户选择：分隔符类型（预设/自定义）、是否 trim、是否
// 跳过空项、引号类型（单/双/不加）、是否包 ()、连接符。
// ============================================================================

#include <QDialog>
#include "StringsToSqlListPlugin.h"

class QComboBox;
class QLineEdit;
class QCheckBox;
class QDialogButtonBox;
class QLabel;

class AdvancedDialog : public QDialog {
    Q_OBJECT
public:
    explicit AdvancedDialog(QWidget* parent = nullptr);

    // 把用户选项写入 AdvancedOptions
    void fillOptions(AdvancedOptions& opt) const;

private slots:
    void onPresetChanged(int index);

private:
    void buildUi();

private:
    QComboBox*         m_presetCombo;   // 预设分隔符
    QLabel*            m_customLabel;
    QLineEdit*         m_customEdit;    // 自定义分隔符
    QCheckBox*         m_trimCheck;
    QCheckBox*         m_skipEmptyCheck;
    QComboBox*         m_quoteCombo;    // 引号类型
    QCheckBox*         m_wrapCheck;     // 是否包 ()
    QLineEdit*         m_joinerEdit;
    QDialogButtonBox*  m_buttons;
};
