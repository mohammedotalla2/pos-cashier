#include "loginwidget.h"
#include "database/databasemanager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QKeyEvent>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();

    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);

    // Allow Enter key to login
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginWidget::onLoginClicked);
    connect(m_usernameEdit, &QLineEdit::returnPressed, [this]() {
        m_passwordEdit->setFocus();
    });
}

void LoginWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    // Title
    QLabel *titleLabel = new QLabel(QString::fromUtf8("نقطة البيع"));
    titleLabel->setStyleSheet(
        "font-size: 36px; font-weight: bold; color: #1976D2; margin: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(QString::fromUtf8("تسجيل الدخول"));
    subtitleLabel->setStyleSheet("font-size: 18px; color: #666; margin-bottom: 20px;");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(subtitleLabel);

    // Login form
    QGroupBox *groupBox = new QGroupBox();
    groupBox->setStyleSheet(
        "QGroupBox { border: 2px solid #e0e0e0; border-radius: 15px; "
        "padding: 30px; background: white; }");
    groupBox->setFixedWidth(400);

    QVBoxLayout *formLayout = new QVBoxLayout(groupBox);
    formLayout->setSpacing(15);

    // Username
    QLabel *userLabel = new QLabel(QString::fromUtf8("اسم المستخدم"));
    userLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");
    formLayout->addWidget(userLabel);

    m_usernameEdit = new QLineEdit();
    m_usernameEdit->setPlaceholderText(QString::fromUtf8("أدخل اسم المستخدم"));
    m_usernameEdit->setStyleSheet(
        "QLineEdit { padding: 12px; font-size: 16px; border: 2px solid #ddd; "
        "border-radius: 8px; } "
        "QLineEdit:focus { border-color: #2196F3; }");
    formLayout->addWidget(m_usernameEdit);

    // Password
    QLabel *passLabel = new QLabel(QString::fromUtf8("كلمة المرور"));
    passLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");
    formLayout->addWidget(passLabel);

    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setPlaceholderText(QString::fromUtf8("أدخل كلمة المرور"));
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setStyleSheet(
        "QLineEdit { padding: 12px; font-size: 16px; border: 2px solid #ddd; "
        "border-radius: 8px; } "
        "QLineEdit:focus { border-color: #2196F3; }");
    formLayout->addWidget(m_passwordEdit);

    // Login button
    m_loginBtn = new QPushButton(QString::fromUtf8("دخول"));
    m_loginBtn->setStyleSheet(
        "QPushButton { background-color: #1976D2; color: white; padding: 14px; "
        "font-size: 18px; font-weight: bold; border: none; border-radius: 8px; }"
        "QPushButton:hover { background-color: #1565C0; }"
        "QPushButton:pressed { background-color: #0D47A1; }");
    formLayout->addWidget(m_loginBtn);

    // Status
    m_statusLabel = new QLabel();
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("font-size: 14px; color: red;");
    formLayout->addWidget(m_statusLabel);

    mainLayout->addWidget(groupBox, 0, Qt::AlignCenter);

    // Default credentials hint
    QLabel *hintLabel = new QLabel(QString::fromUtf8("المستخدم الافتراضي: admin / admin"));
    hintLabel->setStyleSheet("font-size: 12px; color: #999; margin-top: 15px;");
    hintLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(hintLabel);
}

void LoginWidget::onLoginClicked()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        m_statusLabel->setText(QString::fromUtf8("يرجى إدخال اسم المستخدم وكلمة المرور"));
        return;
    }

    DatabaseManager &db = DatabaseManager::instance();

    if (db.authenticateUser(username, password)) {
        User user = db.getUserByUsername(username);
        m_statusLabel->setStyleSheet("font-size: 14px; color: green;");
        m_statusLabel->setText(QString::fromUtf8("تم تسجيل الدخول بنجاح!"));
        emit loginSuccessful(user);
    } else {
        m_statusLabel->setStyleSheet("font-size: 14px; color: red;");
        m_statusLabel->setText(QString::fromUtf8("اسم المستخدم أو كلمة المرور غير صحيحة"));
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
    }
}
