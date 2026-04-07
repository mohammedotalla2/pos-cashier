#include "saleshistorywidget.h"
#include "database/databasemanager.h"
#include "printing/gdiprinter.h"
#include "utils/config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>

SalesHistoryWidget::SalesHistoryWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    refreshTable();
}

void SalesHistoryWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel(QString::fromUtf8("سجل المبيعات"));
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1976D2; padding: 10px;");
    mainLayout->addWidget(title);

    // Filter bar
    QHBoxLayout *filterLayout = new QHBoxLayout();

    filterLayout->addWidget(new QLabel(QString::fromUtf8("من:")));
    m_fromDate = new QDateEdit(QDate::currentDate().addDays(-30));
    m_fromDate->setCalendarPopup(true);
    m_fromDate->setStyleSheet("padding: 8px; font-size: 14px;");
    filterLayout->addWidget(m_fromDate);

    filterLayout->addWidget(new QLabel(QString::fromUtf8("إلى:")));
    m_toDate = new QDateEdit(QDate::currentDate());
    m_toDate->setCalendarPopup(true);
    m_toDate->setStyleSheet("padding: 8px; font-size: 14px;");
    filterLayout->addWidget(m_toDate);

    m_filterBtn = new QPushButton(QString::fromUtf8("بحث"));
    m_filterBtn->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #1976D2; }");
    connect(m_filterBtn, &QPushButton::clicked, this, &SalesHistoryWidget::onFilterClicked);
    filterLayout->addWidget(m_filterBtn);

    filterLayout->addStretch();

    m_viewBtn = new QPushButton(QString::fromUtf8("عرض التفاصيل"));
    m_viewBtn->setStyleSheet(
        "QPushButton { background-color: #FF9800; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #e68900; }");
    connect(m_viewBtn, &QPushButton::clicked, this, &SalesHistoryWidget::onViewDetailsClicked);
    filterLayout->addWidget(m_viewBtn);

    m_reprintBtn = new QPushButton(QString::fromUtf8("إعادة طباعة"));
    m_reprintBtn->setStyleSheet(
        "QPushButton { background-color: #9C27B0; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #7B1FA2; }");
    connect(m_reprintBtn, &QPushButton::clicked, this, &SalesHistoryWidget::onReprintClicked);
    filterLayout->addWidget(m_reprintBtn);

    mainLayout->addLayout(filterLayout);

    // Table
    m_table = new QTableWidget();
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({
        QString::fromUtf8("رقم الفاتورة"),
        QString::fromUtf8("التاريخ"),
        QString::fromUtf8("الكاشير"),
        QString::fromUtf8("المجموع"),
        QString::fromUtf8("الخصم"),
        QString::fromUtf8("الضريبة"),
        QString::fromUtf8("الإجمالي")
    });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet(
        "QTableWidget { font-size: 14px; border: 1px solid #ddd; }"
        "QTableWidget::item { padding: 8px; }"
        "QHeaderView::section { background-color: #1976D2; color: white; padding: 8px; "
        "font-size: 14px; font-weight: bold; border: none; }");
    mainLayout->addWidget(m_table);

    // Summary
    m_summaryLabel = new QLabel();
    m_summaryLabel->setStyleSheet(
        "font-size: 16px; font-weight: bold; padding: 10px; "
        "background-color: #E3F2FD; border-radius: 6px;");
    mainLayout->addWidget(m_summaryLabel);
}

void SalesHistoryWidget::refreshTable()
{
    onFilterClicked();
}

void SalesHistoryWidget::onFilterClicked()
{
    QDateTime from(m_fromDate->date(), QTime(0, 0, 0));
    QDateTime to(m_toDate->date(), QTime(23, 59, 59));

    auto sales = DatabaseManager::instance().getSalesByDateRange(from, to);
    m_table->setRowCount(sales.size());
    QString currency = Config::instance().currencySymbol();

    double totalAmount = 0;

    for (int i = 0; i < sales.size(); i++) {
        const Sale &sale = sales[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(sale.id())));
        m_table->setItem(i, 1, new QTableWidgetItem(
            sale.dateTime().toString("yyyy/MM/dd hh:mm")));
        m_table->setItem(i, 2, new QTableWidgetItem(sale.cashierName()));
        m_table->setItem(i, 3, new QTableWidgetItem(
            QString("%1").arg(sale.subtotal(), 0, 'f', 2)));
        m_table->setItem(i, 4, new QTableWidgetItem(
            QString("%1").arg(sale.discount(), 0, 'f', 2)));
        m_table->setItem(i, 5, new QTableWidgetItem(
            QString("%1").arg(sale.tax(), 0, 'f', 2)));
        m_table->setItem(i, 6, new QTableWidgetItem(
            QString("%1 %2").arg(sale.total(), 0, 'f', 2).arg(currency)));

        totalAmount += sale.total();
    }

    m_summaryLabel->setText(QString::fromUtf8(
        "عدد الفواتير: %1 | إجمالي المبيعات: %2 %3")
        .arg(sales.size())
        .arg(totalAmount, 0, 'f', 2)
        .arg(currency));
}

