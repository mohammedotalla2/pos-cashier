#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>

class Category
{
public:
    Category();
    Category(const QString &name, const QString &description = "");

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString description() const { return m_description; }
    void setDescription(const QString &description) { m_description = description; }

    bool isValid() const { return m_id > 0; }

private:
    int m_id = 0;
    QString m_name;
    QString m_description;
};

#endif // CATEGORY_H
