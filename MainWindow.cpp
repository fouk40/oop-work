#include "MainWindow.h"
#include "ContactDialog.h"
#include "FileStorage.h"
#include "SearchDialog.h"
#include "SortDialog.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

MainWindow::MainWindow(Phonebook& phonebook, ContactStorage* storage, QWidget *parent)
    : QMainWindow(parent), phonebook(phonebook), storage(storage) {
    setupUi();
    refreshTable(phonebook.getAllContacts());
}

void MainWindow::setupUi() {
    setWindowTitle("Phonebook");
    resize(1000, 600);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout* topBarLayout = new QHBoxLayout();

    QLabel* searchLabel = new QLabel("Search:", this);
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search by all fields simultaneously...");
    searchBar->setClearButtonEnabled(true);

    btnAdvancedSearch = new QPushButton("Advanced search", this);
    btnAdvancedSort = new QPushButton("Advanced sort", this);
    btnReset = new QPushButton("Reset view", this);

    topBarLayout->addWidget(searchLabel);
    topBarLayout->addWidget(searchBar);
    topBarLayout->addWidget(btnAdvancedSearch);
    topBarLayout->addWidget(btnAdvancedSort);
    topBarLayout->addWidget(btnReset);

    tableWidget = new QTableWidget(this);

    const QStringList headers = {"ID", "Surname", "Forename", "Patronymic", "Address", "Birth date",
        "Email", "Phone numbers"};
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    tableWidget->setSortingEnabled(true);

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnAdd = new QPushButton("Add contact", this);
    btnEdit = new QPushButton("Edit contact", this);
    btnDelete = new QPushButton("Delete contact", this);

    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDelete);

    mainLayout->addLayout(topBarLayout);
    mainLayout->addWidget(tableWidget);
    mainLayout->addLayout(btnLayout);

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onEditClicked);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);

    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::onEditClicked);
    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);
    connect(btnAdvancedSearch, &QPushButton::clicked, this, &MainWindow::onAdvancedSearchClicked);
    connect(btnAdvancedSort, &QPushButton::clicked, this, &MainWindow::onAdvancedSortClicked);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onResetClicked);
}

void MainWindow::refreshTable(const std::vector<Contact>& contactsToShow) const {
    const bool wasSortingEnabled = tableWidget->isSortingEnabled();
    tableWidget->setSortingEnabled(false);
    tableWidget->setRowCount(0);

    for (const auto& contact : contactsToShow) {
        const int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        QTableWidgetItem* idItem = new QTableWidgetItem();
        idItem->setData(Qt::DisplayRole, contact.getId());
        tableWidget->setItem(row, 0, idItem);

        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(contact.getSurname())));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(contact.getForename())));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(contact.getPatronymic())));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(contact.getAddress())));

        const Date birthDate = contact.getBirthDate();
        QString dateStr;

        if (birthDate.day != 0) {
            dateStr = QString("%1.%2.%3")
                        .arg(birthDate.year, 4, 10, QChar('0'))
                        .arg(birthDate.month, 2, 10, QChar('0'))
                        .arg(birthDate.day, 2, 10, QChar('0'));
        }

        tableWidget->setItem(row, 5, new QTableWidgetItem(dateStr));

        tableWidget->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(contact.getEmail())));

        QString phonesStr;
        const auto& numbers = contact.getPhoneNumbers();
        for (size_t i = 0; i < numbers.size(); ++i) {
            phonesStr += QString::fromStdString(numbers[i].type) + ": " +
                         QString::fromStdString(numbers[i].number);
            if (i < numbers.size() - 1) {
                phonesStr += ", ";
            }
        }
        tableWidget->setItem(row, 7, new QTableWidgetItem(phonesStr));
    }

    tableWidget->setSortingEnabled(wasSortingEnabled);
}

void MainWindow::onAddClicked() {
    ContactDialog dialog(phonebook, nullptr, this);

    if (dialog.exec() == QDialog::Accepted) {
        Contact newContact = dialog.getContact();
        phonebook.addContact(newContact);
        refreshTable(phonebook.getAllContacts());
    }
}

