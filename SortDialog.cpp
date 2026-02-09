#include "SortDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

SortDialog::SortDialog(QWidget* parent) : QDialog(parent) {
    setupUi();
}

void SortDialog::setupUi() {
    setWindowTitle("Advanced sort");
    const auto mainLayout = new QVBoxLayout(this);

    const auto row = new QHBoxLayout();
    comboField = new QComboBox();
    comboField->addItems({"ID", "Surname", "Forename", "Patronymic", "Address", "Birth date", "Email"});

    comboDirection = new QComboBox();
    comboDirection->addItems({"Ascending", "Descending"});

    btnAddLevel = new QPushButton("Add Level");

    row->addWidget(new QLabel("By:"));
    row->addWidget(comboField);
    row->addWidget(comboDirection);
    row->addWidget(btnAddLevel);
    mainLayout->addLayout(row);

    listCriteria = new QListWidget();
    mainLayout->addWidget(listCriteria);

    const auto btnLayout = new QHBoxLayout();
    btnClear = new QPushButton("Clear");
    btnSort = new QPushButton("Sort");
    btnSort->setDefault(true);
    btnCancel = new QPushButton("Cancel");

    btnLayout->addWidget(btnClear);
    btnLayout->addStretch();
    btnLayout->addWidget(btnSort);
    btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    connect(btnAddLevel, &QPushButton::clicked, this, &SortDialog::onAddLevelClicked);
    connect(btnClear, &QPushButton::clicked, this, &SortDialog::onClearClicked);
    connect(btnSort, &QPushButton::clicked, this, &SortDialog::onSortClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void SortDialog::onAddLevelClicked() {
    SortField field;
    switch (comboField->currentIndex()) {
        case 0:
            field = SortField::ID;
            break;
        case 1:
            field = SortField::SURNAME;
            break;
        case 2:
            field = SortField::FORENAME;
            break;
        case 3:
            field = SortField::PATRONYMIC;
            break;
        case 4:
            field = SortField::ADDRESS;
            break;
        case 5:
            field = SortField::BIRTH_DATE;
            break;
        case 6:
            field = SortField::EMAIL;
            break;
        default:
            field = SortField::SURNAME;
            break;
    }

    const SortDirection direction = (comboDirection->currentIndex() == 0)
                                  ? SortDirection::ASCENDING
                                  : SortDirection::DESCENDING;

    criteria.push_back({field, direction});

    const QString text = QString("%1. %2 (%3)")
                   .arg(criteria.size())
                   .arg(comboField->currentText(), comboDirection->currentText());
    listCriteria->addItem(text);
}

void SortDialog::onClearClicked() {
    criteria.clear();
    listCriteria->clear();
}

void SortDialog::onSortClicked() {
    if (criteria.empty()) {
        QMessageBox::warning(this, "Warning", "Add at least one sorting level.");
        return;
    }

    accept();
}

std::vector<SortCriterion> SortDialog::getCriteria() const {
    return criteria;
}
