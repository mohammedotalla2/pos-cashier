#include "sale.h"

Sale::Sale()
    : m_id(0), m_userId(0), m_dateTime(QDateTime::currentDateTime()),
      m_subtotal(0.0), m_tax(0.0), m_discount(0.0),
      m_total(0.0), m_amountPaid(0.0), m_paymentMethod("cash")
{
}

double Sale::profit() const
{
    double totalProfit = 0.0;
    for (const SaleItem &item : m_items) {
        totalProfit += (item.unitPrice - item.cost) * item.quantity;
    }
    return totalProfit;
}
