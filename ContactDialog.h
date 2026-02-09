#pragma once
#include "Contact.h"
#include "Phonebook.h"
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QCheckBox>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>


class ContactDialog : public QDialog {
    Q_OBJECT

public:
    explicit ContactDialog(Phonebook& phonebook, Contact* contactToEdit = nullptr, QWidget* parent = nullptr);

    Contact getContact() const;

private slots:
    void onAddPhoneClicked() const;
    void onDeletePhoneClicked();
    void onSaveClicked();

private:
    Phonebook& phonebook;
    Contact* contactToEdit;
    bool isEditMode;

    QLineEdit* editSurname;
    QLineEdit* editForename;
    QLineEdit* editPatronymic;
    QLineEdit* editAddress;

    QDateEdit* editBirthDate;
    QCheckBox* cbDateEnabled;

    QLineEdit* editEmail;

    QTableWidget* tablePhones;
    QPushButton* btnAddPhone;
    QPushButton* btnDeletePhone;

    QPushButton* btnSave;
    QPushButton* btnCancel;

    void loadContactData() const;
    void setupUi();
};
