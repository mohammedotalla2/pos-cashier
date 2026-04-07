#ifndef RECEIPTFORMATTER_H
#define RECEIPTFORMATTER_H

#include <QString>
#include <QStringList>
#include "models/sale.h"

class ReceiptFormatter
{
public:
    ReceiptFormatter();

    QString formatReceipt(const Sale &sale) const;
    QStringList formatReceiptLines(const Sale &sale) const;

    void setStoreInfo(const QString &name, const QString &address, const QString &phone);
    void setCurrencySymbol(const QString &symbol);
    void setLineWidth(int width);

private:
    QString centerText(const QString &text, int width) const;
    QString leftRightText(const QString &left, const QString &right, int width) const;
    QString repeatChar(QChar ch, int count) const;

    QString m_storeName;
    QString m_storeAddress;
    QString m_storePhone;
    QString m_currencySymbol;
    int m_lineWidth = 42; // Standard 80mm thermal printer
};

#endif // RECEIPTFORMATTER_H
