#ifndef PRODUCTSWIDGET_H
#define PRODUCTSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

class ProductsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProductsWidget(QWidget *parent = nullptr);

public slots:
    void refreshTable();

signals:
    void productsChanged();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onSearchChanged(const QString &text);

private:
    void setupUi();

    QTableWidget *m_table;
    QPushButton *m_addBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
    QLineEdit *m_searchEdit;
};

#endif // PRODUCTSWIDGET_H
