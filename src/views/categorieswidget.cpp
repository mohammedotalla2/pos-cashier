#include "categorieswidget.h"
#include "database/databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>

CategoriesWidget::CategoriesWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    refreshTable();
}

void CategoriesWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel(QString::fromUtf8("إدارة التصنيفات"));
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1976D2; padding: 10px;");
    mainLayout->addWidget(title);

    // Toolbar
    QHBoxLayout *toolbar = new QHBoxLayout();
    toolbar->addStretch();

    m_addBtn = new QPushButton(QString::fromUtf8("إضافة تصنيف"));
    m_addBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #45a049; }");
    connect(m_addBtn, &QPushButton::clicked, this, &CategoriesWidget::onAddClicked);
    toolbar->addWidget(m_addBtn);

    m_editBtn = new QPushButton(QString::fromUtf8("تعديل"));
    m_editBtn->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #1976D2; }");
    connect(m_editBtn, &QPushButton::clicked, this, &CategoriesWidget::onEditClicked);
    toolbar->addWidget(m_editBtn);

    m_deleteBtn = new QPushButton(QString::fromUtf8("حذف"));
    m_deleteBtn->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; padding: 10px 20px; "
        "font-size: 14px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #d32f2f; }");
    connect(m_deleteBtn, &QPushButton::clicked, this, &CategoriesWidget::onDeleteClicked);
    toolbar->addWidget(m_deleteBtn);

    mainLayout->addLayout(toolbar);

    // Table
    m_table = new QTableWidget();
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({
        QString::fromUtf8("المعرف"),
        QString::fromUtf8("الاسم"),
        QString::fromUtf8("الوصف")
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

void CategoriesWidget::refreshTable()
{
    auto categories = DatabaseManager::instance().getAllCategories();
    m_table->setRowCount(categories.size());

    for (int i = 0; i < categories.size(); i++) {
        const Category &cat = categories[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(cat.id())));
        m_table->setItem(i, 1, new QTableWidgetItem(cat.name()));
        m_table->setItem(i, 2, new QTableWidgetItem(cat.description()));
    }
}

void CategoriesWidget::onAddClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("إضافة تصنيف جديد"));
    dialog.setLayoutDirection(Qt::RightToLeft);
    dialog.setFixedSize(400, 200);

    QFormLayout *form = new QFormLayout(&dialog);

    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("اسم التصنيف:"), nameEdit);

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
                                 QString::fromUtf8("يرجى إدخال اسم التصنيف"));
            return;
        }

        Category cat(nameEdit->text().trimmed(), descEdit->text().trimmed());
        if (DatabaseManager::instance().addCategory(cat)) {
            refreshTable();
            emit categoriesChanged();
        }
    }
}

void CategoriesWidget::onEditClicked()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, QString::fromUtf8("تنبيه"),
                             QString::fromUtf8("يرجى اختيار تصنيف للتعديل"));
        return;
    }

    int catId = m_table->item(row, 0)->text().toInt();
    Category cat = DatabaseManager::instance().getCategoryById(catId);
    if (!cat.isValid()) return;

    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("تعديل التصنيف"));
    dialog.setLayoutDirection(Qt::RightToLeft);
    dialog.setFixedSize(400, 200);

    QFormLayout *form = new QFormLayout(&dialog);

    QLineEdit *nameEdit = new QLineEdit(cat.name());
    nameEdit->setStyleSheet("padding: 8px; font-size: 14px;");
    form->addRow(QString::fromUtf8("اسم التصنيف:"), nameEdit);

    QLineEdit *descEdit = new QLineEdit(cat.description());
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
        cat.setName(nameEdit->text().trimmed());
        cat.setDescription(descEdit->text().trimmed());

        if (DatabaseManager::instance().updateCategory(cat)) {
            refreshTable();
            emit categoriesChanged();
        }
    }
}

void CategoriesWidget::onDeleteClicked()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, QString::fromUtf8("تنبيه"),
                             QString::fromUtf8("يرجى اختيار تصنيف للحذف"));
        return;
    }

    QString name = m_table->item(row, 1)->text();
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, QString::fromUtf8("تأكيد الحذف"),
        QString::fromUtf8("هل تريد حذف التصنيف: %1؟").arg(name),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        int id = m_table->item(row, 0)->text().toInt();
        if (DatabaseManager::instance().deleteCategory(id)) {
            refreshTable();
            emit categoriesChanged();
        }
    }
}
