#ifndef USERSWIDGET_H
#define USERSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

class UsersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UsersWidget(QWidget *parent = nullptr);

public slots:
    void refreshTable();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();

private:
    void setupUi();

    QTableWidget *m_table;
    QPushButton *m_addBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
};

#endif // USERSWIDGET_H