void MainWindow::onEditClicked() {
    auto selectedItems = tableWidget->selectedItems();
    if (selectedItems.empty()) {
        QMessageBox::warning(this, "Warning", "Select a contact to edit.");
        return;
    }

    const int row = selectedItems[0]->row();
    const int id = tableWidget->item(row, 0)->text().toInt();

    Contact* contactPtr = phonebook.findContact(id);

    ContactDialog dialog(phonebook, contactPtr, this);

    if (dialog.exec() == QDialog::Accepted) {
        const Contact updatedContact = dialog.getContact();

        contactPtr->setSurname(updatedContact.getSurname());
        contactPtr->setForename(updatedContact.getForename());
        contactPtr->setPatronymic(updatedContact.getPatronymic());
        contactPtr->setAddress(updatedContact.getAddress());
        contactPtr->setBirthDate(updatedContact.getBirthDate());
        contactPtr->setEmail(updatedContact.getEmail());

        contactPtr->clearPhoneNumbers();

        for (const auto& phone : updatedContact.getPhoneNumbers()) {
            contactPtr->addPhoneNumber(phone.type, phone.number);
        }

        refreshTable(phonebook.getAllContacts());
    }
}

void MainWindow::onDeleteClicked() {
    auto selectedItems = tableWidget->selectedItems();
    if (selectedItems.empty()) {
        QMessageBox::warning(this, "Warning", "Select a contact to delete.");
        return;
    }

    const int row = selectedItems[0]->row();
    const int id = tableWidget->item(row, 0)->text().toInt();

    const auto reply = QMessageBox::question(this, "Confirm delete",
                                     "Are you sure you want to delete this contact?",
                                     QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        phonebook.deleteContact(id);
        refreshTable(phonebook.getAllContacts());
    }
}

void MainWindow::onSearchChanged(const QString &text) const {
    refreshTable(phonebook.searchAllFields(text.toStdString()));
}

void MainWindow::onAdvancedSortClicked() {
    SortDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        const auto criteria = dialog.getCriteria();

        if (criteria.empty()) {
            return;
        }

        phonebook.sortContacts(criteria);
        tableWidget->setSortingEnabled(false);

        refreshTable(phonebook.getAllContacts());
    }
}

void MainWindow::onAdvancedSearchClicked() {
    SearchDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        const auto criteria = dialog.getCriteria();
        const std::vector<Contact> results = phonebook.searchContacts(criteria);
        searchBar->clear();
        refreshTable(results);
    }
}

void MainWindow::onResetClicked() const {
    searchBar->clear();

    tableWidget->setSortingEnabled(false);
    tableWidget->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);

    std::vector<SortCriterion> defaultCriteria;
    defaultCriteria.push_back({SortField::ID, SortDirection::ASCENDING});
    phonebook.sortContacts(defaultCriteria);

    refreshTable(phonebook.getAllContacts());

    tableWidget->setSortingEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    const auto reply = QMessageBox::question(this, "Exit", "Do you want to save changes before exit?",
                                             QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
        event->ignore();
        return;
    }

    if (reply == QMessageBox::Yes) {
        const bool isFiltered = tableWidget->rowCount() != static_cast<int>(phonebook.getAllContacts().size());

        if (isFiltered) {
            if (!searchBar->text().isEmpty()) {
                searchBar->blockSignals(true);
                searchBar->clear();
                searchBar->blockSignals(false);
            }

            refreshTable(phonebook.getAllContacts());
        }

        std::vector<int> orderedIds;
        for (int row = 0; row < tableWidget->rowCount(); ++row) {
            int id = tableWidget->item(row, 0)->text().toInt();
            orderedIds.push_back(id);
        }
        phonebook.reorderContacts(orderedIds);

        if (storage->save(phonebook.getAllContacts())) {
            event->accept();
        } else {
            const QString msg = "Failed to save contacts.\n" +  QString::fromStdString(storage->getLastError());
            QMessageBox::critical(this, "Save error", msg);
            event->ignore();
        }
    } else {
        event->accept();
    }
}
