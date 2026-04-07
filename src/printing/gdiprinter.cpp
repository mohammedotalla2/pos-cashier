#include "gdiprinter.h"
#include "receiptformatter.h"
#include "utils/config.h"
#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QFont>
#include <QFontMetrics>

GdiPrinter::GdiPrinter(QObject *parent)
    : QObject(parent)
{
}

void GdiPrinter::setPrinterName(const QString &name)
{
    m_printerName = name;
}

QString GdiPrinter::printerName() const
{
    return m_printerName;
}

void GdiPrinter::printReceipt(const Sale &sale)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize(QPageSize::A4));

    if (!m_printerName.isEmpty())
        printer.setPrinterName(m_printerName);

    QPrintDialog dialog(&printer);
    dialog.setWindowTitle(QString::fromUtf8("طباعة الفاتورة"));

    if (dialog.exec() == QPrintDialog::Accepted) {
        renderReceipt(&printer, sale);
        emit printCompleted();
    }
}

void GdiPrinter::printReceiptWithPreview(const Sale &sale)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize(QPageSize::A4));

    if (!m_printerName.isEmpty())
        printer.setPrinterName(m_printerName);

    QPrintPreviewDialog preview(&printer);
    preview.setWindowTitle(QString::fromUtf8("معاينة الفاتورة"));

    connect(&preview, &QPrintPreviewDialog::paintRequested,
            this, [this, &sale](QPrinter *p) {
        renderReceipt(p, sale);
    });

    preview.exec();
    emit printCompleted();
}

