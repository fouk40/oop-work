#pragma once
#include "Phonebook.h"
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <map>

class SearchDialog : public QDialog {
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);

    std::map<SearchField, std::string> getCriteria() const;

private slots:
    void onSearchClicked();
    void onClearClicked() const;

private:
    QLineEdit* editId;
    QLineEdit* editSurname;
    QLineEdit* editForename;
    QLineEdit* editPatronymic;
    QLineEdit* editAddress;
    QLineEdit* editBirthDay;
    QLineEdit* editBirthMonth;
    QLineEdit* editBirthYear;
    QLineEdit* editEmail;
    QLineEdit* editPhone;

    std::map<SearchField, std::string> criteria;

    void setupUi();
};
