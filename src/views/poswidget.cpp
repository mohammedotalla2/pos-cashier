#include "poswidget.h"
#include "database/databasemanager.h"
#include "utils/config.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QRadioButton>
#include <QButtonGroup>

PosWidget::PosWidget(QWidget *parent)
    : QWidget(parent),
      m_thermalPrinter(new ThermalPrinter(this)),
      m_gdiPrinter(new GdiPrinter(this))
{
    setupUi();
    loadCategories();
    loadProducts();
}

void PosWidget::setCurrentUser(const User &user)
{
    m_currentUser = user;
}

void PosWidget::setupUi()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(10);

    // ===== Left side: Products =====
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // Search and barcode
    QHBoxLayout *searchLayout = new QHBoxLayout();

    m_barcodeEdit = new QLineEdit();
    m_barcodeEdit->setPlaceholderText(QString::fromUtf8("مسح الباركود..."));
    m_barcodeEdit->setStyleSheet(
        "padding: 10px; font-size: 16px; border: 2px solid #4CAF50; border-radius: 8px;");
    connect(m_barcodeEdit, &QLineEdit::returnPressed, this, &PosWidget::onBarcodeEntered);
    searchLayout->addWidget(m_barcodeEdit, 1);

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText(QString::fromUtf8("بحث عن منتج..."));
    m_searchEdit->setStyleSheet(
        "padding: 10px; font-size: 14px; border: 2px solid #2196F3; border-radius: 8px;");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &PosWidget::onSearchTextChanged);
    searchLayout->addWidget(m_searchEdit, 1);

    m_categoryFilter = new QComboBox();
    m_categoryFilter->setStyleSheet(
        "padding: 8px; font-size: 14px; border: 2px solid #ddd; border-radius: 8px;");
    connect(m_categoryFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PosWidget::onCategoryFilterChanged);
    searchLayout->addWidget(m_categoryFilter);

    leftLayout->addLayout(searchLayout);

    // Products table
    m_productsTable = new QTableWidget();
    m_productsTable->setColumnCount(4);
    m_productsTable->setHorizontalHeaderLabels({
        QString::fromUtf8("الاسم"),
        QString::fromUtf8("الباركود"),
        QString::fromUtf8("السعر"),
        QString::fromUtf8("المخزون")
    });
    m_productsTable->horizontalHeader()->setStretchLastSection(true);
    m_productsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_productsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_productsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_productsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_productsTable->setAlternatingRowColors(true);
    m_productsTable->setStyleSheet(
        "QTableWidget { font-size: 14px; border: 1px solid #ddd; border-radius: 5px; }"
        "QTableWidget::item { padding: 8px; }"
        "QHeaderView::section { background-color: #2196F3; color: white; padding: 8px; "
        "font-size: 14px; font-weight: bold; border: none; }");
    connect(m_productsTable, &QTableWidget::cellDoubleClicked,
            this, &PosWidget::onProductClicked);
    leftLayout->addWidget(m_productsTable);

    mainLayout->addLayout(leftLayout, 3);

    // ===== Right side: Cart =====
    QVBoxLayout *rightLayout = new QVBoxLayout();

    QLabel *cartTitle = new QLabel(QString::fromUtf8("سلة المشتريات"));
    cartTitle->setStyleSheet(
        "font-size: 20px; font-weight: bold; color: #1976D2; padding: 10px;");
    cartTitle->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(cartTitle);

    // Cart table
    m_cartTable = new QTableWidget();
    m_cartTable->setColumnCount(5);
    m_cartTable->setHorizontalHeaderLabels({
        QString::fromUtf8("المنتج"),
        QString::fromUtf8("الكمية"),
        QString::fromUtf8("السعر"),
        QString::fromUtf8("الخصم %"),
        QString::fromUtf8("الإجمالي")
    });
    m_cartTable->horizontalHeader()->setStretchLastSection(true);
    m_cartTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_cartTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_cartTable->setAlternatingRowColors(true);
    m_cartTable->setStyleSheet(
        "QTableWidget { font-size: 13px; border: 1px solid #ddd; border-radius: 5px; }"
        "QTableWidget::item { padding: 6px; }"
        "QHeaderView::section { background-color: #FF9800; color: white; padding: 6px; "
        "font-size: 13px; font-weight: bold; border: none; }");
    connect(m_cartTable, &QTableWidget::cellChanged, this, &PosWidget::onQuantityChanged);
    rightLayout->addWidget(m_cartTable);

    // Cart buttons
    QHBoxLayout *cartBtnLayout = new QHBoxLayout();

    m_removeBtn = new QPushButton(QString::fromUtf8("حذف صنف"));
    m_removeBtn->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #d32f2f; }");
    connect(m_removeBtn, &QPushButton::clicked, this, &PosWidget::onRemoveItem);
    cartBtnLayout->addWidget(m_removeBtn);

    m_clearBtn = new QPushButton(QString::fromUtf8("مسح الكل"));
    m_clearBtn->setStyleSheet(
        "QPushButton { background-color: #9E9E9E; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #757575; }");
    connect(m_clearBtn, &QPushButton::clicked, this, &PosWidget::onClearCart);
    cartBtnLayout->addWidget(m_clearBtn);

    rightLayout->addLayout(cartBtnLayout);

    // Discount
    QHBoxLayout *discountLayout = new QHBoxLayout();
    discountLayout->addWidget(new QLabel(QString::fromUtf8("خصم على الفاتورة %:")));
    m_discountSpin = new QDoubleSpinBox();
    m_discountSpin->setRange(0, 100);
    m_discountSpin->setDecimals(1);
    m_discountSpin->setStyleSheet("padding: 5px; font-size: 14px;");
    connect(m_discountSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double val) { m_orderDiscount = val; updateTotals(); });
    discountLayout->addWidget(m_discountSpin);
    rightLayout->addLayout(discountLayout);

    // Totals
    QGroupBox *totalsBox = new QGroupBox(QString::fromUtf8("الحساب"));
    totalsBox->setStyleSheet(
        "QGroupBox { font-size: 14px; font-weight: bold; border: 2px solid #e0e0e0; "
        "border-radius: 8px; padding: 15px; margin-top: 5px; }"
        "QGroupBox::title { subcontrol-origin: margin; padding: 0 5px; }");

    QVBoxLayout *totalsLayout = new QVBoxLayout(totalsBox);

    m_subtotalLabel = new QLabel(QString::fromUtf8("المجموع الفرعي: 0.00"));
    m_subtotalLabel->setStyleSheet("font-size: 16px; padding: 3px;");
    totalsLayout->addWidget(m_subtotalLabel);

    m_discountLabel = new QLabel(QString::fromUtf8("الخصم: 0.00"));
    m_discountLabel->setStyleSheet("font-size: 16px; padding: 3px; color: #f44336;");
    totalsLayout->addWidget(m_discountLabel);

    m_taxLabel = new QLabel(QString::fromUtf8("الضريبة: 0.00"));
    m_taxLabel->setStyleSheet("font-size: 16px; padding: 3px;");
    totalsLayout->addWidget(m_taxLabel);

    m_totalLabel = new QLabel(QString::fromUtf8("الإجمالي: 0.00"));
    m_totalLabel->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #1976D2; padding: 5px;");
    totalsLayout->addWidget(m_totalLabel);

    rightLayout->addWidget(totalsBox);

    // Pay button
    m_payBtn = new QPushButton(QString::fromUtf8("دفع"));
    m_payBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 18px; "
        "font-size: 22px; font-weight: bold; border: none; border-radius: 10px; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:pressed { background-color: #3d8b40; }");
    connect(m_payBtn, &QPushButton::clicked, this, &PosWidget::onPayClicked);
    rightLayout->addWidget(m_payBtn);

    mainLayout->addLayout(rightLayout, 2);
}

