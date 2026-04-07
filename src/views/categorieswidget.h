#ifndef CATEGORIESWIDGET_H
#define CATEGORIESWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

class CategoriesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CategoriesWidget(QWidget *parent = nullptr);

public slots:
    void refreshTable();

signals:
    void categoriesChanged();

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

#endif // CATEGORIESWIDGET_H
