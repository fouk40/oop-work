#pragma once
#include "Phonebook.h"
#include <QDialog>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <vector>

class SortDialog : public QDialog {
    Q_OBJECT

public:
    explicit SortDialog(QWidget* parent = nullptr);
    std::vector<SortCriterion> getCriteria() const;

private slots:
    void onAddLevelClicked();
    void onClearClicked();
    void onSortClicked();

private:
    QComboBox* comboField;
    QComboBox* comboDirection;
    QListWidget* listCriteria;
    std::vector<SortCriterion> criteria;

    QPushButton* btnAddLevel;
    QPushButton* btnClear;
    QPushButton* btnSort;
    QPushButton* btnCancel;

    void setupUi();
};
