# نقطة البيع - POS Cashier

نظام نقطة بيع متكامل باللغة العربية مبني بتقنية Qt/C++

## المميزات

- واجهة عربية كاملة مع دعم RTL
- نظام تفعيل عبر Firebase
- إدارة المستخدمين (مدير / كاشير)
- إدارة المنتجات والتصنيفات
- شاشة بيع متكاملة مع دعم الباركود
- دعم الطابعات الحرارية (ESC/POS)
- دعم طابعات GDI العادية
- سجل المبيعات والتقارير
- حساب الضريبة والخصومات
- قاعدة بيانات SQLite محلية

## المتطلبات

- Qt 6.x
- CMake 3.16+
- مترجم C++17 (MSVC, GCC, Clang)

## البناء

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## البناء في VS Code

1. تثبيت إضافة CMake Tools
2. فتح المجلد في VS Code
3. اختيار Kit المناسب (MSVC / GCC)
4. بناء المشروع عبر CMake: Build

## الاستخدام

### تسجيل الدخول الافتراضي
- المستخدم: `admin`
- كلمة المرور: `admin`

### إعداد Firebase للتفعيل

1. إنشاء مشروع في Firebase Console
2. تفعيل Realtime Database
3. إضافة مفاتيح الترخيص بالتنسيق التالي:

```json
{
  "licenses": {
    "LICENSE-KEY-HERE": {
      "activated": false,
      "expiryDate": "2027-12-31T23:59:59"
    }
  }
}
```

### إعداد الطابعة الحرارية

1. توصيل الطابعة عبر USB/Serial
2. الذهاب إلى الإعدادات
3. تفعيل "استخدام طابعة حرارية"
4. إدخال منفذ الطابعة (مثال: COM1 أو /dev/ttyUSB0)

## هيكل المشروع

```
pos-cashier/
├── CMakeLists.txt
├── README.md
├── resources/
│   └── resources.qrc
├── src/
│   ├── main.cpp
│   ├── database/
│   │   ├── databasemanager.h
│   │   └── databasemanager.cpp
│   ├── firebase/
│   │   ├── firebasemanager.h
│   │   └── firebasemanager.cpp
│   ├── models/
│   │   ├── cartitem.h/.cpp
│   │   ├── category.h/.cpp
│   │   ├── product.h/.cpp
│   │   ├── sale.h/.cpp
│   │   └── user.h/.cpp
│   ├── printing/
│   │   ├── gdiprinter.h/.cpp
│   │   ├── receiptformatter.h/.cpp
│   │   └── thermalprinter.h/.cpp
│   ├── utils/
│   │   ├── config.h
│   │   └── config.cpp
│   └── views/
│       ├── activationwidget.h/.cpp
│       ├── categorieswidget.h/.cpp
│       ├── loginwidget.h/.cpp
│       ├── mainwindow.h/.cpp
│       ├── poswidget.h/.cpp
│       ├── productswidget.h/.cpp
│       ├── reportswidget.h/.cpp
│       ├── saleshistorywidget.h/.cpp
│       ├── settingswidget.h/.cpp
│       └── userswidget.h/.cpp
```

## الترخيص

MIT License
