#include "firebasemanager.h"
#include "utils/config.h"
#include <QJsonArray>
#include <QUrlQuery>
#include <QDateTime>

FirebaseManager::FirebaseManager(QObject *parent)
    : QObject(parent),
      m_networkManager(new QNetworkAccessManager(this))
{
}

void FirebaseManager::validateLicense(const QString &licenseKey, const QString &machineId)
{
    QString firebaseUrl = Config::instance().firebaseUrl();
    if (firebaseUrl.isEmpty()) {
        emit licenseInvalid(QString::fromUtf8("لم يتم تكوين رابط Firebase"));
        return;
    }

    m_pendingMachineId = machineId;
    m_pendingLicenseKey = licenseKey;

    QString url = firebaseUrl + "/licenses/" + licenseKey + ".json";

    QUrl requestUrl(url);
    QNetworkRequest request{requestUrl};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onValidateReply(reply);
    });
}

void FirebaseManager::activateLicense(const QString &licenseKey, const QString &machineId)
{
    QString firebaseUrl = Config::instance().firebaseUrl();
    if (firebaseUrl.isEmpty()) {
        emit activationFailed(QString::fromUtf8("لم يتم تكوين رابط Firebase"));
        return;
    }

    m_pendingMachineId = machineId;
    m_pendingLicenseKey = licenseKey;

    // Update the license with machine ID
    QString url = firebaseUrl + "/licenses/" + licenseKey + ".json";

    QJsonObject data;
    data["machineId"] = machineId;
    data["activated"] = true;
    data["activationDate"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QUrl requestUrl(url);
    QNetworkRequest request{requestUrl};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager->put(request, QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onActivateReply(reply);
    });
}

void FirebaseManager::checkLicenseExpiry(const QString &licenseKey)
{
    QString firebaseUrl = Config::instance().firebaseUrl();
    if (firebaseUrl.isEmpty())
        return;

    QString url = firebaseUrl + "/licenses/" + licenseKey + ".json";

    QUrl requestUrl(url);
    QNetworkRequest request{requestUrl};
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();

        if (obj.contains("expiryDate")) {
            QDateTime expiry = QDateTime::fromString(obj["expiryDate"].toString(), Qt::ISODate);
            if (expiry < QDateTime::currentDateTime()) {
                emit licenseInvalid(QString::fromUtf8("انتهت صلاحية الترخيص"));
                Config::instance().setActivated(false);
            }
        }

        reply->deleteLater();
    });
}

void FirebaseManager::onValidateReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit networkError(QString::fromUtf8("خطأ في الاتصال: ") + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

    if (doc.isNull() || doc.object().isEmpty()) {
        emit licenseInvalid(QString::fromUtf8("مفتاح الترخيص غير صالح"));
        reply->deleteLater();
        return;
    }

    QJsonObject obj = doc.object();

    // Check if already activated with different machine
    if (obj.contains("machineId") && obj["activated"].toBool()) {
        QString storedMachineId = obj["machineId"].toString();
        if (!storedMachineId.isEmpty() && storedMachineId != m_pendingMachineId) {
            emit licenseInvalid(QString::fromUtf8("الترخيص مفعل على جهاز آخر"));
            reply->deleteLater();
            return;
        }
    }

    // Check expiry
    if (obj.contains("expiryDate")) {
        QDateTime expiry = QDateTime::fromString(obj["expiryDate"].toString(), Qt::ISODate);
        if (expiry < QDateTime::currentDateTime()) {
            emit licenseInvalid(QString::fromUtf8("انتهت صلاحية الترخيص"));
            reply->deleteLater();
            return;
        }
    }

    emit licenseValid(QString::fromUtf8("الترخيص صالح"));
    reply->deleteLater();
}

void FirebaseManager::onActivateReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit activationFailed(QString::fromUtf8("فشل التفعيل: ") + reply->errorString());
        reply->deleteLater();
        return;
    }

    Config::instance().setActivated(true);
    Config::instance().setLicenseKey(m_pendingLicenseKey);
    emit activationSuccess(QString::fromUtf8("تم التفعيل بنجاح!"));
    reply->deleteLater();
}
