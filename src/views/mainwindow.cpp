#include "mainwindow.h"
#include "poswidget.h"
#include "productswidget.h"
#include "categorieswidget.h"
#include "userswidget.h"
#include "saleshistorywidget.h"
#include "reportswidget.h"
#include "settingswidget.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    setWindowTitle(QString::fromUtf8("نقطة البيع - POS Cashier"));
    resize(1200, 800);
}

void MainWindow::setCurrentUser(const User &user)
{
    m_currentUser = user;
    m_posWidget->setCurrentUser(user);
    m_userInfoLabel->setText(user.fullName());

    QString roleStr = user.role() == UserRole::Admin ?
        QString::fromUtf8("مدير النظام") : QString::fromUtf8("كاشير");
    m_roleLabel->setText(roleStr);

    // Hide admin-only pages for cashier
    bool isAdmin = (user.role() == UserRole::Admin);
    m_usersBtn->setVisible(isAdmin);
    m_reportsBtn->setVisible(isAdmin);
    m_settingsBtn->setVisible(isAdmin);
}

void MainWindow::setupUi()
{
    QWidget *centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    setupSidebar();
    mainLayout->addWidget(m_sidebarWidget);

    // Create pages
    m_stackedWidget = new QStackedWidget();

    m_posWidget = new PosWidget();
    m_productsWidget = new ProductsWidget();
    m_categoriesWidget = new CategoriesWidget();
    m_usersWidget = new UsersWidget();
    m_salesWidget = new SalesHistoryWidget();
    m_reportsWidget = new ReportsWidget();
    m_settingsWidget = new SettingsWidget();

    m_stackedWidget->addWidget(m_posWidget);       // 0
    m_stackedWidget->addWidget(m_productsWidget);   // 1
    m_stackedWidget->addWidget(m_categoriesWidget); // 2
    m_stackedWidget->addWidget(m_usersWidget);      // 3
    m_stackedWidget->addWidget(m_salesWidget);      // 4
    m_stackedWidget->addWidget(m_reportsWidget);    // 5
    m_stackedWidget->addWidget(m_settingsWidget);   // 6

    mainLayout->addWidget(m_stackedWidget, 1);

    // Connect signals
    connect(m_productsWidget, &ProductsWidget::productsChanged,
            m_posWidget, &PosWidget::refreshProducts);
    connect(m_categoriesWidget, &CategoriesWidget::categoriesChanged,
            m_posWidget, &PosWidget::refreshProducts);
    connect(m_posWidget, &PosWidget::saleCompleted,
            m_salesWidget, &SalesHistoryWidget::refreshTable);

    // Default page
    showPosPage();
}

void MainWindow::setupSidebar()
{
    m_sidebarWidget = new QWidget();
    m_sidebarWidget->setFixedWidth(200);
    m_sidebarWidget->setStyleSheet(
        "background-color: #1a237e; color: white;");

    QVBoxLayout *sideLayout = new QVBoxLayout(m_sidebarWidget);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    // App title
    QLabel *appTitle = new QLabel(QString::fromUtf8("نقطة البيع"));
    appTitle->setStyleSheet(
        "font-size: 20px; font-weight: bold; color: white; padding: 20px; "
        "background-color: #0d1257; text-align: center;");
    appTitle->setAlignment(Qt::AlignCenter);
    sideLayout->addWidget(appTitle);

    // User info
    QWidget *userWidget = new QWidget();
    userWidget->setStyleSheet("background-color: #283593; padding: 10px;");
    QVBoxLayout *userLayout = new QVBoxLayout(userWidget);
    userLayout->setContentsMargins(10, 10, 10, 10);

    m_userInfoLabel = new QLabel(QString::fromUtf8("المستخدم"));
    m_userInfoLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white;");
    m_userInfoLabel->setAlignment(Qt::AlignCenter);
    userLayout->addWidget(m_userInfoLabel);

    m_roleLabel = new QLabel(QString::fromUtf8("كاشير"));
    m_roleLabel->setStyleSheet("font-size: 12px; color: #90CAF9;");
    m_roleLabel->setAlignment(Qt::AlignCenter);
    userLayout->addWidget(m_roleLabel);

    sideLayout->addWidget(userWidget);

    // Navigation buttons
    m_posBtn = createNavButton(QString::fromUtf8("نقطة البيع"), "#4CAF50");
    connect(m_posBtn, &QPushButton::clicked, this, &MainWindow::showPosPage);
    sideLayout->addWidget(m_posBtn);

    m_productsBtn = createNavButton(QString::fromUtf8("المنتجات"), "#2196F3");
    connect(m_productsBtn, &QPushButton::clicked, this, &MainWindow::showProductsPage);
    sideLayout->addWidget(m_productsBtn);

    m_categoriesBtn = createNavButton(QString::fromUtf8("التصنيفات"), "#FF9800");
    connect(m_categoriesBtn, &QPushButton::clicked, this, &MainWindow::showCategoriesPage);
    sideLayout->addWidget(m_categoriesBtn);

    m_usersBtn = createNavButton(QString::fromUtf8("المستخدمين"), "#9C27B0");
    connect(m_usersBtn, &QPushButton::clicked, this, &MainWindow::showUsersPage);
    sideLayout->addWidget(m_usersBtn);

    m_salesBtn = createNavButton(QString::fromUtf8("سجل المبيعات"), "#00BCD4");
    connect(m_salesBtn, &QPushButton::clicked, this, &MainWindow::showSalesPage);
    sideLayout->addWidget(m_salesBtn);

    m_reportsBtn = createNavButton(QString::fromUtf8("التقارير"), "#E91E63");
    connect(m_reportsBtn, &QPushButton::clicked, this, &MainWindow::showReportsPage);
    sideLayout->addWidget(m_reportsBtn);

    m_settingsBtn = createNavButton(QString::fromUtf8("الإعدادات"), "#607D8B");
    connect(m_settingsBtn, &QPushButton::clicked, this, &MainWindow::showSettingsPage);
    sideLayout->addWidget(m_settingsBtn);

    sideLayout->addStretch();

    // Logout button
    m_logoutBtn = new QPushButton(QString::fromUtf8("تسجيل الخروج"));
    m_logoutBtn->setStyleSheet(
        "QPushButton { background-color: #c62828; color: white; padding: 15px; "
        "font-size: 14px; font-weight: bold; border: none; margin: 10px; border-radius: 6px; }"
        "QPushButton:hover { background-color: #b71c1c; }");
    connect(m_logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogout);
    sideLayout->addWidget(m_logoutBtn);
}

