#include "activationwidget.h"
#include "utils/config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QApplication>

ActivationWidget::ActivationWidget(QWidget *parent)
    : QWidget(parent),
      m_firebase(new FirebaseManager(this))
{
    setupUi();

    connect(m_firebase, &FirebaseManager::licenseValid,
            this, &ActivationWidget::onLicenseValid);
    connect(m_firebase, &FirebaseManager::licenseInvalid,
            this, &ActivationWidget::onLicenseInvalid);
    connect(m_firebase, &FirebaseManager::activationSuccess,
            this, &ActivationWidget::onActivationSuccess);
    connect(m_firebase, &FirebaseManager::activationFailed,
            this, &ActivationWidget::onActivationFailed);
    connect(m_firebase, &FirebaseManager::networkError,
            this, &ActivationWidget::onNetworkError);

    connect(m_activateBtn, &QPushButton::clicked,
            this, &ActivationWidget::onActivateClicked);
    connect(m_skipBtn, &QPushButton::clicked,
            this, &ActivationWidget::onSkipClicked);
}

void ActivationWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    // Title
    QLabel *titleLabel = new QLabel(QString::fromUtf8("تفعيل البرنامج"));
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #2196F3; margin: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Activation form
    QGroupBox *groupBox = new QGroupBox(QString::fromUtf8("معلومات التفعيل"));
    groupBox->setStyleSheet(
        "QGroupBox { font-size: 16px; font-weight: bold; border: 2px solid #ddd; "
        "border-radius: 10px; margin-top: 10px; padding: 20px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 20px; padding: 0 10px; }");
    groupBox->setFixedWidth(500);

    QFormLayout *formLayout = new QFormLayout(groupBox);
    formLayout->setSpacing(15);

    m_firebaseUrlEdit = new QLineEdit();
    m_firebaseUrlEdit->setPlaceholderText(QString::fromUtf8("https://your-project.firebaseio.com"));
    m_firebaseUrlEdit->setText(Config::instance().firebaseUrl());
    m_firebaseUrlEdit->setStyleSheet("padding: 10px; font-size: 14px; border: 1px solid #ccc; border-radius: 5px;");
    formLayout->addRow(QString::fromUtf8("رابط Firebase:"), m_firebaseUrlEdit);

    m_licenseKeyEdit = new QLineEdit();
    m_licenseKeyEdit->setPlaceholderText(QString::fromUtf8("أدخل مفتاح الترخيص"));
    m_licenseKeyEdit->setStyleSheet("padding: 10px; font-size: 14px; border: 1px solid #ccc; border-radius: 5px;");
    formLayout->addRow(QString::fromUtf8("مفتاح الترخيص:"), m_licenseKeyEdit);

    // Machine ID
    m_machineIdLabel = new QLabel(Config::instance().machineId());
    m_machineIdLabel->setStyleSheet("color: #666; font-size: 11px;");
    m_machineIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    formLayout->addRow(QString::fromUtf8("معرف الجهاز:"), m_machineIdLabel);

    mainLayout->addWidget(groupBox, 0, Qt::AlignCenter);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    m_activateBtn = new QPushButton(QString::fromUtf8("تفعيل"));
    m_activateBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 12px 40px; "
        "font-size: 16px; font-weight: bold; border: none; border-radius: 8px; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:pressed { background-color: #3d8b40; }");
    buttonLayout->addWidget(m_activateBtn);

    m_skipBtn = new QPushButton(QString::fromUtf8("تخطي (نسخة تجريبية)"));
    m_skipBtn->setStyleSheet(
        "QPushButton { background-color: #FF9800; color: white; padding: 12px 30px; "
        "font-size: 14px; border: none; border-radius: 8px; }"
        "QPushButton:hover { background-color: #e68900; }");
    buttonLayout->addWidget(m_skipBtn);

    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);

    // Status
    m_statusLabel = new QLabel();
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("font-size: 14px; margin: 10px;");
    mainLayout->addWidget(m_statusLabel);
}

void ActivationWidget::onActivateClicked()
{
    QString firebaseUrl = m_firebaseUrlEdit->text().trimmed();
    QString licenseKey = m_licenseKeyEdit->text().trimmed();

    if (firebaseUrl.isEmpty()) {
        m_statusLabel->setStyleSheet("font-size: 14px; margin: 10px; color: red;");
        m_statusLabel->setText(QString::fromUtf8("يرجى إدخال رابط Firebase"));
        return;
    }

    if (licenseKey.isEmpty()) {
        m_statusLabel->setStyleSheet("font-size: 14px; margin: 10px; color: red;");
        m_statusLabel->setText(QString::fromUtf8("يرجى إدخال مفتاح الترخيص"));
        return;
    }

    Config::instance().setFirebaseUrl(firebaseUrl);
    m_activateBtn->setEnabled(false);
    m_statusLabel->setStyleSheet("font-size: 14px; margin: 10px; color: #2196F3;");
    m_statusLabel->setText(QString::fromUtf8("جاري التحقق..."));

    m_firebase->validateLicense(licenseKey, Config::instance().machineId());
}

void ActivationWidget::onLicenseValid(const QString &message)
{
    m_statusLabel->setStyleSheet("font-size: 14px; margin: 10px; color: #4CAF50;");
    m_statusLabel->setText(message);

    // Now activate
    m_firebase->activateLicense(m_licenseKeyEdit->text().trimmed(),
                                Config::instance().machineId());
}

void ActivationWidget::onLicenseInvalid(const QString &error)
{
    m_statusLabel->setStyleSheet("font-size: 14px; margin: 10px; color: red;");
    m_statusLabel->setText(error);
    m_activateBtn->setEnabled(true);
}

void ActivationWidget::onActivationSuccess(const QString &message)
{
    m_statusLabel->setStyleSheet("font-size: 14px; margin: 10px; color: #4CAF50;");
    m_statusLabel->setText(message);

    QMessageBox::information(this, QString::fromUtf8("نجاح"),
                             QString::fromUtf8("تم تفعيل البرنامج بنجاح!"));
    emit activationSuccessful();
}

void ActivationWidget::onActivationFailed(const QString &error)
{
    m_statusLabel->setStyleSheet("font-size: 14px; margin: 10px; color: red;");
    m_statusLabel->setText(error);
    m_activateBtn->setEnabled(true);
}

void ActivationWidget::onNetworkError(const QString &error)
{
    m_statusLabel->setStyleSheet("font-size: 14px; margin: 10px; color: red;");
    m_statusLabel->setText(error);
    m_activateBtn->setEnabled(true);
}

void ActivationWidget::onSkipClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, QString::fromUtf8("تخطي التفعيل"),
        QString::fromUtf8("هل تريد استخدام النسخة التجريبية؟\nبعض الميزات قد تكون محدودة."),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        emit skipActivation();
    }
}
