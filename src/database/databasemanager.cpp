#include "databasemanager.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}

bool DatabaseManager::initialize()
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists())
        dir.mkpath(".");

    QString dbPath = dataPath + "/pos_cashier.db";

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qCritical() << "فشل فتح قاعدة البيانات:" << m_db.lastError().text();
        return false;
    }

    if (!createTables()) {
        qCritical() << "فشل إنشاء الجداول";
        return false;
    }

    if (getUserCount() == 0) {
        createDefaultAdmin();
    }

    return true;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_db);

    // Users table
    bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT UNIQUE NOT NULL,"
        "  password TEXT NOT NULL,"
        "  full_name TEXT NOT NULL,"
        "  role TEXT NOT NULL DEFAULT 'cashier',"
        "  is_active INTEGER NOT NULL DEFAULT 1,"
        "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
    );
    if (!ok) {
        qCritical() << "فشل إنشاء جدول المستخدمين:" << query.lastError().text();
        return false;
    }

    // Categories table
    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS categories ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL,"
        "  description TEXT"
        ")"
    );
    if (!ok) return false;

    // Products table
    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS products ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL,"
        "  barcode TEXT UNIQUE,"
        "  price REAL NOT NULL DEFAULT 0,"
        "  cost REAL NOT NULL DEFAULT 0,"
        "  stock INTEGER NOT NULL DEFAULT 0,"
        "  category_id INTEGER,"
        "  description TEXT,"
        "  is_active INTEGER NOT NULL DEFAULT 1,"
        "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  FOREIGN KEY (category_id) REFERENCES categories(id)"
        ")"
    );
    if (!ok) return false;

    // Sales table
    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS sales ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  user_id INTEGER NOT NULL,"
        "  cashier_name TEXT,"
        "  date_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  subtotal REAL NOT NULL DEFAULT 0,"
        "  tax REAL NOT NULL DEFAULT 0,"
        "  discount REAL NOT NULL DEFAULT 0,"
        "  total REAL NOT NULL DEFAULT 0,"
        "  amount_paid REAL NOT NULL DEFAULT 0,"
        "  payment_method TEXT DEFAULT 'cash',"
        "  FOREIGN KEY (user_id) REFERENCES users(id)"
        ")"
    );
    if (!ok) return false;

    // Sale items table
    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS sale_items ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  sale_id INTEGER NOT NULL,"
        "  product_id INTEGER NOT NULL,"
        "  product_name TEXT,"
        "  quantity INTEGER NOT NULL DEFAULT 1,"
        "  unit_price REAL NOT NULL DEFAULT 0,"
        "  cost REAL NOT NULL DEFAULT 0,"
        "  discount REAL NOT NULL DEFAULT 0,"
        "  total REAL NOT NULL DEFAULT 0,"
        "  FOREIGN KEY (sale_id) REFERENCES sales(id),"
        "  FOREIGN KEY (product_id) REFERENCES products(id)"
        ")"
    );
    if (!ok) return false;

    return true;
}

bool DatabaseManager::createDefaultAdmin()
{
    User admin;
    admin.setUsername("admin");
    admin.setPassword(User::hashPassword("admin"));
    admin.setFullName(QString::fromUtf8("مدير النظام"));
    admin.setRole(UserRole::Admin);
    return addUser(admin);
}

// ===== Users =====

