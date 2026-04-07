#include "category.h"

Category::Category()
    : m_id(0)
{
}

Category::Category(const QString &name, const QString &description)
    : m_id(0), m_name(name), m_description(description)
{
}
