#include "reportswidget.h"
#include "database/databasemanager.h"
#include "utils/config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QDateTime>

ReportsWidget::ReportsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void ReportsWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel(QString::fromUtf8("التقارير"));
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1976D2; padding: 10px;");
    mainLayout->addWidget(title);

    // Filter bar
    QHBoxLayout *filterLayout = new QHBoxLayout();

    m_reportType = new QComboBox();
    m_reportType->addItem(QString::fromUtf8("تقرير يومي"), "daily");
    m_reportType->addItem(QString::fromUtf8("تقرير لفترة"), "range");
    m_reportType->setStyleSheet("padding: 8px; font-size: 14px;");
    filterLayout->addWidget(m_reportType);

    filterLayout->addWidget(new QLabel(QString::fromUtf8("من:")));
    m_fromDate = new QDateEdit(QDate::currentDate());
    m_fromDate->setCalendarPopup(true);
    m_fromDate->setStyleSheet("padding: 8px; font-size: 14px;");
    filterLayout->addWidget(m_fromDate);

    filterLayout->addWidget(new QLabel(QString::fromUtf8("إلى:")));
    m_toDate = new QDateEdit(QDate::currentDate());
    m_toDate->setCalendarPopup(true);
    m_toDate->setStyleSheet("padding: 8px; font-size: 14px;");
    filterLayout->addWidget(m_toDate);

    m_generateBtn = new QPushButton(QString::fromUtf8("إنشاء التقرير"));
    m_generateBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 10px 25px; "
        "font-size: 14px; font-weight: bold; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #45a049; }");
    connect(m_generateBtn, &QPushButton::clicked, this, &ReportsWidget::onGenerateReport);
    filterLayout->addWidget(m_generateBtn);

    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);

    // Report content
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border: 1px solid #ddd; border-radius: 6px;");

    m_reportContent = new QLabel();
    m_reportContent->setAlignment(Qt::AlignTop | Qt::AlignRight);
    m_reportContent->setWordWrap(true);
    m_reportContent->setStyleSheet(
        "font-size: 14px; padding: 20px; background: white;");
    m_reportContent->setTextFormat(Qt::RichText);
    scrollArea->setWidget(m_reportContent);

    mainLayout->addWidget(scrollArea);
}

void ReportsWidget::onGenerateReport()
{
    QString type = m_reportType->currentData().toString();
    if (type == "daily") {
        generateDailyReport(m_fromDate->date());
    } else {
        generateDateRangeReport(m_fromDate->date(), m_toDate->date());
    }
}

void ReportsWidget::generateDailyReport(const QDate &date)
{
    QDateTime from(date, QTime(0, 0, 0));
    QDateTime to(date, QTime(23, 59, 59));

    DatabaseManager &db = DatabaseManager::instance();
    double totalSales = db.getTotalSalesAmount(from, to);
    int salesCount = db.getTotalSalesCount(from, to);
    double totalProfit = db.getTotalProfit(from, to);
    QString currency = Config::instance().currencySymbol();

    auto sales = db.getSalesByDateRange(from, to);

    double cashTotal = 0, cardTotal = 0;
    for (const Sale &s : sales) {
        if (s.paymentMethod() == "cash")
            cashTotal += s.total();
        else
            cardTotal += s.total();
    }

    QString html = QString::fromUtf8(
        "<div style='text-align:center;'>"
        "<h2 style='color:#1976D2;'>تقرير يومي</h2>"
        "<h3>%1</h3>"
        "</div>"
        "<hr>"
        "<table width='100%' style='font-size:16px; border-collapse:collapse;'>"
        "<tr style='background:#E3F2FD;'>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>عدد الفواتير</td>"
        "  <td style='padding:12px; text-align:left;'>%2</td>"
        "</tr>"
        "<tr>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>إجمالي المبيعات</td>"
        "  <td style='padding:12px; text-align:left;'>%3 %4</td>"
        "</tr>"
        "<tr style='background:#E8F5E9;'>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>صافي الربح</td>"
        "  <td style='padding:12px; text-align:left; color:#4CAF50; font-weight:bold;'>%5 %6</td>"
        "</tr>"
        "<tr>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>مبيعات نقدية</td>"
        "  <td style='padding:12px; text-align:left;'>%7 %8</td>"
        "</tr>"
        "<tr style='background:#E3F2FD;'>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>مبيعات بطاقة</td>"
        "  <td style='padding:12px; text-align:left;'>%9 %10</td>"
        "</tr>"
        "</table>"
    ).arg(date.toString("yyyy/MM/dd"))
     .arg(salesCount)
     .arg(totalSales, 0, 'f', 2).arg(currency)
     .arg(totalProfit, 0, 'f', 2).arg(currency)
     .arg(cashTotal, 0, 'f', 2).arg(currency)
     .arg(cardTotal, 0, 'f', 2).arg(currency);

    m_reportContent->setText(html);
}

