#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QSettings>

class Config
{
public:
    static Config& instance();

    // Firebase settings
    QString firebaseUrl() const;
    void setFirebaseUrl(const QString &url);
    QString firebaseApiKey() const;
    void setFirebaseApiKey(const QString &key);

    // Store settings
    QString storeName() const;
    void setStoreName(const QString &name);
    QString storeAddress() const;
    void setStoreAddress(const QString &address);
    QString storePhone() const;
    void setStorePhone(const QString &phone);

    // Printer settings
    QString thermalPrinterPort() const;
    void setThermalPrinterPort(const QString &port);
    int thermalPrinterBaudRate() const;
    void setThermalPrinterBaudRate(int rate);
    bool useThermalPrinter() const;
    void setUseThermalPrinter(bool use);

    // Tax settings
    double taxRate() const;
    void setTaxRate(double rate);

    // Activation
    bool isActivated() const;
    void setActivated(bool activated);
    QString licenseKey() const;
    void setLicenseKey(const QString &key);
    QString machineId() const;

    // Currency
    QString currencySymbol() const;
    void setCurrencySymbol(const QString &symbol);

private:
    Config();
    ~Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    QSettings m_settings;
    QString generateMachineId() const;
};

#endif // CONFIG_H
