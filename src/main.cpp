#include <QApplication>
#include <QLocale>
#include <QFont>
#include <QStyleFactory>
#include "database/databasemanager.h"
#include "utils/config.h"
#include "views/activationwidget.h"
#include "views/loginwidget.h"
#include "views/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application info
    app.setOrganizationName("POSCashier");
    app.setApplicationName("POSCashier");
    app.setApplicationVersion("1.0.0");

    // Set RTL layout direction for Arabic
    app.setLayoutDirection(Qt::RightToLeft);

    // Set Arabic-friendly font
    QFont appFont("Arial", 12);
    app.setFont(appFont);

    // Set Fusion style for consistent cross-platform look
    app.setStyle(QStyleFactory::create("Fusion"));

    // Global stylesheet
    app.setStyleSheet(
        "QWidget { font-family: 'Arial', 'Tahoma', 'Segoe UI', sans-serif; }"
        "QToolTip { background-color: #333; color: white; border: 1px solid #555; "
        "padding: 5px; font-size: 12px; }"
    );

    // Initialize database
    if (!DatabaseManager::instance().initialize()) {
        qCritical("Failed to initialize database!");
        return -1;
    }

    int exitCode = 0;

    do {
        // Check activation
        if (!Config::instance().isActivated()) {
            ActivationWidget activationWidget;
            activationWidget.setWindowTitle(QString::fromUtf8("تفعيل البرنامج - POS Cashier"));
            activationWidget.resize(700, 500);

            bool activated = false;
            bool skipped = false;

            QObject::connect(&activationWidget, &ActivationWidget::activationSuccessful,
                             [&]() { activated = true; activationWidget.close(); });
            QObject::connect(&activationWidget, &ActivationWidget::skipActivation,
                             [&]() { skipped = true; activationWidget.close(); });

            activationWidget.show();
            app.exec();

            if (!activated && !skipped) {
                return 0; // User closed window
            }
        }

        // Show login
        LoginWidget loginWidget;
        loginWidget.setWindowTitle(QString::fromUtf8("تسجيل الدخول - POS Cashier"));
        loginWidget.resize(500, 500);

        User loggedInUser;
        bool loggedIn = false;

        QObject::connect(&loginWidget, &LoginWidget::loginSuccessful,
                         [&](const User &user) {
            loggedInUser = user;
            loggedIn = true;
            loginWidget.close();
        });

        loginWidget.show();
        app.exec();

        if (!loggedIn) {
            return 0; // User closed window
        }

        // Show main window
        MainWindow mainWindow;
        mainWindow.setCurrentUser(loggedInUser);
        mainWindow.showMaximized();

        exitCode = app.exec();

    } while (exitCode == 1000); // Restart code for logout

    return 0;
}
