#include "productswidget.h"
#include "database/databasemanager.h"
#include "utils/config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>

ProductsWidget::ProductsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    refreshTable();
}

void ProductsWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Title
    QLabel *title = new QLabel(QString::fromUtf8("إدارة المنتجات"));
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1976D2; padding: 10px;");
    mainLayout->addWidget(title);

    // Toolbar
    QHBoxLayout *toolbar = new QHBoxLayout();

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText(QString::fromUtf8("بحث عن منتج..."));
    m_searchEdit->setStyleSheet(
        "padding: 8px; font-size: 14px; border: 2px solid #ddd; border-radius: 6px;");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ProductsWidget::onSearchChanged);
    toolbar->addWidget(m_searchEdit, 1);

    m_addBtn = new QPushButton(QString::fromUtf8("إضافة منتج"));
    m_addBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #45a049; }");
    connect(m_addBtn, &QPushButton::clicked, this, &ProductsWidget::onAddClicked);
    toolbar->addWidget(m_addBtn);

    m_editBtn = new QPushButton(QString::fromUtf8("تعديل"));
    m_editBtn->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #1976D2; }");
    connect(m_editBtn, &QPushButton::clicked, this, &ProductsWidget::onEditClicked);
    toolbar->addWidget(m_editBtn);

    m_deleteBtn = new QPushButton(QString::fromUtf8("حذف"));
    m_deleteBtn->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #d32f2f; }");
    connect(m_deleteBtn, &QPushButton::clicked, this, &ProductsWidget::onDeleteClicked);
    toolbar->addWidget(m_deleteBtn);

    mainLayout->addLayout(toolbar);

    // Table
    m_table = new QTableWidget();
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({
        QString::fromUtf8("المعرف"),
        QString::fromUtf8("الاسم"),
        QString::fromUtf8("الباركود"),
        QString::fromUtf8("السعر"),
        QString::fromUtf8("التكلفة"),
        QString::fromUtf8("المخزون"),
        QString::fromUtf8("التصنيف")
    });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet(
        "QTableWidget { font-size: 14px; border: 1px solid #ddd; }"
        "QTableWidget::item { padding: 8px; }"
        "QHeaderView::section { background-color: #1976D2; color: white; padding: 8px; "
        "font-size: 14px; font-weight: bold; border: none; }");
    mainLayout->addWidget(m_table);
}

void ProductsWidget::refreshTable()
{
    auto products = DatabaseManager::instance().getAllProducts();
    m_table->setRowCount(products.size());
    QString currency = Config::instance().currencySymbol();

    for (int i = 0; i < products.size(); i++) {
        const Product &p = products[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(p.id())));
        m_table->setItem(i, 1, new QTableWidgetItem(p.name()));
        m_table->setItem(i, 2, new QTableWidgetItem(p.barcode()));
        m_table->setItem(i, 3, new QTableWidgetItem(
            QString("%1 %2").arg(p.price(), 0, 'f', 2).arg(currency)));
        m_table->setItem(i, 4, new QTableWidgetItem(
            QString("%1 %2").arg(p.cost(), 0, 'f', 2).arg(currency)));
        m_table->setItem(i, 5, new QTableWidgetItem(QString::number(p.stock())));

        Category cat = DatabaseManager::instance().getCategoryById(p.categoryId());
        m_table->setItem(i, 6, new QTableWidgetItem(cat.name()));

        if (p.stock() <= 5) {
            for (int col = 0; col < 7; col++)
                m_table->item(i, col)->setBackground(QColor(255, 235, 238));
        }
    }
}

void ProductsWidget::onSearchChanged(const QString &text)
{
    if (text.isEmpty()) {
        refreshTable();
        return;
    }

    auto products = DatabaseManager::instance().searchProducts(text);
    m_table->setRowCount(products.size());
    QString currency = Config::instance().currencySymbol();

    for (int i = 0; i < products.size(); i++) {
        const Product &p = products[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(p.id())));
        m_table->setItem(i, 1, new QTableWidgetItem(p.name()));
        m_table->setItem(i, 2, new QTableWidgetItem(p.barcode()));
        m_table->setItem(i, 3, new QTableWidgetItem(
            QString("%1 %2").arg(p.price(), 0, 'f', 2).arg(currency)));
        m_table->setItem(i, 4, new QTableWidgetItem(
            QString("%1 %2").arg(p.cost(), 0, 'f', 2).arg(currency)));
        m_table->setItem(i, 5, new QTableWidgetItem(QString::number(p.stock())));
    }
}

