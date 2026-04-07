#include "userswidget.h"
#include "database/databasemanager.h"
#include "models/user.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>

UsersWidget::UsersWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    refreshTable();
}

void UsersWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel(QString::fromUtf8("إدارة المستخدمين"));
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1976D2; padding: 10px;");
    mainLayout->addWidget(title);

    // Toolbar
    QHBoxLayout *toolbar = new QHBoxLayout();
    toolbar->addStretch();

    m_addBtn = new QPushButton(QString::fromUtf8("إضافة مستخدم"));
    m_addBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #45a049; }");
    connect(m_addBtn, &QPushButton::clicked, this, &UsersWidget::onAddClicked);
    toolbar->addWidget(m_addBtn);

    m_editBtn = new QPushButton(QString::fromUtf8("تعديل"));
    m_editBtn->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #1976D2; }");
    connect(m_editBtn, &QPushButton::clicked, this, &UsersWidget::onEditClicked);
    toolbar->addWidget(m_editBtn);

    m_deleteBtn = new QPushButton(QString::fromUtf8("حذف"));
    m_deleteBtn->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #d32f2f; }");
    connect(m_deleteBtn, &QPushButton::clicked, this, &UsersWidget::onDeleteClicked);
    toolbar->addWidget(m_deleteBtn);

    mainLayout->addLayout(toolbar);

    // Table
    m_table = new QTableWidget();
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({
        QString::fromUtf8("المعرف"),
        QString::fromUtf8("اسم المستخدم"),
        QString::fromUtf8("الاسم الكامل"),
        QString::fromUtf8("الدور"),
        QString::fromUtf8("الحالة")
    });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet(
        "QTableWidget { font-size: 14px; border: 1px solid #ddd; }"
        "QTableWidget::item { padding: 8px; }"
        "QHeaderView::section { background-color: #1976D2; color: white; padding: 8px; "
        "font-size: 14px; font-weight: bold; border: none; }");
    mainLayout->addWidget(m_table);
}

void UsersWidget::refreshTable()
{
    auto users = DatabaseManager::instance().getAllUsers();
    m_table->setRowCount(users.size());

    for (int i = 0; i < users.size(); i++) {
        const User &user = users[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(user.id())));
        m_table->setItem(i, 1, new QTableWidgetItem(user.username()));
        m_table->setItem(i, 2, new QTableWidgetItem(user.fullName()));

        QString roleStr = user.role() == UserRole::Admin ?
            QString::fromUtf8("مدير") : QString::fromUtf8("كاشير");
        m_table->setItem(i, 3, new QTableWidgetItem(roleStr));

        QString statusStr = user.isActive() ?
            QString::fromUtf8("نشط") : QString::fromUtf8("معطل");
        QTableWidgetItem *statusItem = new QTableWidgetItem(statusStr);
        statusItem->setForeground(user.isActive() ? QColor("#4CAF50") : QColor("#f44336"));
        m_table->setItem(i, 4, statusItem);
    }
}

void UsersWidget::onAddClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("إضافة مستخدم جديد"));
    dialog.setLayoutDirection(Qt::RightToLeft);
    dialog.setFixedSize(400, 300);

    QFormLayout *form = new QFormLayout(&dialog);
    form->setSpacing(10);

    QLineEdit *usernameEdit = new QLineEdit();
    usernameEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("اسم المستخدم:"), usernameEdit);

    QLineEdit *passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("كلمة المرور:"), passwordEdit);

    QLineEdit *fullNameEdit = new QLineEdit();
    fullNameEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("الاسم الكامل:"), fullNameEdit);

    QComboBox *roleCombo = new QComboBox();
    roleCombo->addItem(QString::fromUtf8("كاشير"), "cashier");
    roleCombo->addItem(QString::fromUtf8("مدير"), "admin");
    roleCombo->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("الدور:"), roleCombo);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(QString::fromUtf8("حفظ"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QString::fromUtf8("إلغاء"));
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    form->addRow(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        if (usernameEdit->text().trimmed().isEmpty() || passwordEdit->text().isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("خطأ"),
                                 QString::fromUtf8("يرجى ملء جميع الحقول المطلوبة"));
            return;
        }

        User user;
        user.setUsername(usernameEdit->text().trimmed());
        user.setPassword(User::hashPassword(passwordEdit->text()));
        user.setFullName(fullNameEdit->text().trimmed());
        user.setRole(User::roleFromString(roleCombo->currentData().toString()));

        if (DatabaseManager::instance().addUser(user)) {
            refreshTable();
        } else {
            QMessageBox::critical(this, QString::fromUtf8("خطأ"),
                                  QString::fromUtf8("فشل إضافة المستخدم. قد يكون اسم المستخدم مستخدماً بالفعل."));
        }
    }
}

