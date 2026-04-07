#ifndef GDIPRINTER_H
#define GDIPRINTER_H

#include <QObject>
#include <QPrinter>
#include "models/sale.h"

class GdiPrinter : public QObject
{
    Q_OBJECT

public:
    explicit GdiPrinter(QObject *parent = nullptr);

    void printReceipt(const Sale &sale);
    void printReceiptWithPreview(const Sale &sale);

    void setPrinterName(const QString &name);
    QString printerName() const;

signals:
    void printCompleted();
    void errorOccurred(const QString &error);

private:
    void renderReceipt(QPrinter *printer, const Sale &sale);
    QString m_printerName;
};

#endif // GDIPRINTER_H