void PosWidget::loadCategories()
{
    m_categoryFilter->clear();
    m_categoryFilter->addItem(QString::fromUtf8("جميع التصنيفات"), -1);

    auto categories = DatabaseManager::instance().getAllCategories();
    for (const Category &cat : categories) {
        m_categoryFilter->addItem(cat.name(), cat.id());
    }
}

void PosWidget::loadProducts(int categoryId)
{
    QList<Product> products;
    if (categoryId < 0) {
        products = DatabaseManager::instance().getAllProducts();
    } else {
        products = DatabaseManager::instance().getProductsByCategory(categoryId);
    }

    m_productsTable->setRowCount(products.size());
    QString currency = Config::instance().currencySymbol();

    for (int i = 0; i < products.size(); i++) {
        const Product &p = products[i];

        QTableWidgetItem *nameItem = new QTableWidgetItem(p.name());
        nameItem->setData(Qt::UserRole, p.id());
        m_productsTable->setItem(i, 0, nameItem);
        m_productsTable->setItem(i, 1, new QTableWidgetItem(p.barcode()));
        m_productsTable->setItem(i, 2, new QTableWidgetItem(
            QString("%1 %2").arg(p.price(), 0, 'f', 2).arg(currency)));
        m_productsTable->setItem(i, 3, new QTableWidgetItem(QString::number(p.stock())));

        // Color code low stock
        if (p.stock() <= 5) {
            for (int col = 0; col < 4; col++) {
                m_productsTable->item(i, col)->setBackground(QColor(255, 235, 238));
            }
        }
    }
}