bool DatabaseManager::addUser(const User &user)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (username, password, full_name, role, is_active) "
                  "VALUES (:username, :password, :fullName, :role, :isActive)");
    query.bindValue(":username", user.username());
    query.bindValue(":password", user.password());
    query.bindValue(":fullName", user.fullName());
    query.bindValue(":role", User::roleToString(user.role()));
    query.bindValue(":isActive", user.isActive() ? 1 : 0);

    if (!query.exec()) {
        qWarning() << "فشل إضافة المستخدم:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateUser(const User &user)
{
    QSqlQuery query(m_db);
    if (user.password().isEmpty()) {
        query.prepare("UPDATE users SET username=:username, full_name=:fullName, "
                      "role=:role, is_active=:isActive WHERE id=:id");
    } else {
        query.prepare("UPDATE users SET username=:username, password=:password, "
                      "full_name=:fullName, role=:role, is_active=:isActive WHERE id=:id");
        query.bindValue(":password", user.password());
    }
    query.bindValue(":username", user.username());
    query.bindValue(":fullName", user.fullName());
    query.bindValue(":role", User::roleToString(user.role()));
    query.bindValue(":isActive", user.isActive() ? 1 : 0);
    query.bindValue(":id", user.id());

    if (!query.exec()) {
        qWarning() << "فشل تحديث المستخدم:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteUser(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE id=:id");
    query.bindValue(":id", id);
    return query.exec();
}

User DatabaseManager::getUserById(int id)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM users WHERE id=:id");
    query.bindValue(":id", id);

    User user;
    if (query.exec() && query.next()) {
        user.setId(query.value("id").toInt());
        user.setUsername(query.value("username").toString());
        user.setPassword(query.value("password").toString());
        user.setFullName(query.value("full_name").toString());
        user.setRole(User::roleFromString(query.value("role").toString()));
        user.setActive(query.value("is_active").toBool());
    }
    return user;
}

User DatabaseManager::getUserByUsername(const QString &username)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM users WHERE username=:username");
    query.bindValue(":username", username);

    User user;
    if (query.exec() && query.next()) {
        user.setId(query.value("id").toInt());
        user.setUsername(query.value("username").toString());
        user.setPassword(query.value("password").toString());
        user.setFullName(query.value("full_name").toString());
        user.setRole(User::roleFromString(query.value("role").toString()));
        user.setActive(query.value("is_active").toBool());
    }
    return user;
}

QList<User> DatabaseManager::getAllUsers()
{
    QList<User> users;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM users ORDER BY id");

    while (query.next()) {
        User user;
        user.setId(query.value("id").toInt());
        user.setUsername(query.value("username").toString());
        user.setFullName(query.value("full_name").toString());
        user.setRole(User::roleFromString(query.value("role").toString()));
        user.setActive(query.value("is_active").toBool());
        users.append(user);
    }
    return users;
}

bool DatabaseManager::authenticateUser(const QString &username, const QString &password)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT password FROM users WHERE username=:username AND is_active=1");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        QString storedHash = query.value("password").toString();
        return storedHash == User::hashPassword(password);
    }
    return false;
}

int DatabaseManager::getUserCount()
{
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next())
        return query.value(0).toInt();
    return 0;
}

// ===== Categories =====

bool DatabaseManager::addCategory(const Category &category)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO categories (name, description) VALUES (:name, :desc)");
    query.bindValue(":name", category.name());
    query.bindValue(":desc", category.description());

    if (!query.exec()) {
        qWarning() << "فشل إضافة التصنيف:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateCategory(const Category &category)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE categories SET name=:name, description=:desc WHERE id=:id");
    query.bindValue(":name", category.name());
    query.bindValue(":desc", category.description());
    query.bindValue(":id", category.id());
    return query.exec();
}

bool DatabaseManager::deleteCategory(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM categories WHERE id=:id");
    query.bindValue(":id", id);
    return query.exec();
}

Category DatabaseManager::getCategoryById(int id)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM categories WHERE id=:id");
    query.bindValue(":id", id);

    Category cat;
    if (query.exec() && query.next()) {
        cat.setId(query.value("id").toInt());
        cat.setName(query.value("name").toString());
        cat.setDescription(query.value("description").toString());
    }
    return cat;
}

QList<Category> DatabaseManager::getAllCategories()
{
    QList<Category> categories;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM categories ORDER BY name");

    while (query.next()) {
        Category cat;
        cat.setId(query.value("id").toInt());
        cat.setName(query.value("name").toString());
        cat.setDescription(query.value("description").toString());
        categories.append(cat);
    }
    return categories;
}

