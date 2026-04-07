#include "settingswidget.h"
#include "utils/config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    loadSettings();
}

void SettingsWidget::setupUi()
{
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border: none;");

    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(scrollWidget);
    mainLayout->setSpacing(15);

    QLabel *title = new QLabel(QString::fromUtf8("الإعدادات"));
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1976D2; padding: 10px;");
    mainLayout->addWidget(title);

    QString groupStyle =
        "QGroupBox { font-size: 16px; font-weight: bold; border: 2px solid #e0e0e0; "
        "border-radius: 10px; margin-top: 15px; padding: 20px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 15px; padding: 0 10px; }";
    QString inputStyle = "padding: 8px; font-size: 14px; border: 1px solid #ccc; border-radius: 5px;";

    // ===== Store Settings =====
    QGroupBox *storeGroup = new QGroupBox(QString::fromUtf8("معلومات المتجر"));
    storeGroup->setStyleSheet(groupStyle);
    QFormLayout *storeForm = new QFormLayout(storeGroup);
    storeForm->setSpacing(10);

    m_storeNameEdit = new QLineEdit();
    m_storeNameEdit->setStyleSheet(inputStyle);
    storeForm->addRow(QString::fromUtf8("اسم المتجر:"), m_storeNameEdit);

    m_storeAddressEdit = new QLineEdit();
    m_storeAddressEdit->setStyleSheet(inputStyle);
    storeForm->addRow(QString::fromUtf8("العنوان:"), m_storeAddressEdit);

    m_storePhoneEdit = new QLineEdit();
    m_storePhoneEdit->setStyleSheet(inputStyle);
    storeForm->addRow(QString::fromUtf8("الهاتف:"), m_storePhoneEdit);

    mainLayout->addWidget(storeGroup);

    // ===== Tax & Currency =====
    QGroupBox *taxGroup = new QGroupBox(QString::fromUtf8("الضريبة والعملة"));
    taxGroup->setStyleSheet(groupStyle);
    QFormLayout *taxForm = new QFormLayout(taxGroup);
    taxForm->setSpacing(10);

    m_taxRateSpin = new QDoubleSpinBox();
    m_taxRateSpin->setRange(0, 100);
    m_taxRateSpin->setDecimals(1);
    m_taxRateSpin->setSuffix(" %");
    m_taxRateSpin->setStyleSheet(inputStyle);
    taxForm->addRow(QString::fromUtf8("نسبة الضريبة:"), m_taxRateSpin);

    m_currencyEdit = new QLineEdit();
    m_currencyEdit->setStyleSheet(inputStyle);
    taxForm->addRow(QString::fromUtf8("رمز العملة:"), m_currencyEdit);

    mainLayout->addWidget(taxGroup);

    // ===== Printer Settings =====
    QGroupBox *printerGroup = new QGroupBox(QString::fromUtf8("إعدادات الطابعة"));
    printerGroup->setStyleSheet(groupStyle);
    QFormLayout *printerForm = new QFormLayout(printerGroup);
    printerForm->setSpacing(10);

    m_useThermalCheck = new QCheckBox(QString::fromUtf8("استخدام طابعة حرارية"));
    m_useThermalCheck->setStyleSheet("font-size: 14px;");
    printerForm->addRow("", m_useThermalCheck);

    m_printerPortEdit = new QLineEdit();
    m_printerPortEdit->setStyleSheet(inputStyle);
    m_printerPortEdit->setPlaceholderText("COM1 / /dev/ttyUSB0");
    printerForm->addRow(QString::fromUtf8("منفذ الطابعة:"), m_printerPortEdit);

    m_baudRateCombo = new QComboBox();
    m_baudRateCombo->addItems({"9600", "19200", "38400", "115200"});
    m_baudRateCombo->setStyleSheet(inputStyle);
    printerForm->addRow(QString::fromUtf8("سرعة الاتصال:"), m_baudRateCombo);

    mainLayout->addWidget(printerGroup);

    // ===== Firebase Settings =====
    QGroupBox *firebaseGroup = new QGroupBox(QString::fromUtf8("إعدادات Firebase"));
    firebaseGroup->setStyleSheet(groupStyle);
    QFormLayout *firebaseForm = new QFormLayout(firebaseGroup);
    firebaseForm->setSpacing(10);

    m_firebaseUrlEdit = new QLineEdit();
    m_firebaseUrlEdit->setStyleSheet(inputStyle);
    m_firebaseUrlEdit->setPlaceholderText("https://your-project.firebaseio.com");
    firebaseForm->addRow(QString::fromUtf8("رابط Firebase:"), m_firebaseUrlEdit);

    m_licenseKeyEdit = new QLineEdit();
    m_licenseKeyEdit->setStyleSheet(inputStyle);
    m_licenseKeyEdit->setReadOnly(true);
    firebaseForm->addRow(QString::fromUtf8("مفتاح الترخيص:"), m_licenseKeyEdit);

    mainLayout->addWidget(firebaseGroup);

    // Save button
    m_saveBtn = new QPushButton(QString::fromUtf8("حفظ الإعدادات"));
    m_saveBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 14px 40px; "
        "font-size: 16px; font-weight: bold; border: none; border-radius: 8px; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:pressed { background-color: #3d8b40; }");
    connect(m_saveBtn, &QPushButton::clicked, this, &SettingsWidget::onSaveClicked);
    mainLayout->addWidget(m_saveBtn, 0, Qt::AlignCenter);

    mainLayout->addStretch();

    scrollArea->setWidget(scrollWidget);

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);
}

void SettingsWidget::loadSettings()
{
    Config &cfg = Config::instance();

    m_storeNameEdit->setText(cfg.storeName());
    m_storeAddressEdit->setText(cfg.storeAddress());
    m_storePhoneEdit->setText(cfg.storePhone());

    m_taxRateSpin->setValue(cfg.taxRate());
    m_currencyEdit->setText(cfg.currencySymbol());

    m_useThermalCheck->setChecked(cfg.useThermalPrinter());
    m_printerPortEdit->setText(cfg.thermalPrinterPort());

    int baudRate = cfg.thermalPrinterBaudRate();
    int baudIndex = m_baudRateCombo->findText(QString::number(baudRate));
    if (baudIndex >= 0) m_baudRateCombo->setCurrentIndex(baudIndex);

    m_firebaseUrlEdit->setText(cfg.firebaseUrl());
    m_licenseKeyEdit->setText(cfg.licenseKey());
}

void SettingsWidget::onSaveClicked()
{
    Config &cfg = Config::instance();

    cfg.setStoreName(m_storeNameEdit->text().trimmed());
    cfg.setStoreAddress(m_storeAddressEdit->text().trimmed());
    cfg.setStorePhone(m_storePhoneEdit->text().trimmed());

    cfg.setTaxRate(m_taxRateSpin->value());
    cfg.setCurrencySymbol(m_currencyEdit->text().trimmed());

    cfg.setUseThermalPrinter(m_useThermalCheck->isChecked());
    cfg.setThermalPrinterPort(m_printerPortEdit->text().trimmed());
    cfg.setThermalPrinterBaudRate(m_baudRateCombo->currentText().toInt());

    cfg.setFirebaseUrl(m_firebaseUrlEdit->text().trimmed());

    QMessageBox::information(this, QString::fromUtf8("تم الحفظ"),
                             QString::fromUtf8("تم حفظ الإعدادات بنجاح!"));
}
