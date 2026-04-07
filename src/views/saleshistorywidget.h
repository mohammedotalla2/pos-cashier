#ifndef SALESHISTORYWIDGET_H
#define SALESHISTORYWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>

class SalesHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SalesHistoryWidget(QWidget *parent = nullptr);
    void refreshTable();

private slots:
    void onFilterClicked();
    void onViewDetailsClicked();
    void onReprintClicked();

private:
    void setupUi();

    QTableWidget *m_table;
    QDateEdit *m_fromDate;
    QDateEdit *m_toDate;
    QPushButton *m_filterBtn;
    QPushButton *m_viewBtn;
    QPushButton *m_reprintBtn;
    QLabel *m_summaryLabel;
};

#endif // SALESHISTORYWIDGET_H
