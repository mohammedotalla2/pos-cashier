#include "config.h"
#include <QCryptographicHash>
#include <QSysInfo>
#include <QNetworkInterface>

Config& Config::instance()
{
    static Config instance;
    return instance;
}

Config::Config()
    : m_settings("POSCashier", "POSCashier")
{
}

// Firebase
QString Config::firebaseUrl() const
{
    return m_settings.value("firebase/url", "").toString();
}

void Config::setFirebaseUrl(const QString &url)
{
    m_settings.setValue("firebase/url", url);
}

QString Config::firebaseApiKey() const
{
    return m_settings.value("firebase/apiKey", "").toString();
}

void Config::setFirebaseApiKey(const QString &key)
{
    m_settings.setValue("firebase/apiKey", key);
}

// Store
QString Config::storeName() const
{
    return m_settings.value("store/name", QString::fromUtf8("متجري")).toString();
}

void Config::setStoreName(const QString &name)
{
    m_settings.setValue("store/name", name);
}

QString Config::storeAddress() const
{
    return m_settings.value("store/address", "").toString();
}

void Config::setStoreAddress(const QString &address)
{
    m_settings.setValue("store/address", address);
}

QString Config::storePhone() const
{
    return m_settings.value("store/phone", "").toString();
}

void Config::setStorePhone(const QString &phone)
{
    m_settings.setValue("store/phone", phone);
}

// Printer
QString Config::thermalPrinterPort() const
{
    return m_settings.value("printer/thermalPort", "COM1").toString();
}

void Config::setThermalPrinterPort(const QString &port)
{
    m_settings.setValue("printer/thermalPort", port);
}

int Config::thermalPrinterBaudRate() const
{
    return m_settings.value("printer/baudRate", 9600).toInt();
}

void Config::setThermalPrinterBaudRate(int rate)
{
    m_settings.setValue("printer/baudRate", rate);
}

bool Config::useThermalPrinter() const
{
    return m_settings.value("printer/useThermal", false).toBool();
}

void Config::setUseThermalPrinter(bool use)
{
    m_settings.setValue("printer/useThermal", use);
}

// Tax
double Config::taxRate() const
{
    return m_settings.value("tax/rate", 15.0).toDouble();
}

void Config::setTaxRate(double rate)
{
    m_settings.setValue("tax/rate", rate);
}

// Activation
bool Config::isActivated() const
{
    return m_settings.value("activation/activated", false).toBool();
}

void Config::setActivated(bool activated)
{
    m_settings.setValue("activation/activated", activated);
}

QString Config::licenseKey() const
{
    return m_settings.value("activation/licenseKey", "").toString();
}

void Config::setLicenseKey(const QString &key)
{
    m_settings.setValue("activation/licenseKey", key);
}

QString Config::machineId() const
{
    return generateMachineId();
}

QString Config::currencySymbol() const
{
    return m_settings.value("currency/symbol", QString::fromUtf8("ر.س")).toString();
}

void Config::setCurrencySymbol(const QString &symbol)
{
    m_settings.setValue("currency/symbol", symbol);
}

QString Config::generateMachineId() const
{
    QString data;
    data += QSysInfo::machineUniqueId();
    data += QSysInfo::machineHostName();

    // Add MAC address
    const auto interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        if (!(iface.flags() & QNetworkInterface::IsLoopBack)) {
            data += iface.hardwareAddress();
            break;
        }
    }

    QByteArray hash = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex().left(32);
}
