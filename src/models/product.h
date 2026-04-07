#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>

class Product
{
public:
    Product();
    Product(const QString &name, const QString &barcode,
            double price, double cost, int stock, int categoryId);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString barcode() const { return m_barcode; }
    void setBarcode(const QString &barcode) { m_barcode = barcode; }

    double price() const { return m_price; }
    void setPrice(double price) { m_price = price; }

    double cost() const { return m_cost; }
    void setCost(double cost) { m_cost = cost; }

    int stock() const { return m_stock; }
    void setStock(int stock) { m_stock = stock; }

    int categoryId() const { return m_categoryId; }
    void setCategoryId(int id) { m_categoryId = id; }

    QString description() const { return m_description; }
    void setDescription(const QString &desc) { m_description = desc; }

    bool isActive() const { return m_isActive; }
    void setActive(bool active) { m_isActive = active; }

    bool isValid() const { return m_id > 0; }

    double profit() const { return m_price - m_cost; }

private:
    int m_id = 0;
    QString m_name;
    QString m_barcode;
    double m_price = 0.0;
    double m_cost = 0.0;
    int m_stock = 0;
    int m_categoryId = 0;
    QString m_description;
    bool m_isActive = true;
};

#endif // PRODUCT_H
