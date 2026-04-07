#include "cartitem.h"

CartItem::CartItem()
    : m_quantity(1), m_discount(0.0)
{
}

CartItem::CartItem(const Product &product, int quantity, double discount)
    : m_product(product), m_quantity(quantity), m_discount(discount)
{
}

double CartItem::subtotal() const
{
    return m_product.price() * m_quantity;
}

double CartItem::totalAfterDiscount() const
{
    double sub = subtotal();
    return sub - (sub * m_discount / 100.0);
}
