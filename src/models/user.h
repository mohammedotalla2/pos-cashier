#ifndef USER_H
#define USER_H

#include <QString>

enum class UserRole {
    Admin = 0,
    Cashier = 1
};

class User
{
public:
    User();
    User(const QString &username, const QString &password,
         const QString &fullName, UserRole role);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    QString username() const { return m_username; }
    void setUsername(const QString &username) { m_username = username; }

    QString password() const { return m_password; }
    void setPassword(const QString &password) { m_password = password; }

    QString fullName() const { return m_fullName; }
    void setFullName(const QString &fullName) { m_fullName = fullName; }

    UserRole role() const { return m_role; }
    void setRole(UserRole role) { m_role = role; }

    bool isActive() const { return m_isActive; }
    void setActive(bool active) { m_isActive = active; }

    bool isValid() const { return m_id > 0; }

    static QString hashPassword(const QString &password);
    static QString roleToString(UserRole role);
    static UserRole roleFromString(const QString &role);

private:
    int m_id = 0;
    QString m_username;
    QString m_password;
    QString m_fullName;
    UserRole m_role = UserRole::Cashier;
    bool m_isActive = true;
};

#endif // USER_H
