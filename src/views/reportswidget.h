#ifndef REPORTSWIDGET_H
#define REPORTSWIDGET_H

#include <QWidget>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>

class ReportsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsWidget(QWidget *parent = nullptr);

private slots:
    void onGenerateReport();

private:
    void setupUi();
    void generateDailyReport(const QDate &date);
    void generateDateRangeReport(const QDate &from, const QDate &to);

    QComboBox *m_reportType;
    QDateEdit *m_fromDate;
    QDateEdit *m_toDate;
    QPushButton *m_generateBtn;
    QLabel *m_reportContent;
};

#endif // REPORTSWIDGET_H