// ===== Products =====

bool DatabaseManager::addProduct(const Product &product)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO products (name, barcode, price, cost, stock, "
                  "category_id, description, is_active) "
                  "VALUES (:name, :barcode, :price, :cost, :stock, "
                  ":categoryId, :desc, :isActive)");
    query.bindValue(":name", product.name());
    query.bindValue(":barcode", product.barcode());
    query.bindValue(":price", product.price());
    query.bindValue(":cost", product.cost());
    query.bindValue(":stock", product.stock());
    query.bindValue(":categoryId", product.categoryId());
    query.bindValue(":desc", product.description());
    query.bindValue(":isActive", product.isActive() ? 1 : 0);

    if (!query.exec()) {
        qWarning() << "فشل إضافة المنتج:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateProduct(const Product &product)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE products SET name=:name, barcode=:barcode, price=:price, "
                  "cost=:cost, stock=:stock, category_id=:categoryId, "
                  "description=:desc, is_active=:isActive WHERE id=:id");
    query.bindValue(":name", product.name());
    query.bindValue(":barcode", product.barcode());
    query.bindValue(":price", product.price());
    query.bindValue(":cost", product.cost());
    query.bindValue(":stock", product.stock());
    query.bindValue(":categoryId", product.categoryId());
    query.bindValue(":desc", product.description());
    query.bindValue(":isActive", product.isActive() ? 1 : 0);
    query.bindValue(":id", product.id());
    return query.exec();
}

bool DatabaseManager::deleteProduct(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM products WHERE id=:id");
    query.bindValue(":id", id);
    return query.exec();
}

Product DatabaseManager::getProductById(int id)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM products WHERE id=:id");
    query.bindValue(":id", id);

    Product product;
    if (query.exec() && query.next()) {
        product.setId(query.value("id").toInt());
        product.setName(query.value("name").toString());
        product.setBarcode(query.value("barcode").toString());
        product.setPrice(query.value("price").toDouble());
        product.setCost(query.value("cost").toDouble());
        product.setStock(query.value("stock").toInt());
        product.setCategoryId(query.value("category_id").toInt());
        product.setDescription(query.value("description").toString());
        product.setActive(query.value("is_active").toBool());
    }
    return product;
}

Product DatabaseManager::getProductByBarcode(const QString &barcode)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM products WHERE barcode=:barcode AND is_active=1");
    query.bindValue(":barcode", barcode);

    Product product;
    if (query.exec() && query.next()) {
        product.setId(query.value("id").toInt());
        product.setName(query.value("name").toString());
        product.setBarcode(query.value("barcode").toString());
        product.setPrice(query.value("price").toDouble());
        product.setCost(query.value("cost").toDouble());
        product.setStock(query.value("stock").toInt());
        product.setCategoryId(query.value("category_id").toInt());
        product.setDescription(query.value("description").toString());
        product.setActive(query.value("is_active").toBool());
    }
    return product;
}

QList<Product> DatabaseManager::getAllProducts()
{
    QList<Product> products;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM products WHERE is_active=1 ORDER BY name");

    while (query.next()) {
        Product product;
        product.setId(query.value("id").toInt());
        product.setName(query.value("name").toString());
        product.setBarcode(query.value("barcode").toString());
        product.setPrice(query.value("price").toDouble());
        product.setCost(query.value("cost").toDouble());
        product.setStock(query.value("stock").toInt());
        product.setCategoryId(query.value("category_id").toInt());
        product.setDescription(query.value("description").toString());
        product.setActive(query.value("is_active").toBool());
        products.append(product);
    }
    return products;
}

