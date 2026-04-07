cmake_minimum_required(VERSION 3.16)
project(POSCashier VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Try Qt5 first (for Windows 7 compatibility), fall back to Qt6
find_package(Qt5 COMPONENTS Core Gui Widgets Sql Network PrintSupport)
if(Qt5_FOUND)
    set(QT_VERSION_MAJOR 5)
    message(STATUS "Using Qt5")
else()
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Network PrintSupport)
    set(QT_VERSION_MAJOR 6)
    message(STATUS "Using Qt6")
endif()

set(SOURCES
    src/main.cpp
    src/database/databasemanager.cpp
    src/database/databasemanager.h
    src/models/user.cpp
    src/models/user.h
    src/models/product.cpp
    src/models/product.h
    src/models/category.cpp
    src/models/category.h
    src/models/sale.cpp
    src/models/sale.h
    src/models/cartitem.cpp
    src/models/cartitem.h
    src/firebase/firebasemanager.cpp
    src/firebase/firebasemanager.h
    src/printing/thermalprinter.cpp
    src/printing/thermalprinter.h
    src/printing/gdiprinter.cpp
    src/printing/gdiprinter.h
    src/printing/receiptformatter.cpp
    src/printing/receiptformatter.h
    src/views/activationwidget.cpp
    src/views/activationwidget.h
    src/views/loginwidget.cpp
    src/views/loginwidget.h
    src/views/mainwindow.cpp
    src/views/mainwindow.h
    src/views/poswidget.cpp
    src/views/poswidget.h
    src/views/productswidget.cpp
    src/views/productswidget.h
    src/views/categorieswidget.cpp
    src/views/categorieswidget.h
    src/views/userswidget.cpp
    src/views/userswidget.h
    src/views/saleshistorywidget.cpp
    src/views/saleshistorywidget.h
    src/views/settingswidget.cpp
    src/views/settingswidget.h
    src/views/reportswidget.cpp
    src/views/reportswidget.h
    src/utils/config.cpp
    src/utils/config.h
    resources/resources.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::PrintSupport
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
