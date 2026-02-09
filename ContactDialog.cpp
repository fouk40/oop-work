#include "ContactDialog.h"
#include "validation.h"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>

ContactDialog::ContactDialog(Phonebook& phonebook, Contact* contactToEdit, QWidget* parent)
    : QDialog(parent), phonebook(phonebook), contactToEdit(contactToEdit) {
    isEditMode = contactToEdit != nullptr;
    setupUi();

    if (isEditMode) {
        setWindowTitle("Edit contact");
        loadContactData();
    } else {
        setWindowTitle("Add contact");
    }
}

void ContactDialog::setupUi() {
    const auto mainLayout = new QVBoxLayout(this);

    QLabel* infoLabel = new QLabel("Fields marked with an asterisk (*) are mandatory.");
    mainLayout->addWidget(infoLabel);

    QFormLayout* formLayout = new QFormLayout();

    editSurname = new QLineEdit();
    editForename = new QLineEdit();
    editPatronymic = new QLineEdit();
    editAddress = new QLineEdit();

    formLayout->addRow("Surname*:", editSurname);
    formLayout->addRow("Forename*:", editForename);
    formLayout->addRow("Patronymic:", editPatronymic);
    formLayout->addRow("Address:", editAddress);

    QHBoxLayout* dateLayout = new QHBoxLayout();

    editBirthDate = new QDateEdit();
    editBirthDate->setDisplayFormat("dd.MM.yyyy");
    editBirthDate->setCalendarPopup(true);

    editBirthDate->setMaximumDate(QDate::currentDate());
    editBirthDate->setMinimumDate(QDate(1900, 1, 1));

    editBirthDate->setDate(QDate::currentDate());
    editBirthDate->setEnabled(false);

    cbDateEnabled = new QCheckBox("Set date");
    cbDateEnabled->setChecked(false);

    dateLayout->addWidget(editBirthDate);
    dateLayout->addWidget(cbDateEnabled);

    dateLayout->addStretch();

    formLayout->addRow("Birth date:", dateLayout);

    editEmail = new QLineEdit();
    formLayout->addRow("Email*:", editEmail);

    mainLayout->addLayout(formLayout);

    mainLayout->addWidget(new QLabel("Phone numbers* (at least one):"));
    tablePhones = new QTableWidget();
    tablePhones->setColumnCount(2);
    tablePhones->setHorizontalHeaderLabels({"Type", "Number"});
    tablePhones->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablePhones->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablePhones->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(tablePhones);

    const auto phoneBtnLayout = new QHBoxLayout();
    btnAddPhone = new QPushButton("Add phone");
    btnDeletePhone = new QPushButton("Delete selected");
    phoneBtnLayout->addWidget(btnAddPhone);
    phoneBtnLayout->addWidget(btnDeletePhone);
    mainLayout->addLayout(phoneBtnLayout);

    mainLayout->addStretch();
    const auto btnLayout = new QHBoxLayout();
    btnSave = new QPushButton("Save");
    btnSave->setDefault(true);
    btnCancel = new QPushButton("Cancel");

    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    connect(btnAddPhone, &QPushButton::clicked, this, &ContactDialog::onAddPhoneClicked);
    connect(btnDeletePhone, &QPushButton::clicked, this, &ContactDialog::onDeletePhoneClicked);
    connect(btnSave, &QPushButton::clicked, this, &ContactDialog::onSaveClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    connect(cbDateEnabled, &QCheckBox::toggled, editBirthDate, &QWidget::setEnabled);
}

void ContactDialog::onAddPhoneClicked() const {
    const int row = tablePhones->rowCount();
    tablePhones->insertRow(row);
    tablePhones->setItem(row, 0, new QTableWidgetItem(""));
    tablePhones->setItem(row, 1, new QTableWidgetItem(""));
}

void ContactDialog::onDeletePhoneClicked() {
    const int currentRow = tablePhones->currentRow();

    if (currentRow != -1) {
        tablePhones->removeRow(currentRow);
    } else {
        QMessageBox::warning(this, "Error", "Select a number to delete.");
    }
}

void ContactDialog::loadContactData() const {
    editSurname->setText(QString::fromStdString(contactToEdit->getSurname()));
    editForename->setText(QString::fromStdString(contactToEdit->getForename()));
    editPatronymic->setText(QString::fromStdString(contactToEdit->getPatronymic()));
    editAddress->setText(QString::fromStdString(contactToEdit->getAddress()));

    const Date birthDate = contactToEdit->getBirthDate();
    if (birthDate.day != 0) {
        cbDateEnabled->setChecked(true);
        editBirthDate->setDate(QDate(birthDate.year, birthDate.month, birthDate.day));
    }

    editEmail->setText(QString::fromStdString(contactToEdit->getEmail()));

    for (const auto& phone : contactToEdit->getPhoneNumbers()) {
        const int row = tablePhones->rowCount();
        tablePhones->insertRow(row);
        tablePhones->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(phone.type)));
        tablePhones->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(phone.number)));
    }
}

