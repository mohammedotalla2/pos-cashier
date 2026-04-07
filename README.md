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

- Qt 5.x أو Qt 6.x (يدعم كلا الإصدارين)
- CMake 3.16+
- مترجم C++17 (MSVC, GCC, Clang)
- متوافق مع Windows 7 والأحدث (عند استخدام Qt 5)

## البناء

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

يكتشف CMake تلقائياً إصدار Qt المثبت (يفضل Qt5 إن وُجد).

## البناء في VS Code

1. تثبيت إضافة CMake Tools
2. فتح المجلد في VS Code
3. اختيار Kit المناسب (MSVC / GCC)
4. التأكد من أن مسار Qt مضاف في PATH أو CMAKE_PREFIX_PATH
5. بناء المشروع عبر CMake: Build

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