void ReportsWidget::generateDateRangeReport(const QDate &from, const QDate &to)
{
    QDateTime fromDt(from, QTime(0, 0, 0));
    QDateTime toDt(to, QTime(23, 59, 59));

    DatabaseManager &db = DatabaseManager::instance();
    double totalSales = db.getTotalSalesAmount(fromDt, toDt);
    int salesCount = db.getTotalSalesCount(fromDt, toDt);
    double totalProfit = db.getTotalProfit(fromDt, toDt);
    QString currency = Config::instance().currencySymbol();

    auto sales = db.getSalesByDateRange(fromDt, toDt);

    double cashTotal = 0, cardTotal = 0;
    double totalDiscount = 0, totalTax = 0;
    for (const Sale &s : sales) {
        if (s.paymentMethod() == "cash")
            cashTotal += s.total();
        else
            cardTotal += s.total();
        totalDiscount += s.discount();
        totalTax += s.tax();
    }

    int days = from.daysTo(to) + 1;
    double avgDaily = days > 0 ? totalSales / days : 0;

    QString html = QString::fromUtf8(
        "<div style='text-align:center;'>"
        "<h2 style='color:#1976D2;'>تقرير لفترة</h2>"
        "<h3>من %1 إلى %2</h3>"
        "</div>"
        "<hr>"
        "<table width='100%' style='font-size:16px; border-collapse:collapse;'>"
        "<tr style='background:#E3F2FD;'>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>عدد الأيام</td>"
        "  <td style='padding:12px; text-align:left;'>%3</td>"
        "</tr>"
        "<tr>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>عدد الفواتير</td>"
        "  <td style='padding:12px; text-align:left;'>%4</td>"
        "</tr>"
        "<tr style='background:#E3F2FD;'>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>إجمالي المبيعات</td>"
        "  <td style='padding:12px; text-align:left;'>%5 %6</td>"
        "</tr>"
        "<tr>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>إجمالي الخصومات</td>"
        "  <td style='padding:12px; text-align:left; color:#f44336;'>%7 %8</td>"
        "</tr>"
        "<tr style='background:#E3F2FD;'>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>إجمالي الضرائب</td>"
        "  <td style='padding:12px; text-align:left;'>%9 %10</td>"
        "</tr>"
        "<tr style='background:#E8F5E9;'>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>صافي الربح</td>"
        "  <td style='padding:12px; text-align:left; color:#4CAF50; font-weight:bold;'>%11 %12</td>"
        "</tr>"
        "<tr>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>متوسط المبيعات اليومي</td>"
        "  <td style='padding:12px; text-align:left;'>%13 %14</td>"
        "</tr>"
        "<tr style='background:#E3F2FD;'>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>مبيعات نقدية</td>"
        "  <td style='padding:12px; text-align:left;'>%15 %16</td>"
        "</tr>"
        "<tr>"
        "  <td style='padding:12px; text-align:right; font-weight:bold;'>مبيعات بطاقة</td>"
        "  <td style='padding:12px; text-align:left;'>%17 %18</td>"
        "</tr>"
        "</table>"
    ).arg(from.toString("yyyy/MM/dd"))
     .arg(to.toString("yyyy/MM/dd"))
     .arg(days)
     .arg(salesCount)
     .arg(totalSales, 0, 'f', 2).arg(currency)
     .arg(totalDiscount, 0, 'f', 2).arg(currency)
     .arg(totalTax, 0, 'f', 2).arg(currency)
     .arg(totalProfit, 0, 'f', 2).arg(currency)
     .arg(avgDaily, 0, 'f', 2).arg(currency)
     .arg(cashTotal, 0, 'f', 2).arg(currency)
     .arg(cardTotal, 0, 'f', 2).arg(currency);

    m_reportContent->setText(html);
}
