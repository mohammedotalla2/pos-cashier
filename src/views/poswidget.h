#ifndef POSWIDGET_H
#define POSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QGridLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include "models/user.h"
#include "models/cartitem.h"
#include "models/product.h"
#include "printing/thermalprinter.h"
#include "printing/gdiprinter.h"

class PosWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PosWidget(QWidget *parent = nullptr);

    void setCurrentUser(const User &user);
    void refreshProducts();

signals:
    void saleCompleted();

private slots:
    void onBarcodeEntered();
    void onSearchTextChanged(const QString &text);
    void onProductClicked(int row, int col);
    void onRemoveItem();
    void onClearCart();
    void onQuantityChanged(int row, int col);
    void onPayClicked();
    void onCategoryFilterChanged(int index);

private:
    void setupUi();
    void addToCart(const Product &product);
    void updateCartDisplay();
    void updateTotals();
    void processPayment(double amountPaid, const QString &paymentMethod);
    void printReceipt(int saleId);
    void loadProducts(int categoryId = -1);
    void loadCategories();

    // UI Elements
    QLineEdit *m_barcodeEdit;
    QLineEdit *m_searchEdit;
    QTableWidget *m_productsTable;
    QTableWidget *m_cartTable;
    QComboBox *m_categoryFilter;

    QLabel *m_subtotalLabel;
    QLabel *m_taxLabel;
    QLabel *m_discountLabel;
    QLabel *m_totalLabel;

    QPushButton *m_payBtn;
    QPushButton *m_clearBtn;
    QPushButton *m_removeBtn;

    QDoubleSpinBox *m_discountSpin;

    // Data
    QList<CartItem> m_cartItems;
    User m_currentUser;
    double m_orderDiscount = 0.0;

    // Printers
    ThermalPrinter *m_thermalPrinter;
    GdiPrinter *m_gdiPrinter;
};

#endif // POSWIDGET_H
