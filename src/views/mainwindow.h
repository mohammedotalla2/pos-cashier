#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "models/user.h"

class PosWidget;
class ProductsWidget;
class CategoriesWidget;
class UsersWidget;
class SalesHistoryWidget;
class ReportsWidget;
class SettingsWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void setCurrentUser(const User &user);

private slots:
    void showPosPage();
    void showProductsPage();
    void showCategoriesPage();
    void showUsersPage();
    void showSalesPage();
    void showReportsPage();
    void showSettingsPage();
    void onLogout();

private:
    void setupUi();
    void setupSidebar();
    QPushButton* createNavButton(const QString &text, const QString &color);
    void setActiveButton(QPushButton *btn);

    QStackedWidget *m_stackedWidget;
    QWidget *m_sidebarWidget;
    QLabel *m_userInfoLabel;
    QLabel *m_roleLabel;

    // Navigation buttons
    QPushButton *m_posBtn;
    QPushButton *m_productsBtn;
    QPushButton *m_categoriesBtn;
    QPushButton *m_usersBtn;
    QPushButton *m_salesBtn;
    QPushButton *m_reportsBtn;
    QPushButton *m_settingsBtn;
    QPushButton *m_logoutBtn;
    QPushButton *m_activeBtn = nullptr;

    // Pages
    PosWidget *m_posWidget;
    ProductsWidget *m_productsWidget;
    CategoriesWidget *m_categoriesWidget;
    UsersWidget *m_usersWidget;
    SalesHistoryWidget *m_salesWidget;
    ReportsWidget *m_reportsWidget;
    SettingsWidget *m_settingsWidget;

    User m_currentUser;
};

#endif // MAINWINDOW_H
