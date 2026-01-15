#include "SearchDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <vector>

SearchDialog::SearchDialog(QWidget *parent) : QDialog(parent) {
    setupUi();
}

void SearchDialog::setupUi() {
    setWindowTitle("Advanced search");
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    editId = new QLineEdit();
    editSurname = new QLineEdit();
    editForename = new QLineEdit();
    editPatronymic = new QLineEdit();
    editAddress = new QLineEdit();

    formLayout->addRow("ID:", editId);
    formLayout->addRow("Surname:", editSurname);
    formLayout->addRow("Forename:", editForename);
    formLayout->addRow("Patronymic:", editPatronymic);
    formLayout->addRow("Address:", editAddress);

    QHBoxLayout* dateFieldsLayout = new QHBoxLayout();

    editBirthDay = new QLineEdit();
    editBirthDay->setPlaceholderText("Day");

    editBirthMonth = new QLineEdit();
    editBirthMonth->setPlaceholderText("Month");

    editBirthYear = new QLineEdit();
    editBirthYear->setPlaceholderText("Year");

    dateFieldsLayout->addWidget(editBirthDay);
    dateFieldsLayout->addWidget(editBirthMonth);
    dateFieldsLayout->addWidget(editBirthYear);

    formLayout->addRow("Birth date:", dateFieldsLayout);

    editEmail = new QLineEdit();
    editPhone = new QLineEdit();

    formLayout->addRow("Email:", editEmail);
    formLayout->addRow("Phone number:", editPhone);

    mainLayout->addLayout(formLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* btnSearch = new QPushButton("Search");
    btnSearch->setDefault(true);
    QPushButton* btnClear = new QPushButton("Clear fields");
    QPushButton* btnCancel = new QPushButton("Cancel");

    btnLayout->addWidget(btnClear);
    btnLayout->addStretch();
    btnLayout->addWidget(btnSearch);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    connect(btnSearch, &QPushButton::clicked, this, &SearchDialog::onSearchClicked);
    connect(btnClear, &QPushButton::clicked, this, &SearchDialog::onClearClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void SearchDialog::onSearchClicked() {
    criteria.clear();

    const std::vector<std::pair<QLineEdit*, SearchField>> fields = {
        {editId,         SearchField::ID},
        {editSurname,    SearchField::SURNAME},
        {editForename,   SearchField::FORENAME},
        {editPatronymic, SearchField::PATRONYMIC},
        {editAddress,    SearchField::ADDRESS},
        {editBirthDay,   SearchField::BIRTH_DAY},
        {editBirthMonth, SearchField::BIRTH_MONTH},
        {editBirthYear,  SearchField::BIRTH_YEAR},
        {editEmail,      SearchField::EMAIL},
        {editPhone,      SearchField::PHONE},
    };

    for (const auto& pair : fields) {
        const QLineEdit* edit = pair.first;
        SearchField field = pair.second;

        QString text = edit->text().trimmed();
        if (!text.isEmpty()) {
            criteria[field] = text.toStdString();
        }
    }

    accept();
}

void SearchDialog::onClearClicked() const {
    editId->clear();
    editSurname->clear();
    editForename->clear();
    editPatronymic->clear();
    editAddress->clear();
    editBirthDay->clear();
    editBirthMonth->clear();
    editBirthYear->clear();
    editEmail->clear();
    editPhone->clear();
}

std::map<SearchField, std::string> SearchDialog::getCriteria() const {
    return criteria;
}
