#ifndef SALE_H
#define SALE_H

#include <QString>
#include <QDateTime>
#include <QList>

struct SaleItem {
    int id = 0;
    int saleId = 0;
    int productId = 0;
    QString productName;
    int quantity = 0;
    double unitPrice = 0.0;
    double cost = 0.0;
    double discount = 0.0;
    double total = 0.0;
};

class Sale
{
public:
    Sale();

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    int userId() const { return m_userId; }
    void setUserId(int id) { m_userId = id; }

    QString cashierName() const { return m_cashierName; }
    void setCashierName(const QString &name) { m_cashierName = name; }

    QDateTime dateTime() const { return m_dateTime; }
    void setDateTime(const QDateTime &dt) { m_dateTime = dt; }

    double subtotal() const { return m_subtotal; }
    void setSubtotal(double amount) { m_subtotal = amount; }

    double tax() const { return m_tax; }
    void setTax(double amount) { m_tax = amount; }

    double discount() const { return m_discount; }
    void setDiscount(double amount) { m_discount = amount; }

    double total() const { return m_total; }
    void setTotal(double amount) { m_total = amount; }

    double amountPaid() const { return m_amountPaid; }
    void setAmountPaid(double amount) { m_amountPaid = amount; }

    double change() const { return m_amountPaid - m_total; }

    QString paymentMethod() const { return m_paymentMethod; }
    void setPaymentMethod(const QString &method) { m_paymentMethod = method; }

    QList<SaleItem> items() const { return m_items; }
    void setItems(const QList<SaleItem> &items) { m_items = items; }
    void addItem(const SaleItem &item) { m_items.append(item); }

    double profit() const;

    bool isValid() const { return m_id > 0; }

private:
    int m_id = 0;
    int m_userId = 0;
    QString m_cashierName;
    QDateTime m_dateTime;
    double m_subtotal = 0.0;
    double m_tax = 0.0;
    double m_discount = 0.0;
    double m_total = 0.0;
    double m_amountPaid = 0.0;
    QString m_paymentMethod = "cash";
    QList<SaleItem> m_items;
};

#endif // SALE_H
