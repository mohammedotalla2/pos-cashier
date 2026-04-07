#ifndef THERMALPRINTER_H
#define THERMALPRINTER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include "models/sale.h"

// ESC/POS Commands
namespace ESCPOS {
    const QByteArray INIT = QByteArray("\x1B\x40", 2);           // Initialize printer
    const QByteArray CUT = QByteArray("\x1D\x56\x00", 3);       // Full cut
    const QByteArray PARTIAL_CUT = QByteArray("\x1D\x56\x01", 3); // Partial cut
    const QByteArray FEED = QByteArray("\x0A", 1);               // Line feed
    const QByteArray BOLD_ON = QByteArray("\x1B\x45\x01", 3);   // Bold on
    const QByteArray BOLD_OFF = QByteArray("\x1B\x45\x00", 3);  // Bold off
    const QByteArray CENTER = QByteArray("\x1B\x61\x01", 3);    // Center alignment
    const QByteArray LEFT = QByteArray("\x1B\x61\x00", 3);      // Left alignment
    const QByteArray RIGHT = QByteArray("\x1B\x61\x02", 3);     // Right alignment
    const QByteArray DOUBLE_HEIGHT = QByteArray("\x1B\x21\x10", 3); // Double height
    const QByteArray DOUBLE_WIDTH = QByteArray("\x1B\x21\x20", 3);  // Double width
    const QByteArray NORMAL_SIZE = QByteArray("\x1B\x21\x00", 3);   // Normal size
    const QByteArray LARGE = QByteArray("\x1B\x21\x30", 3);     // Double height+width
    const QByteArray CODEPAGE_ARABIC = QByteArray("\x1B\x74\x16", 3); // Arabic code page
    const QByteArray OPEN_DRAWER = QByteArray("\x1B\x70\x00\x19\xFA", 5); // Open cash drawer
}

class ThermalPrinter : public QObject
{
    Q_OBJECT

public:
    explicit ThermalPrinter(QObject *parent = nullptr);

    bool open(const QString &portName, int baudRate = 9600);
    void close();
    bool isOpen() const;

    // Low-level commands
    void initialize();
    void feed(int lines = 1);
    void cut();
    void partialCut();
    void openDrawer();

    // Text formatting
    void setBold(bool bold);
    void setAlignment(int align); // 0=left, 1=center, 2=right
    void setTextSize(int width, int height); // 1-8

    // Printing
    void printText(const QString &text);
    void printLine(const QString &text);
    void printReceipt(const Sale &sale);

    // Direct write
    void writeRaw(const QByteArray &data);

signals:
    void errorOccurred(const QString &error);
    void printCompleted();

private:
    void sendData(const QByteArray &data);

    QString m_portName;
    int m_baudRate = 9600;
    bool m_isOpen = false;

#ifdef Q_OS_WIN
    void *m_handle = nullptr; // HANDLE for Windows serial port
#else
    int m_fd = -1; // File descriptor for Linux serial port
#endif
};

#endif // THERMALPRINTER_H
