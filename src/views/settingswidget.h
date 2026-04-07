#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);

private slots:
    void onSaveClicked();
    void loadSettings();

private:
    void setupUi();

    // Store
    QLineEdit *m_storeNameEdit;
    QLineEdit *m_storeAddressEdit;
    QLineEdit *m_storePhoneEdit;

    // Tax
    QDoubleSpinBox *m_taxRateSpin;

    // Currency
    QLineEdit *m_currencyEdit;

    // Printer
    QCheckBox *m_useThermalCheck;
    QLineEdit *m_printerPortEdit;
    QComboBox *m_baudRateCombo;

    // Firebase
    QLineEdit *m_firebaseUrlEdit;
    QLineEdit *m_licenseKeyEdit;

    QPushButton *m_saveBtn;
};

#endif // SETTINGSWIDGET_H
