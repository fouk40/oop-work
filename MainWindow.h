#pragma once
#include "ContactStorage.h"
#include "Phonebook.h"
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCloseEvent>


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Phonebook& phonebook, ContactStorage* storage, QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();

    void onSearchChanged(const QString &text) const;
    void onAdvancedSearchClicked();
    void onAdvancedSortClicked();
    void onResetClicked() const;

private:
    Phonebook& phonebook;
    ContactStorage* storage;

    QWidget* centralWidget;
    QTableWidget* tableWidget;
    QLineEdit* searchBar;
    QPushButton* btnAdd;
    QPushButton* btnEdit;
    QPushButton* btnDelete;
    QPushButton* btnAdvancedSort;
    QPushButton* btnAdvancedSearch;
    QPushButton* btnReset;

    void refreshTable(const std::vector<Contact>& contactsToShow) const;

    void setupUi();
};
