#ifndef ACTIVATIONWIDGET_H
#define ACTIVATIONWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "firebase/firebasemanager.h"

class ActivationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActivationWidget(QWidget *parent = nullptr);

signals:
    void activationSuccessful();
    void skipActivation();

private slots:
    void onActivateClicked();
    void onLicenseValid(const QString &message);
    void onLicenseInvalid(const QString &error);
    void onActivationSuccess(const QString &message);
    void onActivationFailed(const QString &error);
    void onNetworkError(const QString &error);
    void onSkipClicked();

private:
    void setupUi();

    QLineEdit *m_firebaseUrlEdit;
    QLineEdit *m_licenseKeyEdit;
    QPushButton *m_activateBtn;
    QPushButton *m_skipBtn;
    QLabel *m_statusLabel;
    QLabel *m_machineIdLabel;

    FirebaseManager *m_firebase;
};

#endif // ACTIVATIONWIDGET_H