void PosWidget::refreshProducts()
{
    loadCategories();
    int catId = m_categoryFilter->currentData().toInt();
    loadProducts(catId);
}

void PosWidget::onBarcodeEntered()
{
    QString barcode = m_barcodeEdit->text().trimmed();
    if (barcode.isEmpty()) return;

    Product product = DatabaseManager::instance().getProductByBarcode(barcode);
    if (product.isValid()) {
        addToCart(product);
    } else {
        QMessageBox::warning(this, QString::fromUtf8("خطأ"),
                             QString::fromUtf8("لم يتم العثور على المنتج بالباركود: %1").arg(barcode));
    }
    m_barcodeEdit->clear();
    m_barcodeEdit->setFocus();
}

void PosWidget::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        int catId = m_categoryFilter->currentData().toInt();
        loadProducts(catId);
        return;
    }

    auto products = DatabaseManager::instance().searchProducts(text);
    m_productsTable->setRowCount(products.size());
    QString currency = Config::instance().currencySymbol();

    for (int i = 0; i < products.size(); i++) {
        const Product &p = products[i];
        QTableWidgetItem *nameItem = new QTableWidgetItem(p.name());
        nameItem->setData(Qt::UserRole, p.id());
        m_productsTable->setItem(i, 0, nameItem);
        m_productsTable->setItem(i, 1, new QTableWidgetItem(p.barcode()));
        m_productsTable->setItem(i, 2, new QTableWidgetItem(
            QString("%1 %2").arg(p.price(), 0, 'f', 2).arg(currency)));
        m_productsTable->setItem(i, 3, new QTableWidgetItem(QString::number(p.stock())));
    }
}

void PosWidget::onProductClicked(int row, int /*col*/)
{
    int productId = m_productsTable->item(row, 0)->data(Qt::UserRole).toInt();
    Product product = DatabaseManager::instance().getProductById(productId);
    if (product.isValid()) {
        addToCart(product);
    }
}

void PosWidget::addToCart(const Product &product)
{
    // Check if product already in cart
    for (int i = 0; i < m_cartItems.size(); i++) {
        if (m_cartItems[i].product().id() == product.id()) {
            m_cartItems[i].setQuantity(m_cartItems[i].quantity() + 1);
            updateCartDisplay();
            return;
        }
    }

    CartItem item(product, 1, 0.0);
    m_cartItems.append(item);
    updateCartDisplay();
}

void PosWidget::updateCartDisplay()
{
    m_cartTable->blockSignals(true);
    m_cartTable->setRowCount(m_cartItems.size());
    QString currency = Config::instance().currencySymbol();

    for (int i = 0; i < m_cartItems.size(); i++) {
        const CartItem &item = m_cartItems[i];

        QTableWidgetItem *nameItem = new QTableWidgetItem(item.product().name());
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        m_cartTable->setItem(i, 0, nameItem);

        QTableWidgetItem *qtyItem = new QTableWidgetItem(QString::number(item.quantity()));
        m_cartTable->setItem(i, 1, qtyItem);

        QTableWidgetItem *priceItem = new QTableWidgetItem(
            QString("%1").arg(item.product().price(), 0, 'f', 2));
        priceItem->setFlags(priceItem->flags() & ~Qt::ItemIsEditable);
        m_cartTable->setItem(i, 2, priceItem);

        QTableWidgetItem *discItem = new QTableWidgetItem(
            QString::number(item.discount(), 'f', 1));
        m_cartTable->setItem(i, 3, discItem);

        QTableWidgetItem *totalItem = new QTableWidgetItem(
            QString("%1 %2").arg(item.totalAfterDiscount(), 0, 'f', 2).arg(currency));
        totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable);
        m_cartTable->setItem(i, 4, totalItem);
    }

    m_cartTable->blockSignals(false);
    updateTotals();
}

