#include "user.h"
#include <QCryptographicHash>

User::User()
    : m_id(0), m_role(UserRole::Cashier), m_isActive(true)
{
}

User::User(const QString &username, const QString &password,
           const QString &fullName, UserRole role)
    : m_id(0), m_username(username), m_password(password),
      m_fullName(fullName), m_role(role), m_isActive(true)
{
}

QString User::hashPassword(const QString &password)
{
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

QString User::roleToString(UserRole role)
{
    switch (role) {
    case UserRole::Admin:
        return "admin";
    case UserRole::Cashier:
        return "cashier";
    default:
        return "cashier";
    }
}

UserRole User::roleFromString(const QString &role)
{
    if (role.toLower() == "admin")
        return UserRole::Admin;
    return UserRole::Cashier;
}
