#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QList>
#include <QDateTime>
#include "models/user.h"
#include "models/product.h"
#include "models/category.h"
#include "models/sale.h"

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    bool initialize();

    // Users
    bool addUser(const User &user);
    bool updateUser(const User &user);
    bool deleteUser(int id);
    User getUserById(int id);
    User getUserByUsername(const QString &username);
    QList<User> getAllUsers();
    bool authenticateUser(const QString &username, const QString &password);
    int getUserCount();

    // Categories
    bool addCategory(const Category &category);
    bool updateCategory(const Category &category);
    bool deleteCategory(int id);
    Category getCategoryById(int id);
    QList<Category> getAllCategories();

    // Products
    bool addProduct(const Product &product);
    bool updateProduct(const Product &product);
    bool deleteProduct(int id);
    Product getProductById(int id);
    Product getProductByBarcode(const QString &barcode);
    QList<Product> getAllProducts();
    QList<Product> getProductsByCategory(int categoryId);
    QList<Product> searchProducts(const QString &keyword);
    bool updateProductStock(int productId, int quantity);

    // Sales
    bool addSale(Sale &sale);
    Sale getSaleById(int id);
    QList<Sale> getSalesByDateRange(const QDateTime &from, const QDateTime &to);
    QList<Sale> getAllSales();
    double getTotalSalesAmount(const QDateTime &from, const QDateTime &to);
    int getTotalSalesCount(const QDateTime &from, const QDateTime &to);
    double getTotalProfit(const QDateTime &from, const QDateTime &to);

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();
    bool createDefaultAdmin();

    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
