QT += core gui sql widgets

TEMPLATE = app

TARGET = phonebook

CONFIG += c++20

CONFIG += console

SOURCES += \
    ContactDialog.cpp \
    SearchDialog.cpp \
    SortDialog.cpp \
    main.cpp \
    Phonebook.cpp \
    Contact.cpp \
    FileStorage.cpp \
    validation.cpp \
    cli.cpp \
    MainWindow.cpp \
    DbStorage.cpp

HEADERS += \
    ContactDialog.h \
    Phonebook.h \
    Contact.h \
    Date.h \
    FileStorage.h \
    SearchDialog.h \
    SortDialog.h \
    validation.h \
    cli.h \
    MainWindow.h \
    DbStorage.h \
    ContactStorage.h

DISTFILES += \
    contacts.txt
