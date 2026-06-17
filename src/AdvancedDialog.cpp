// ============================================================================
// AdvancedDialog.cpp
// ============================================================================

#include "AdvancedDialog.h"

#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>


AdvancedDialog::AdvancedDialog(QWidget* parent)
    : QDialog(parent)
    , m_presetCombo(nullptr)
    , m_customLabel(nullptr)
    , m_customEdit(nullptr)
    , m_trimCheck(nullptr)
    , m_skipEmptyCheck(nullptr)
    , m_quoteCombo(nullptr)
    , m_wrapCheck(nullptr)
    , m_joinerEdit(nullptr)
    , m_buttons(nullptr)
{
    setWindowTitle(QObject::tr("高级转换 —— 字符串转 SQL IN 列表"));
    setMinimumWidth(420);
    buildUi();
}

void AdvancedDialog::buildUi()
{
    auto* mainLayout = new QVBoxLayout(this);

    // --- 分隔符组 ---
    auto* delimBox = new QGroupBox(QObject::tr("分隔符"), this);
    auto* delimForm = new QFormLayout(delimBox);

    m_presetCombo = new QComboBox(delimBox);
    m_presetCombo->addItem(QObject::tr("逗号 / 顿号 / 分号 / 制表符"),
                           QStringLiteral("__COMMA_FAMILY__"));
    m_presetCombo->addItem(QObject::tr("换行 ( \\n )"), QStringLiteral("\\n"));
    m_presetCombo->addItem(QObject::tr("空格"), QStringLiteral(" "));
    m_presetCombo->addItem(QObject::tr("单个字符 '|'"), QStringLiteral("|"));
    m_presetCombo->addItem(QObject::tr("自定义"), QString());

    m_customLabel = new QLabel(QObject::tr("自定义分隔符:"), delimBox);
    m_customEdit = new QLineEdit(delimBox);
    m_customEdit->setPlaceholderText(QObject::tr("例如: , 或 、 或 | 或 - "));

    delimForm->addRow(QObject::tr("预设:"), m_presetCombo);
    delimForm->addRow(m_customLabel, m_customEdit);

    // 初始状态：预设为"逗号/顿号"时，隐藏自定义输入
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdvancedDialog::onPresetChanged);
    onPresetChanged(m_presetCombo->currentIndex());

    mainLayout->addWidget(delimBox);

    // --- 处理选项组 ---
    auto* optBox = new QGroupBox(QObject::tr("处理选项"), this);
    auto* optForm = new QFormLayout(optBox);

    m_trimCheck = new QCheckBox(QObject::tr("去除每项首尾空白 (Trim)"), optBox);
    m_trimCheck->setChecked(true);
    m_skipEmptyCheck = new QCheckBox(QObject::tr("跳过空项"), optBox);
    m_skipEmptyCheck->setChecked(true);

    m_quoteCombo = new QComboBox(optBox);
    m_quoteCombo->addItem(QObject::tr("单引号  '...'"), QStringLiteral("'"));
    m_quoteCombo->addItem(QObject::tr("双引号  \"...\""), QStringLiteral("\""));
    m_quoteCombo->addItem(QObject::tr("不加引号"), QString());

    m_wrapCheck = new QCheckBox(QObject::tr("首尾加  ( ... 包裹")), optBox);
    m_wrapCheck->setChecked(true);

    m_joinerEdit = new QLineEdit(optBox);
    m_joinerEdit->setText(QStringLiteral(","));

    optForm->addRow(m_trimCheck);
    optForm->addRow(m_skipEmptyCheck);
    optForm->addRow(QObject::tr("引号类型:"), m_quoteCombo);
    optForm->addRow(m_wrapCheck);
    optForm->addRow(QObject::tr("连接符:"), m_joinerEdit);

    mainLayout->addWidget(optBox);

    // --- OK / Cancel ---
    m_buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, this);
    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttons);
}

void AdvancedDialog::onPresetChanged(int index)
{
    // 最后一项 = "自定义"；仅在此项显示自定义分隔符输入框
    const bool isCustom = (index == m_presetCombo->count() - 1);
    m_customLabel->setVisible(isCustom);
    m_customEdit->setVisible(isCustom);
}

void AdvancedDialog::fillOptions(AdvancedOptions& opt) const
{
    const int idx = m_presetCombo->currentIndex();
    const QString preset = m_presetCombo->itemData(idx).toString();
    const bool isCustom = (idx == m_presetCombo->count() - 1);

    if (isCustom) {
        opt.delimiter = m_customEdit->text();
    } else {
        // 预设项都已提供 data：逗号族/换行/空格/竖线
        opt.delimiter = preset;
    }

    opt.trimItems   = m_trimCheck->isChecked();
    opt.skipEmpty   = m_skipEmptyCheck->isChecked();
    opt.quoteChar   = m_quoteCombo->currentData().toString();
    opt.wrapParens  = m_wrapCheck->isChecked();
    opt.joiner      = m_joinerEdit->text().trimmed();
    if (opt.joiner.isEmpty()) opt.joiner = QStringLiteral(",");
}