void GdiPrinter::renderReceipt(QPrinter *printer, const Sale &sale)
{
    QPainter painter(printer);
    painter.setRenderHint(QPainter::Antialiasing);

    // Set RTL layout
    painter.setLayoutDirection(Qt::RightToLeft);

    int dpi = printer->resolution();
    int pageWidth = printer->pageLayout().paintRectPixels(dpi).width();

    // Fonts
    QFont titleFont("Arial", 18, QFont::Bold);
    QFont headerFont("Arial", 12, QFont::Bold);
    QFont normalFont("Arial", 10);
    QFont smallFont("Arial", 8);

    int y = 50;
    int lineHeight = 0;
    int margin = 30;

    // Store name
    painter.setFont(titleFont);
    QFontMetrics titleMetrics(titleFont);
    lineHeight = titleMetrics.height();
    QString storeName = Config::instance().storeName();
    painter.drawText(QRect(margin, y, pageWidth - 2 * margin, lineHeight),
                     Qt::AlignCenter, storeName);
    y += lineHeight + 10;

    // Store address
    painter.setFont(normalFont);
    QFontMetrics normalMetrics(normalFont);
    lineHeight = normalMetrics.height();
    QString address = Config::instance().storeAddress();
    if (!address.isEmpty()) {
        painter.drawText(QRect(margin, y, pageWidth - 2 * margin, lineHeight),
                         Qt::AlignCenter, address);
        y += lineHeight + 5;
    }

    // Store phone
    QString phone = Config::instance().storePhone();
    if (!phone.isEmpty()) {
        painter.drawText(QRect(margin, y, pageWidth - 2 * margin, lineHeight),
                         Qt::AlignCenter, phone);
        y += lineHeight + 5;
    }

    // Separator
    y += 10;
    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 15;

    // Invoice info
    painter.setFont(normalFont);
    painter.drawText(QRect(margin, y, pageWidth - 2 * margin, lineHeight),
                     Qt::AlignRight,
                     QString::fromUtf8("رقم الفاتورة: %1").arg(sale.id()));
    painter.drawText(QRect(margin, y, pageWidth - 2 * margin, lineHeight),
                     Qt::AlignLeft,
                     sale.dateTime().toString("yyyy/MM/dd hh:mm"));
    y += lineHeight + 5;

    painter.drawText(QRect(margin, y, pageWidth - 2 * margin, lineHeight),
                     Qt::AlignRight,
                     QString::fromUtf8("الكاشير: %1").arg(sale.cashierName()));
    y += lineHeight + 10;

    // Separator
    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 15;

    // Table header
    painter.setFont(headerFont);
    QFontMetrics headerMetrics(headerFont);
    int headerHeight = headerMetrics.height();

    int col1 = margin;                        // Product name
    int col2 = pageWidth * 0.45;              // Quantity
    int col3 = pageWidth * 0.60;              // Unit Price
    int col4 = pageWidth * 0.80;              // Total

    painter.drawText(QRect(col1, y, col2 - col1, headerHeight), Qt::AlignRight,
                     QString::fromUtf8("المنتج"));
    painter.drawText(QRect(col2, y, col3 - col2, headerHeight), Qt::AlignCenter,
                     QString::fromUtf8("الكمية"));
    painter.drawText(QRect(col3, y, col4 - col3, headerHeight), Qt::AlignCenter,
                     QString::fromUtf8("السعر"));
    painter.drawText(QRect(col4, y, pageWidth - margin - col4, headerHeight), Qt::AlignLeft,
                     QString::fromUtf8("الإجمالي"));
    y += headerHeight + 5;

    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 10;

    // Items
    painter.setFont(normalFont);
    QString currency = Config::instance().currencySymbol();
    const auto items = sale.items();
    for (const SaleItem &item : items) {
        painter.drawText(QRect(col1, y, col2 - col1, lineHeight), Qt::AlignRight,
                         item.productName);
        painter.drawText(QRect(col2, y, col3 - col2, lineHeight), Qt::AlignCenter,
                         QString::number(item.quantity));
        painter.drawText(QRect(col3, y, col4 - col3, lineHeight), Qt::AlignCenter,
                         QString("%1 %2").arg(item.unitPrice, 0, 'f', 2).arg(currency));
        painter.drawText(QRect(col4, y, pageWidth - margin - col4, lineHeight), Qt::AlignLeft,
                         QString("%1 %2").arg(item.total, 0, 'f', 2).arg(currency));
        y += lineHeight + 3;
    }

    // Separator
    y += 5;
    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 15;

    // Totals
    painter.setFont(normalFont);
    auto drawTotal = [&](const QString &label, const QString &value) {
        painter.drawText(QRect(margin, y, pageWidth - 2 * margin, lineHeight),
                         Qt::AlignRight, label);
        painter.drawText(QRect(margin, y, pageWidth - 2 * margin, lineHeight),
                         Qt::AlignLeft, value);
        y += lineHeight + 3;
    };

    drawTotal(QString::fromUtf8("المجموع الفرعي:"),
              QString("%1 %2").arg(sale.subtotal(), 0, 'f', 2).arg(currency));

    if (sale.discount() > 0) {
        drawTotal(QString::fromUtf8("الخصم:"),
                  QString("-%1 %2").arg(sale.discount(), 0, 'f', 2).arg(currency));
    }

    if (sale.tax() > 0) {
        drawTotal(QString::fromUtf8("الضريبة:"),
                  QString("%1 %2").arg(sale.tax(), 0, 'f', 2).arg(currency));
    }

    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 10;

    painter.setFont(headerFont);
    drawTotal(QString::fromUtf8("الإجمالي:"),
              QString("%1 %2").arg(sale.total(), 0, 'f', 2).arg(currency));

    painter.setFont(normalFont);
    drawTotal(QString::fromUtf8("المدفوع:"),
              QString("%1 %2").arg(sale.amountPaid(), 0, 'f', 2).arg(currency));

    if (sale.change() > 0) {
        drawTotal(QString::fromUtf8("الباقي:"),
                  QString("%1 %2").arg(sale.change(), 0, 'f', 2).arg(currency));
    }

    // Payment method
    y += 15;
    QString payMethod = sale.paymentMethod() == "cash" ?
        QString::fromUtf8("نقدي") : QString::fromUtf8("بطاقة");
    painter.drawText(QRect(margin, y, pageWidth - 2 * margin, lineHeight),
                     Qt::AlignCenter,
                     QString::fromUtf8("طريقة الدفع: %1").arg(payMethod));
    y += lineHeight + 20;

    // Footer
    painter.setFont(smallFont);
    QFontMetrics smallMetrics(smallFont);
    int smallHeight = smallMetrics.height();
    painter.drawText(QRect(margin, y, pageWidth - 2 * margin, smallHeight),
                     Qt::AlignCenter, QString::fromUtf8("شكراً لزيارتكم"));
    y += smallHeight + 5;
    painter.drawText(QRect(margin, y, pageWidth - 2 * margin, smallHeight),
                     Qt::AlignCenter, QString::fromUtf8("نتمنى لكم يوماً سعيداً"));

    painter.end();
}
