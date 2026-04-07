#include "receiptformatter.h"
#include "utils/config.h"
#include <QDateTime>

ReceiptFormatter::ReceiptFormatter()
{
    m_storeName = Config::instance().storeName();
    m_storeAddress = Config::instance().storeAddress();
    m_storePhone = Config::instance().storePhone();
    m_currencySymbol = Config::instance().currencySymbol();
}

void ReceiptFormatter::setStoreInfo(const QString &name, const QString &address, const QString &phone)
{
    m_storeName = name;
    m_storeAddress = address;
    m_storePhone = phone;
}

void ReceiptFormatter::setCurrencySymbol(const QString &symbol)
{
    m_currencySymbol = symbol;
}

void ReceiptFormatter::setLineWidth(int width)
{
    m_lineWidth = width;
}

QString ReceiptFormatter::centerText(const QString &text, int width) const
{
    if (text.length() >= width)
        return text;
    int padding = (width - text.length()) / 2;
    return QString(padding, ' ') + text;
}

QString ReceiptFormatter::leftRightText(const QString &left, const QString &right, int width) const
{
    int spaces = width - left.length() - right.length();
    if (spaces < 1) spaces = 1;
    return left + QString(spaces, ' ') + right;
}

QString ReceiptFormatter::repeatChar(QChar ch, int count) const
{
    return QString(count, ch);
}

QStringList ReceiptFormatter::formatReceiptLines(const Sale &sale) const
{
    QStringList lines;

    // Header
    lines << centerText(m_storeName, m_lineWidth);
    if (!m_storeAddress.isEmpty())
        lines << centerText(m_storeAddress, m_lineWidth);
    if (!m_storePhone.isEmpty())
        lines << centerText(m_storePhone, m_lineWidth);

    lines << repeatChar('=', m_lineWidth);

    // Sale info
    lines << leftRightText(
        QString::fromUtf8("رقم الفاتورة: %1").arg(sale.id()),
        sale.dateTime().toString("yyyy/MM/dd"), m_lineWidth);
    lines << leftRightText(
        QString::fromUtf8("الكاشير: %1").arg(sale.cashierName()),
        sale.dateTime().toString("hh:mm:ss"), m_lineWidth);

    lines << repeatChar('-', m_lineWidth);

    // Column headers
    lines << leftRightText(
        QString::fromUtf8("المنتج"),
        QString::fromUtf8("الإجمالي"), m_lineWidth);
    lines << repeatChar('-', m_lineWidth);

    // Items
    const auto items = sale.items();
    for (const SaleItem &item : items) {
        lines << item.productName;
        QString qtyPrice = QString("%1 x %2 %3")
            .arg(item.quantity)
            .arg(item.unitPrice, 0, 'f', 2)
            .arg(m_currencySymbol);
        QString total = QString("%1 %2")
            .arg(item.total, 0, 'f', 2)
            .arg(m_currencySymbol);
        lines << leftRightText(qtyPrice, total, m_lineWidth);

        if (item.discount > 0) {
            lines << leftRightText(
                QString::fromUtf8("  خصم: %1%").arg(item.discount, 0, 'f', 1),
                "", m_lineWidth);
        }
    }

    lines << repeatChar('=', m_lineWidth);

    // Totals
    lines << leftRightText(
        QString::fromUtf8("المجموع الفرعي:"),
        QString("%1 %2").arg(sale.subtotal(), 0, 'f', 2).arg(m_currencySymbol),
        m_lineWidth);

    if (sale.discount() > 0) {
        lines << leftRightText(
            QString::fromUtf8("الخصم:"),
            QString("-%1 %2").arg(sale.discount(), 0, 'f', 2).arg(m_currencySymbol),
            m_lineWidth);
    }

    if (sale.tax() > 0) {
        lines << leftRightText(
            QString::fromUtf8("الضريبة:"),
            QString("%1 %2").arg(sale.tax(), 0, 'f', 2).arg(m_currencySymbol),
            m_lineWidth);
    }

    lines << repeatChar('-', m_lineWidth);

    lines << leftRightText(
        QString::fromUtf8("الإجمالي:"),
        QString("%1 %2").arg(sale.total(), 0, 'f', 2).arg(m_currencySymbol),
        m_lineWidth);

    lines << leftRightText(
        QString::fromUtf8("المدفوع:"),
        QString("%1 %2").arg(sale.amountPaid(), 0, 'f', 2).arg(m_currencySymbol),
        m_lineWidth);

    if (sale.change() > 0) {
        lines << leftRightText(
            QString::fromUtf8("الباقي:"),
            QString("%1 %2").arg(sale.change(), 0, 'f', 2).arg(m_currencySymbol),
            m_lineWidth);
    }

    lines << repeatChar('=', m_lineWidth);

    // Payment method
    QString payMethod = sale.paymentMethod() == "cash" ?
        QString::fromUtf8("نقدي") : QString::fromUtf8("بطاقة");
    lines << centerText(QString::fromUtf8("طريقة الدفع: %1").arg(payMethod), m_lineWidth);

    // Footer
    lines << "";
    lines << centerText(QString::fromUtf8("شكراً لزيارتكم"), m_lineWidth);
    lines << centerText(QString::fromUtf8("نتمنى لكم يوماً سعيداً"), m_lineWidth);

    return lines;
}

QString ReceiptFormatter::formatReceipt(const Sale &sale) const
{
    return formatReceiptLines(sale).join("\n");
}