QList<Product> DatabaseManager::getProductsByCategory(int categoryId)
{
    QList<Product> products;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM products WHERE category_id=:catId AND is_active=1 ORDER BY name");
    query.bindValue(":catId", categoryId);

    if (query.exec()) {
        while (query.next()) {
            Product product;
            product.setId(query.value("id").toInt());
            product.setName(query.value("name").toString());
            product.setBarcode(query.value("barcode").toString());
            product.setPrice(query.value("price").toDouble());
            product.setCost(query.value("cost").toDouble());
            product.setStock(query.value("stock").toInt());
            product.setCategoryId(query.value("category_id").toInt());
            products.append(product);
        }
    }
    return products;
}

QList<Product> DatabaseManager::searchProducts(const QString &keyword)
{
    QList<Product> products;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM products WHERE is_active=1 AND "
                  "(name LIKE :keyword OR barcode LIKE :keyword2) ORDER BY name");
    query.bindValue(":keyword", "%" + keyword + "%");
    query.bindValue(":keyword2", "%" + keyword + "%");

    if (query.exec()) {
        while (query.next()) {
            Product product;
            product.setId(query.value("id").toInt());
            product.setName(query.value("name").toString());
            product.setBarcode(query.value("barcode").toString());
            product.setPrice(query.value("price").toDouble());
            product.setCost(query.value("cost").toDouble());
            product.setStock(query.value("stock").toInt());
            product.setCategoryId(query.value("category_id").toInt());
            products.append(product);
        }
    }
    return products;
}

bool DatabaseManager::updateProductStock(int productId, int quantity)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE products SET stock = stock - :qty WHERE id = :id");
    query.bindValue(":qty", quantity);
    query.bindValue(":id", productId);
    return query.exec();
}

// ===== Sales =====

bool DatabaseManager::addSale(Sale &sale)
{
    m_db.transaction();

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO sales (user_id, cashier_name, date_time, subtotal, "
                  "tax, discount, total, amount_paid, payment_method) "
                  "VALUES (:userId, :cashierName, :dateTime, :subtotal, "
                  ":tax, :discount, :total, :amountPaid, :paymentMethod)");
    query.bindValue(":userId", sale.userId());
    query.bindValue(":cashierName", sale.cashierName());
    query.bindValue(":dateTime", sale.dateTime());
    query.bindValue(":subtotal", sale.subtotal());
    query.bindValue(":tax", sale.tax());
    query.bindValue(":discount", sale.discount());
    query.bindValue(":total", sale.total());
    query.bindValue(":amountPaid", sale.amountPaid());
    query.bindValue(":paymentMethod", sale.paymentMethod());

    if (!query.exec()) {
        m_db.rollback();
        qWarning() << "فشل إضافة الفاتورة:" << query.lastError().text();
        return false;
    }

    int saleId = query.lastInsertId().toInt();
    sale.setId(saleId);

    // Insert sale items
    const auto items = sale.items();
    for (const SaleItem &item : items) {
        QSqlQuery itemQuery(m_db);
        itemQuery.prepare("INSERT INTO sale_items (sale_id, product_id, product_name, "
                          "quantity, unit_price, cost, discount, total) "
                          "VALUES (:saleId, :productId, :productName, "
                          ":quantity, :unitPrice, :cost, :discount, :total)");
        itemQuery.bindValue(":saleId", saleId);
        itemQuery.bindValue(":productId", item.productId);
        itemQuery.bindValue(":productName", item.productName);
        itemQuery.bindValue(":quantity", item.quantity);
        itemQuery.bindValue(":unitPrice", item.unitPrice);
        itemQuery.bindValue(":cost", item.cost);
        itemQuery.bindValue(":discount", item.discount);
        itemQuery.bindValue(":total", item.total);

        if (!itemQuery.exec()) {
            m_db.rollback();
            return false;
        }

        // Update stock
        updateProductStock(item.productId, item.quantity);
    }

    m_db.commit();
    return true;
}

