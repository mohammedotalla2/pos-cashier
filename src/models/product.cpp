#include "product.h"

Product::Product()
    : m_id(0), m_price(0.0), m_cost(0.0), m_stock(0),
      m_categoryId(0), m_isActive(true)
{
}

Product::Product(const QString &name, const QString &barcode,
                 double price, double cost, int stock, int categoryId)
    : m_id(0), m_name(name), m_barcode(barcode),
      m_price(price), m_cost(cost), m_stock(stock),
      m_categoryId(categoryId), m_isActive(true)
{
}
