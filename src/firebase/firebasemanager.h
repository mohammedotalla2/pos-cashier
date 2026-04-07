#ifndef FIREBASEMANAGER_H
#define FIREBASEMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class FirebaseManager : public QObject
{
    Q_OBJECT

public:
    explicit FirebaseManager(QObject *parent = nullptr);

    void validateLicense(const QString &licenseKey, const QString &machineId);
    void activateLicense(const QString &licenseKey, const QString &machineId);
    void checkLicenseExpiry(const QString &licenseKey);

signals:
    void licenseValid(const QString &message);
    void licenseInvalid(const QString &error);
    void activationSuccess(const QString &message);
    void activationFailed(const QString &error);
    void networkError(const QString &error);

private slots:
    void onValidateReply(QNetworkReply *reply);
    void onActivateReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_pendingMachineId;
    QString m_pendingLicenseKey;
};

#endif // FIREBASEMANAGER_H