void PosWidget::onQuantityChanged(int row, int col)
{
    if (row >= m_cartItems.size()) return;

    if (col == 1) { // Quantity
        bool ok;
        int qty = m_cartTable->item(row, col)->text().toInt(&ok);
        if (ok && qty > 0) {
            m_cartItems[row].setQuantity(qty);
        }
    } else if (col == 3) { // Discount
        bool ok;
        double disc = m_cartTable->item(row, col)->text().toDouble(&ok);
        if (ok && disc >= 0 && disc <= 100) {
            m_cartItems[row].setDiscount(disc);
        }
    }
    updateCartDisplay();
}

void PosWidget::updateTotals()
{
    double subtotal = 0.0;
    for (const CartItem &item : m_cartItems) {
        subtotal += item.totalAfterDiscount();
    }

    double discountAmount = subtotal * m_orderDiscount / 100.0;
    double afterDiscount = subtotal - discountAmount;
    double taxRate = Config::instance().taxRate();
    double taxAmount = afterDiscount * taxRate / 100.0;
    double total = afterDiscount + taxAmount;

    QString currency = Config::instance().currencySymbol();

    m_subtotalLabel->setText(QString::fromUtf8("المجموع الفرعي: %1 %2")
        .arg(subtotal, 0, 'f', 2).arg(currency));
    m_discountLabel->setText(QString::fromUtf8("الخصم: %1 %2")
        .arg(discountAmount, 0, 'f', 2).arg(currency));
    m_taxLabel->setText(QString::fromUtf8("الضريبة (%1%): %2 %3")
        .arg(taxRate, 0, 'f', 1).arg(taxAmount, 0, 'f', 2).arg(currency));
    m_totalLabel->setText(QString::fromUtf8("الإجمالي: %1 %2")
        .arg(total, 0, 'f', 2).arg(currency));
}

void PosWidget::onRemoveItem()
{
    int row = m_cartTable->currentRow();
    if (row >= 0 && row < m_cartItems.size()) {
        m_cartItems.removeAt(row);
        updateCartDisplay();
    }
}

void PosWidget::onClearCart()
{
    if (m_cartItems.isEmpty()) return;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, QString::fromUtf8("تأكيد"),
        QString::fromUtf8("هل تريد مسح جميع الأصناف من السلة؟"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_cartItems.clear();
        m_discountSpin->setValue(0);
        updateCartDisplay();
    }
}

void PosWidget::onCategoryFilterChanged(int index)
{
    if (index < 0) return;
    int catId = m_categoryFilter->itemData(index).toInt();
    loadProducts(catId);
}

void PosWidget::onPayClicked()
{
    if (m_cartItems.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("تنبيه"),
                             QString::fromUtf8("السلة فارغة!"));
        return;
    }

    // Calculate totals
    double subtotal = 0.0;
    for (const CartItem &item : m_cartItems) {
        subtotal += item.totalAfterDiscount();
    }

    double discountAmount = subtotal * m_orderDiscount / 100.0;
    double afterDiscount = subtotal - discountAmount;
    double taxRate = Config::instance().taxRate();
    double taxAmount = afterDiscount * taxRate / 100.0;
    double total = afterDiscount + taxAmount;

    // Payment dialog
    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("الدفع"));
    dialog.setLayoutDirection(Qt::RightToLeft);
    dialog.setFixedSize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QString currency = Config::instance().currencySymbol();
    QLabel *totalDisplay = new QLabel(
        QString::fromUtf8("المبلغ المطلوب: %1 %2").arg(total, 0, 'f', 2).arg(currency));
    totalDisplay->setStyleSheet(
        "font-size: 22px; font-weight: bold; color: #1976D2; padding: 10px;");
    totalDisplay->setAlignment(Qt::AlignCenter);
    layout->addWidget(totalDisplay);

    // Payment method
    QHBoxLayout *methodLayout = new QHBoxLayout();
    QRadioButton *cashRadio = new QRadioButton(QString::fromUtf8("نقدي"));
    cashRadio->setChecked(true);
    cashRadio->setStyleSheet("font-size: 16px;");
    QRadioButton *cardRadio = new QRadioButton(QString::fromUtf8("بطاقة"));
    cardRadio->setStyleSheet("font-size: 16px;");
    methodLayout->addWidget(cashRadio);
    methodLayout->addWidget(cardRadio);
    layout->addLayout(methodLayout);

    // Amount paid
    QFormLayout *formLayout = new QFormLayout();
    QDoubleSpinBox *amountSpin = new QDoubleSpinBox();
    amountSpin->setRange(0, 999999);
    amountSpin->setDecimals(2);
    amountSpin->setValue(total);
    amountSpin->setStyleSheet("padding: 8px; font-size: 18px;");
    formLayout->addRow(QString::fromUtf8("المبلغ المدفوع:"), amountSpin);

    QLabel *changeLabel = new QLabel(QString::fromUtf8("الباقي: 0.00 %1").arg(currency));
    changeLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #4CAF50;");
    formLayout->addRow("", changeLabel);

    connect(amountSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [&](double val) {
        double change = val - total;
        if (change < 0) change = 0;
        changeLabel->setText(QString::fromUtf8("الباقي: %1 %2")
            .arg(change, 0, 'f', 2).arg(currency));
    });

    layout->addLayout(formLayout);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(QString::fromUtf8("تأكيد الدفع"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QString::fromUtf8("إلغاء"));
    buttons->button(QDialogButtonBox::Ok)->setStyleSheet(
        "padding: 10px 30px; font-size: 16px; background-color: #4CAF50; "
        "color: white; border-radius: 6px;");
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        double amountPaid = amountSpin->value();
        if (amountPaid < total) {
            QMessageBox::warning(this, QString::fromUtf8("خطأ"),
                                 QString::fromUtf8("المبلغ المدفوع أقل من المطلوب!"));
            return;
        }

        QString paymentMethod = cashRadio->isChecked() ? "cash" : "card";
        processPayment(amountPaid, paymentMethod);
    }
}

