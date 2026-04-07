#include "thermalprinter.h"
#include "receiptformatter.h"
#include <QDebug>


#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#endif

ThermalPrinter::ThermalPrinter(QObject *parent)
    : QObject(parent)
{
}

bool ThermalPrinter::open(const QString &portName, int baudRate)
{
    m_portName = portName;
    m_baudRate = baudRate;

#ifdef Q_OS_WIN
    // Windows serial port
    QString fullPort = "\\\\.\\" + portName;
    m_handle = CreateFileW(
        fullPort.toStdWString().c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (m_handle == INVALID_HANDLE_VALUE) {
        emit errorOccurred(QString::fromUtf8("فشل فتح المنفذ: %1").arg(portName));
        return false;
    }

    // Configure serial port
    DCB dcb = {0};
    dcb.DCBlength = sizeof(DCB);
    GetCommState(m_handle, &dcb);
    dcb.BaudRate = baudRate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    SetCommState(m_handle, &dcb);

    // Set timeouts
    COMMTIMEOUTS timeouts = {0};
    timeouts.WriteTotalTimeoutConstant = 5000;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(m_handle, &timeouts);
#else
    // Linux serial port
    m_fd = ::open(portName.toLocal8Bit().constData(), O_WRONLY | O_NOCTTY);
    if (m_fd < 0) {
        emit errorOccurred(QString::fromUtf8("فشل فتح المنفذ: %1 - %2")
            .arg(portName).arg(strerror(errno)));
        return false;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    tcgetattr(m_fd, &tty);

    speed_t speed;
    switch (baudRate) {
    case 9600: speed = B9600; break;
    case 19200: speed = B19200; break;
    case 38400: speed = B38400; break;
    case 115200: speed = B115200; break;
    default: speed = B9600; break;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;

    tcsetattr(m_fd, TCSANOW, &tty);
#endif

    m_isOpen = true;
    initialize();
    return true;
}

void ThermalPrinter::close()
{
    if (!m_isOpen) return;

#ifdef Q_OS_WIN
    if (m_handle) {
        CloseHandle(m_handle);
        m_handle = nullptr;
    }
#else
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
#endif

    m_isOpen = false;
}

bool ThermalPrinter::isOpen() const
{
    return m_isOpen;
}

void ThermalPrinter::sendData(const QByteArray &data)
{
    if (!m_isOpen) {
        emit errorOccurred(QString::fromUtf8("الطابعة غير متصلة"));
        return;
    }

#ifdef Q_OS_WIN
    DWORD bytesWritten;
    WriteFile(m_handle, data.constData(), data.size(), &bytesWritten, nullptr);
#else
    ::write(m_fd, data.constData(), data.size());
#endif
}

void ThermalPrinter::initialize()
{
    sendData(ESCPOS::INIT);
    sendData(ESCPOS::CODEPAGE_ARABIC);
}

void ThermalPrinter::feed(int lines)
{
    for (int i = 0; i < lines; i++)
        sendData(ESCPOS::FEED);
}

void ThermalPrinter::cut()
{
    feed(3);
    sendData(ESCPOS::CUT);
}

void ThermalPrinter::partialCut()
{
    feed(3);
    sendData(ESCPOS::PARTIAL_CUT);
}

void ThermalPrinter::openDrawer()
{
    sendData(ESCPOS::OPEN_DRAWER);
}

void ThermalPrinter::setBold(bool bold)
{
    sendData(bold ? ESCPOS::BOLD_ON : ESCPOS::BOLD_OFF);
}

void ThermalPrinter::setAlignment(int align)
{
    switch (align) {
    case 0: sendData(ESCPOS::LEFT); break;
    case 1: sendData(ESCPOS::CENTER); break;
    case 2: sendData(ESCPOS::RIGHT); break;
    }
}

void ThermalPrinter::setTextSize(int width, int height)
{
    int val = ((width - 1) << 4) | (height - 1);
    QByteArray cmd;
    cmd.append('\x1D');
    cmd.append('!');
    cmd.append(static_cast<char>(val));
    sendData(cmd);
}

void ThermalPrinter::printText(const QString &text)
{
    QByteArray encoded = text.toUtf8();
    sendData(encoded);
}

void ThermalPrinter::printLine(const QString &text)
{
    printText(text);
    sendData(ESCPOS::FEED);
}

void ThermalPrinter::printReceipt(const Sale &sale)
{
    if (!m_isOpen) {
        emit errorOccurred(QString::fromUtf8("الطابعة غير متصلة"));
        return;
    }

    initialize();

    ReceiptFormatter formatter;
    QStringList lines = formatter.formatReceiptLines(sale);

    // Print store name large and centered
    setAlignment(1);
    setBold(true);
    setTextSize(2, 2);
    printLine(formatter.formatReceiptLines(sale).first());
    setTextSize(1, 1);
    setBold(false);
    setAlignment(0);

    // Print rest of receipt
    for (int i = 1; i < lines.size(); i++) {
        printLine(lines[i]);
    }

    feed(2);
    cut();
    openDrawer();

    emit printCompleted();
}

void ThermalPrinter::writeRaw(const QByteArray &data)
{
    sendData(data);
}