Sale DatabaseManager::getSaleById(int id)
{
    Sale sale;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM sales WHERE id=:id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        sale.setId(query.value("id").toInt());
        sale.setUserId(query.value("user_id").toInt());
        sale.setCashierName(query.value("cashier_name").toString());
        sale.setDateTime(query.value("date_time").toDateTime());
        sale.setSubtotal(query.value("subtotal").toDouble());
        sale.setTax(query.value("tax").toDouble());
        sale.setDiscount(query.value("discount").toDouble());
        sale.setTotal(query.value("total").toDouble());
        sale.setAmountPaid(query.value("amount_paid").toDouble());
        sale.setPaymentMethod(query.value("payment_method").toString());

        // Load items
        QSqlQuery itemQuery(m_db);
        itemQuery.prepare("SELECT * FROM sale_items WHERE sale_id=:saleId");
        itemQuery.bindValue(":saleId", id);

        if (itemQuery.exec()) {
            while (itemQuery.next()) {
                SaleItem item;
                item.id = itemQuery.value("id").toInt();
                item.saleId = id;
                item.productId = itemQuery.value("product_id").toInt();
                item.productName = itemQuery.value("product_name").toString();
                item.quantity = itemQuery.value("quantity").toInt();
                item.unitPrice = itemQuery.value("unit_price").toDouble();
                item.cost = itemQuery.value("cost").toDouble();
                item.discount = itemQuery.value("discount").toDouble();
                item.total = itemQuery.value("total").toDouble();
                sale.addItem(item);
            }
        }
    }
    return sale;
}

QList<Sale> DatabaseManager::getSalesByDateRange(const QDateTime &from, const QDateTime &to)
{
    QList<Sale> sales;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM sales WHERE date_time BETWEEN :from AND :to ORDER BY date_time DESC");
    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (query.exec()) {
        while (query.next()) {
            Sale sale;
            sale.setId(query.value("id").toInt());
            sale.setUserId(query.value("user_id").toInt());
            sale.setCashierName(query.value("cashier_name").toString());
            sale.setDateTime(query.value("date_time").toDateTime());
            sale.setSubtotal(query.value("subtotal").toDouble());
            sale.setTax(query.value("tax").toDouble());
            sale.setDiscount(query.value("discount").toDouble());
            sale.setTotal(query.value("total").toDouble());
            sale.setAmountPaid(query.value("amount_paid").toDouble());
            sale.setPaymentMethod(query.value("payment_method").toString());
            sales.append(sale);
        }
    }
    return sales;
}

QList<Sale> DatabaseManager::getAllSales()
{
    QList<Sale> sales;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM sales ORDER BY date_time DESC");

    while (query.next()) {
        Sale sale;
        sale.setId(query.value("id").toInt());
        sale.setUserId(query.value("user_id").toInt());
        sale.setCashierName(query.value("cashier_name").toString());
        sale.setDateTime(query.value("date_time").toDateTime());
        sale.setSubtotal(query.value("subtotal").toDouble());
        sale.setTax(query.value("tax").toDouble());
        sale.setDiscount(query.value("discount").toDouble());
        sale.setTotal(query.value("total").toDouble());
        sale.setAmountPaid(query.value("amount_paid").toDouble());
        sale.setPaymentMethod(query.value("payment_method").toString());
        sales.append(sale);
    }
    return sales;
}

double DatabaseManager::getTotalSalesAmount(const QDateTime &from, const QDateTime &to)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COALESCE(SUM(total), 0) FROM sales WHERE date_time BETWEEN :from AND :to");
    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (query.exec() && query.next())
        return query.value(0).toDouble();
    return 0.0;
}

int DatabaseManager::getTotalSalesCount(const QDateTime &from, const QDateTime &to)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM sales WHERE date_time BETWEEN :from AND :to");
    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}

double DatabaseManager::getTotalProfit(const QDateTime &from, const QDateTime &to)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COALESCE(SUM((si.unit_price - si.cost) * si.quantity), 0) "
                  "FROM sale_items si "
                  "JOIN sales s ON si.sale_id = s.id "
                  "WHERE s.date_time BETWEEN :from AND :to");
    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (query.exec() && query.next())
        return query.value(0).toDouble();
    return 0.0;
}