void ContactDialog::onSaveClicked() {
    const std::string surname = validation::trim(editSurname->text().toStdString());
    if (!validation::isValidName(surname)) {
        QMessageBox::warning(this, "Error", "Invalid surname.");
        editSurname->setFocus();
        return;
    }

    const std::string forename = validation::trim(editForename->text().toStdString());
    if (!validation::isValidName(forename)) {
        QMessageBox::warning(this, "Error", "Invalid forename.");
        editForename->setFocus();
        return;
    }

    const std::string patronymic = validation::trim(editPatronymic->text().toStdString());
    if (!patronymic.empty() && !validation::isValidName(patronymic)) {
        QMessageBox::warning(this, "Error", "Invalid patronymic.");
        editPatronymic->setFocus();
        return;
    }

    const std::string address = validation::trim(editAddress->text().toStdString());
    if (!validation::isValidAddress(address)) {
        QMessageBox::warning(this, "Error", "Invalid address.");
        editAddress->setFocus();
        return;
    }

    const std::string email = validation::normalizeEmail(editEmail->text().toStdString());
    if (!validation::isValidEmail(email)) {
        QMessageBox::warning(this, "Error", "Invalid email.");
        editEmail->setFocus();
        return;
    }

    if (!validation::isForenameInEmail(email, forename)) {
        QMessageBox::warning(this, "Error",
                             "Email must contain the forename '" + QString::fromStdString(forename) + "'.\n"
                             "Update email or forename.");
        editForename->setFocus();
        return;
    }

    const int ignoreId = isEditMode ? contactToEdit->getId() : 0;

    if (!phonebook.isEmailUnique(email, ignoreId)) {
        QMessageBox::warning(this, "Error", "Email already in use.");
        editEmail->setFocus();
        return;
    }

    if (tablePhones->rowCount() == 0) {
        QMessageBox::warning(this, "Error", "Add at least one phone number.");
        btnAddPhone->setFocus();
        return;
    }

    std::vector<std::string> currentPhones;

    for (int i = 0; i < tablePhones->rowCount(); ++i) {
        std::string type = validation::trim(tablePhones->item(i, 0)->text().toStdString());
        std::string number = validation::trim(tablePhones->item(i, 1)->text().toStdString());

        if (!validation::isValidPhoneType(type)) {
            QMessageBox::warning(this, "Error", "Invalid phone type in row " + QString::number(i+1));
            tablePhones->setCurrentCell(i, 0);
            return;
        }

        if (!validation::isValidPhoneNumber(number)) {
            QMessageBox::warning(this, "Error", "Invalid phone number in row " + QString::number(i+1));
            tablePhones->setCurrentCell(i, 1);
            return;
        }

        std::string normalizedNumber = validation::normalizePhoneNumber(number);
        if (!phonebook.isPhoneNumberUnique(number, ignoreId)) {
            QMessageBox::warning(this, "Error", "Phone number in row " + QString::number(i+1) + " is already in use by other contact.");
            tablePhones->setCurrentCell(i, 1);
            return;
        }

        for (const auto& existing : currentPhones) {
            if (existing == normalizedNumber) {
                QMessageBox::warning(this, "Error", "Duplicate phone number inside this contact.");
                tablePhones->setCurrentCell(i, 1);
                return;
            }
        }
        currentPhones.push_back(normalizedNumber);
    }

    accept();
}

Contact ContactDialog::getContact() const {
    Contact contact;
    contact.setSurname(editSurname->text().toStdString());
    contact.setForename(editForename->text().toStdString());
    contact.setPatronymic(editPatronymic->text().toStdString());
    contact.setAddress(editAddress->text().toStdString());

    if (cbDateEnabled->isChecked()) {
        const QDate date = editBirthDate->date();
        contact.setBirthDate(Date(date.day(), date.month(), date.year()));
    } else {
        contact.setBirthDate(Date());
    }

    contact.setEmail(editEmail->text().toStdString());

    for (int i = 0; i < tablePhones->rowCount(); ++i) {
        contact.addPhoneNumber(
            tablePhones->item(i, 0)->text().toStdString(),
            tablePhones->item(i, 1)->text().toStdString()
        );
    }

    return contact;
}