void SalesHistoryWidget::onViewDetailsClicked()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, QString::fromUtf8("تنبيه"),
                             QString::fromUtf8("يرجى اختيار فاتورة لعرض التفاصيل"));
        return;
    }

    int saleId = m_table->item(row, 0)->text().toInt();
    Sale sale = DatabaseManager::instance().getSaleById(saleId);
    if (!sale.isValid()) return;

    QString currency = Config::instance().currencySymbol();

    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("تفاصيل الفاتورة #%1").arg(saleId));
    dialog.setLayoutDirection(Qt::RightToLeft);
    dialog.setMinimumSize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // Sale info
    QLabel *infoLabel = new QLabel(QString::fromUtf8(
        "التاريخ: %1 | الكاشير: %2 | طريقة الدفع: %3")
        .arg(sale.dateTime().toString("yyyy/MM/dd hh:mm"))
        .arg(sale.cashierName())
        .arg(sale.paymentMethod() == "cash" ?
             QString::fromUtf8("نقدي") : QString::fromUtf8("بطاقة")));
    infoLabel->setStyleSheet("font-size: 14px; padding: 10px;");
    layout->addWidget(infoLabel);

    // Items table
    QTableWidget *itemsTable = new QTableWidget();
    itemsTable->setColumnCount(5);
    itemsTable->setHorizontalHeaderLabels({
        QString::fromUtf8("المنتج"),
        QString::fromUtf8("الكمية"),
        QString::fromUtf8("السعر"),
        QString::fromUtf8("الخصم %"),
        QString::fromUtf8("الإجمالي")
    });
    itemsTable->horizontalHeader()->setStretchLastSection(true);
    itemsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    itemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    itemsTable->setStyleSheet(
        "QTableWidget { font-size: 13px; }"
        "QHeaderView::section { background-color: #FF9800; color: white; padding: 6px; }");

    const auto items = sale.items();
    itemsTable->setRowCount(items.size());
    for (int i = 0; i < items.size(); i++) {
        const SaleItem &item = items[i];
        itemsTable->setItem(i, 0, new QTableWidgetItem(item.productName));
        itemsTable->setItem(i, 1, new QTableWidgetItem(QString::number(item.quantity)));
        itemsTable->setItem(i, 2, new QTableWidgetItem(
            QString("%1").arg(item.unitPrice, 0, 'f', 2)));
        itemsTable->setItem(i, 3, new QTableWidgetItem(
            QString("%1").arg(item.discount, 0, 'f', 1)));
        itemsTable->setItem(i, 4, new QTableWidgetItem(
            QString("%1 %2").arg(item.total, 0, 'f', 2).arg(currency)));
    }
    layout->addWidget(itemsTable);

    // Totals
    QLabel *totalsLabel = new QLabel(QString::fromUtf8(
        "المجموع: %1 | الخصم: %2 | الضريبة: %3 | الإجمالي: %4 | المدفوع: %5 | الباقي: %6 %7")
        .arg(sale.subtotal(), 0, 'f', 2)
        .arg(sale.discount(), 0, 'f', 2)
        .arg(sale.tax(), 0, 'f', 2)
        .arg(sale.total(), 0, 'f', 2)
        .arg(sale.amountPaid(), 0, 'f', 2)
        .arg(sale.change(), 0, 'f', 2)
        .arg(currency));
    totalsLabel->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px; "
                               "background-color: #E8F5E9; border-radius: 6px;");
    totalsLabel->setWordWrap(true);
    layout->addWidget(totalsLabel);

    dialog.exec();
}

void SalesHistoryWidget::onReprintClicked()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, QString::fromUtf8("تنبيه"),
                             QString::fromUtf8("يرجى اختيار فاتورة لإعادة الطباعة"));
        return;
    }

    int saleId = m_table->item(row, 0)->text().toInt();
    Sale sale = DatabaseManager::instance().getSaleById(saleId);
    if (!sale.isValid()) return;

    GdiPrinter printer;
    printer.printReceiptWithPreview(sale);
}