void UsersWidget::onEditClicked()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, QString::fromUtf8("تنبيه"),
                             QString::fromUtf8("يرجى اختيار مستخدم للتعديل"));
        return;
    }

    int userId = m_table->item(row, 0)->text().toInt();
    User user = DatabaseManager::instance().getUserById(userId);
    if (!user.isValid()) return;

    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("تعديل المستخدم"));
    dialog.setLayoutDirection(Qt::RightToLeft);
    dialog.setFixedSize(400, 350);

    QFormLayout *form = new QFormLayout(&dialog);
    form->setSpacing(10);

    QLineEdit *usernameEdit = new QLineEdit(user.username());
    usernameEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("اسم المستخدم:"), usernameEdit);

    QLineEdit *passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText(QString::fromUtf8("اتركه فارغاً لعدم التغيير"));
    passwordEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("كلمة المرور:"), passwordEdit);

    QLineEdit *fullNameEdit = new QLineEdit(user.fullName());
    fullNameEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("الاسم الكامل:"), fullNameEdit);

    QComboBox *roleCombo = new QComboBox();
    roleCombo->addItem(QString::fromUtf8("كاشير"), "cashier");
    roleCombo->addItem(QString::fromUtf8("مدير"), "admin");
    roleCombo->setCurrentIndex(user.role() == UserRole::Admin ? 1 : 0);
    roleCombo->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("الدور:"), roleCombo);

    QCheckBox *activeCheck = new QCheckBox(QString::fromUtf8("نشط"));
    activeCheck->setChecked(user.isActive());
    activeCheck->setStyleSheet("font-size: 14px;");
    form->addRow(QString::fromUtf8("الحالة:"), activeCheck);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(QString::fromUtf8("حفظ"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QString::fromUtf8("إلغاء"));
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    form->addRow(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        user.setUsername(usernameEdit->text().trimmed());
        if (!passwordEdit->text().isEmpty()) {
            user.setPassword(User::hashPassword(passwordEdit->text()));
        } else {
            user.setPassword(""); // Signal to not update password
        }
        user.setFullName(fullNameEdit->text().trimmed());
        user.setRole(User::roleFromString(roleCombo->currentData().toString()));
        user.setActive(activeCheck->isChecked());

        if (DatabaseManager::instance().updateUser(user)) {
            refreshTable();
        }
    }
}

void UsersWidget::onDeleteClicked()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, QString::fromUtf8("تنبيه"),
                             QString::fromUtf8("يرجى اختيار مستخدم للحذف"));
        return;
    }

    int userId = m_table->item(row, 0)->text().toInt();
    if (userId == 1) {
        QMessageBox::warning(this, QString::fromUtf8("خطأ"),
                             QString::fromUtf8("لا يمكن حذف المدير الرئيسي"));
        return;
    }

    QString name = m_table->item(row, 2)->text();
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, QString::fromUtf8("تأكيد الحذف"),
        QString::fromUtf8("هل تريد حذف المستخدم: %1؟").arg(name),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteUser(userId)) {
            refreshTable();
        }
    }
}