QPushButton* MainWindow::createNavButton(const QString &text, const QString &color)
{
    QPushButton *btn = new QPushButton(text);
    btn->setStyleSheet(QString(
        "QPushButton { background-color: transparent; color: white; padding: 15px 20px; "
        "font-size: 14px; text-align: right; border: none; border-right: 4px solid transparent; }"
        "QPushButton:hover { background-color: rgba(255,255,255,0.1); border-right: 4px solid %1; }"
    ).arg(color));
    btn->setCursor(Qt::PointingHandCursor);
    return btn;
}

void MainWindow::setActiveButton(QPushButton *btn)
{
    // Reset previous
    if (m_activeBtn) {
        m_activeBtn->setStyleSheet(
            "QPushButton { background-color: transparent; color: white; padding: 15px 20px; "
            "font-size: 14px; text-align: right; border: none; border-right: 4px solid transparent; }"
            "QPushButton:hover { background-color: rgba(255,255,255,0.1); }");
    }

    // Set active
    m_activeBtn = btn;
    btn->setStyleSheet(
        "QPushButton { background-color: rgba(255,255,255,0.15); color: white; padding: 15px 20px; "
        "font-size: 14px; font-weight: bold; text-align: right; border: none; "
        "border-right: 4px solid #4CAF50; }");
}

void MainWindow::showPosPage()
{
    m_stackedWidget->setCurrentIndex(0);
    setActiveButton(m_posBtn);
}

void MainWindow::showProductsPage()
{
    m_productsWidget->refreshTable();
    m_stackedWidget->setCurrentIndex(1);
    setActiveButton(m_productsBtn);
}

void MainWindow::showCategoriesPage()
{
    m_categoriesWidget->refreshTable();
    m_stackedWidget->setCurrentIndex(2);
    setActiveButton(m_categoriesBtn);
}

void MainWindow::showUsersPage()
{
    m_usersWidget->refreshTable();
    m_stackedWidget->setCurrentIndex(3);
    setActiveButton(m_usersBtn);
}

void MainWindow::showSalesPage()
{
    m_salesWidget->refreshTable();
    m_stackedWidget->setCurrentIndex(4);
    setActiveButton(m_salesBtn);
}

void MainWindow::showReportsPage()
{
    m_stackedWidget->setCurrentIndex(5);
    setActiveButton(m_reportsBtn);
}

void MainWindow::showSettingsPage()
{
    m_stackedWidget->setCurrentIndex(6);
    setActiveButton(m_settingsBtn);
}

void MainWindow::onLogout()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, QString::fromUtf8("تسجيل الخروج"),
        QString::fromUtf8("هل تريد تسجيل الخروج؟"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Signal to app to show login
        close();
        // Restart the app to show login again
        qApp->exit(1000); // Custom exit code to signal restart
    }
}
