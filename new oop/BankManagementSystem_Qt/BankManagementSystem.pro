QT += widgets sql

CONFIG += c++17
TEMPLATE = app
TARGET = BankManagementSystem

SOURCES += \
    main.cpp \
    database.cpp \
    account.cpp \
    savingsaccount.cpp \
    checkingaccount.cpp \
    bankmanager.cpp \
    loginwindow.cpp \
    mainwindow.cpp

HEADERS += \
    database.h \
    account.h \
    savingsaccount.h \
    checkingaccount.h \
    bankmanager.h \
    loginwindow.h \
    mainwindow.h