void ProductsWidget::onAddClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("إضافة منتج جديد"));
    dialog.setLayoutDirection(Qt::RightToLeft);
    dialog.setFixedSize(450, 400);

    QFormLayout *form = new QFormLayout(&dialog);
    form->setSpacing(10);

    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("اسم المنتج:"), nameEdit);

    QLineEdit *barcodeEdit = new QLineEdit();
    barcodeEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("الباركود:"), barcodeEdit);

    QDoubleSpinBox *priceSpin = new QDoubleSpinBox();
    priceSpin->setRange(0, 999999);
    priceSpin->setDecimals(2);
    priceSpin->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("سعر البيع:"), priceSpin);

    QDoubleSpinBox *costSpin = new QDoubleSpinBox();
    costSpin->setRange(0, 999999);
    costSpin->setDecimals(2);
    costSpin->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("سعر التكلفة:"), costSpin);

    QSpinBox *stockSpin = new QSpinBox();
    stockSpin->setRange(0, 999999);
    stockSpin->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("المخزون:"), stockSpin);

    QComboBox *catCombo = new QComboBox();
    catCombo->setStyleSheet("padding: 8px; font-size: 14px;");
    catCombo->addItem(QString::fromUtf8("بدون تصنيف"), 0);
    auto categories = DatabaseManager::instance().getAllCategories();
    for (const Category &cat : categories)
        catCombo->addItem(cat.name(), cat.id());
    form->addRow(QString::fromUtf8("التصنيف:"), catCombo);

    QLineEdit *descEdit = new QLineEdit();
    descEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("الوصف:"), descEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(QString::fromUtf8("حفظ"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QString::fromUtf8("إلغاء"));
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    form->addRow(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        if (nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("خطأ"),
                                 QString::fromUtf8("يرجى إدخال اسم المنتج"));
            return;
        }

        Product product;
        product.setName(nameEdit->text().trimmed());
        product.setBarcode(barcodeEdit->text().trimmed());
        product.setPrice(priceSpin->value());
        product.setCost(costSpin->value());
        product.setStock(stockSpin->value());
        product.setCategoryId(catCombo->currentData().toInt());
        product.setDescription(descEdit->text().trimmed());

        if (DatabaseManager::instance().addProduct(product)) {
            refreshTable();
            emit productsChanged();
        } else {
            QMessageBox::critical(this, QString::fromUtf8("خطأ"),
                                  QString::fromUtf8("فشل إضافة المنتج"));
        }
    }
}

void ProductsWidget::onEditClicked()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, QString::fromUtf8("تنبيه"),
                             QString::fromUtf8("يرجى اختيار منتج للتعديل"));
        return;
    }

    int productId = m_table->item(row, 0)->text().toInt();
    Product product = DatabaseManager::instance().getProductById(productId);
    if (!product.isValid()) return;

    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("تعديل المنتج"));
    dialog.setLayoutDirection(Qt::RightToLeft);
    dialog.setFixedSize(450, 400);

    QFormLayout *form = new QFormLayout(&dialog);
    form->setSpacing(10);

    QLineEdit *nameEdit = new QLineEdit(product.name());
    nameEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("اسم المنتج:"), nameEdit);

    QLineEdit *barcodeEdit = new QLineEdit(product.barcode());
    barcodeEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("الباركود:"), barcodeEdit);

    QDoubleSpinBox *priceSpin = new QDoubleSpinBox();
    priceSpin->setRange(0, 999999);
    priceSpin->setDecimals(2);
    priceSpin->setValue(product.price());
    priceSpin->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("سعر البيع:"), priceSpin);

    QDoubleSpinBox *costSpin = new QDoubleSpinBox();
    costSpin->setRange(0, 999999);
    costSpin->setDecimals(2);
    costSpin->setValue(product.cost());
    costSpin->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("سعر التكلفة:"), costSpin);

    QSpinBox *stockSpin = new QSpinBox();
    stockSpin->setRange(0, 999999);
    stockSpin->setValue(product.stock());
    stockSpin->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("المخزون:"), stockSpin);

    QComboBox *catCombo = new QComboBox();
    catCombo->setStyleSheet("padding: 8px; font-size: 14px;");
    catCombo->addItem(QString::fromUtf8("بدون تصنيف"), 0);
    auto categories = DatabaseManager::instance().getAllCategories();
    int catIndex = 0;
    for (int i = 0; i < categories.size(); i++) {
        catCombo->addItem(categories[i].name(), categories[i].id());
        if (categories[i].id() == product.categoryId())
            catIndex = i + 1;
    }
    catCombo->setCurrentIndex(catIndex);
    form->addRow(QString::fromUtf8("التصنيف:"), catCombo);

    QLineEdit *descEdit = new QLineEdit(product.description());
    descEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("الوصف:"), descEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(QString::fromUtf8("حفظ"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QString::fromUtf8("إلغاء"));
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    form->addRow(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        product.setName(nameEdit->text().trimmed());
        product.setBarcode(barcodeEdit->text().trimmed());
        product.setPrice(priceSpin->value());
        product.setCost(costSpin->value());
        product.setStock(stockSpin->value());
        product.setCategoryId(catCombo->currentData().toInt());
        product.setDescription(descEdit->text().trimmed());

        if (DatabaseManager::instance().updateProduct(product)) {
            refreshTable();
            emit productsChanged();
        }
    }
}

void ProductsWidget::onDeleteClicked()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, QString::fromUtf8("تنبيه"),
                             QString::fromUtf8("يرجى اختيار منتج للحذف"));
        return;
    }

    QString name = m_table->item(row, 1)->text();
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, QString::fromUtf8("تأكيد الحذف"),
        QString::fromUtf8("هل تريد حذف المنتج: %1؟").arg(name),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        int id = m_table->item(row, 0)->text().toInt();
        if (DatabaseManager::instance().deleteProduct(id)) {
            refreshTable();
            emit productsChanged();
        }
    }
}
