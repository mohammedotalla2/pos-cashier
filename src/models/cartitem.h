#ifndef CARTITEM_H
#define CARTITEM_H

#include "product.h"

class CartItem
{
public:
    CartItem();
    CartItem(const Product &product, int quantity = 1, double discount = 0.0);

    Product product() const { return m_product; }
    void setProduct(const Product &product) { m_product = product; }

    int quantity() const { return m_quantity; }
    void setQuantity(int qty) { m_quantity = qty; }

    double discount() const { return m_discount; }
    void setDiscount(double discount) { m_discount = discount; }

    double subtotal() const;
    double totalAfterDiscount() const;

private:
    Product m_product;
    int m_quantity = 1;
    double m_discount = 0.0; // percentage
};

#endif // CARTITEM_H