void PosWidget::processPayment(double amountPaid, const QString &paymentMethod)
{
    double subtotal = 0.0;
    for (const CartItem &item : m_cartItems) {
        subtotal += item.totalAfterDiscount();
    }

    double discountAmount = subtotal * m_orderDiscount / 100.0;
    double afterDiscount = subtotal - discountAmount;
    double taxRate = Config::instance().taxRate();
    double taxAmount = afterDiscount * taxRate / 100.0;
    double total = afterDiscount + taxAmount;

    Sale sale;
    sale.setUserId(m_currentUser.id());
    sale.setCashierName(m_currentUser.fullName());
    sale.setDateTime(QDateTime::currentDateTime());
    sale.setSubtotal(subtotal);
    sale.setTax(taxAmount);
    sale.setDiscount(discountAmount);
    sale.setTotal(total);
    sale.setAmountPaid(amountPaid);
    sale.setPaymentMethod(paymentMethod);

    // Add items
    for (const CartItem &cartItem : m_cartItems) {
        SaleItem saleItem;
        saleItem.productId = cartItem.product().id();
        saleItem.productName = cartItem.product().name();
        saleItem.quantity = cartItem.quantity();
        saleItem.unitPrice = cartItem.product().price();
        saleItem.cost = cartItem.product().cost();
        saleItem.discount = cartItem.discount();
        saleItem.total = cartItem.totalAfterDiscount();
        sale.addItem(saleItem);
    }

    if (DatabaseManager::instance().addSale(sale)) {
        printReceipt(sale.id());

        // Clear cart
        m_cartItems.clear();
        m_discountSpin->setValue(0);
        updateCartDisplay();
        refreshProducts();

        QString currency = Config::instance().currencySymbol();
        double change = amountPaid - total;

        QMessageBox::information(this, QString::fromUtf8("تمت العملية بنجاح"),
            QString::fromUtf8("تم حفظ الفاتورة رقم: %1\nالباقي: %2 %3")
                .arg(sale.id()).arg(change, 0, 'f', 2).arg(currency));

        emit saleCompleted();
    } else {
        QMessageBox::critical(this, QString::fromUtf8("خطأ"),
                              QString::fromUtf8("فشل حفظ الفاتورة!"));
    }
}

void PosWidget::printReceipt(int saleId)
{
    Sale sale = DatabaseManager::instance().getSaleById(saleId);
    if (!sale.isValid()) return;

    if (Config::instance().useThermalPrinter()) {
        // Try thermal printer
        QString port = Config::instance().thermalPrinterPort();
        int baud = Config::instance().thermalPrinterBaudRate();

        if (m_thermalPrinter->open(port, baud)) {
            m_thermalPrinter->printReceipt(sale);
            m_thermalPrinter->close();
        } else {
            // Fallback to GDI
            m_gdiPrinter->printReceipt(sale);
        }
    } else {
        // Use GDI printer
        m_gdiPrinter->printReceipt(sale);
    }
}
