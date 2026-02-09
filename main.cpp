#include "cli.h"
#include "DbStorage.h"
#include "FileStorage.h"
#include "Phonebook.h"

#include <QApplication>
#include <QMessageBox>
#include "MainWindow.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
#endif

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    char interfaceMode;
    while (true) {
        std::cout << "Select interface mode:" << std::endl;
        std::cout << "1. Console mode" << std::endl;
        std::cout << "2. Graphical mode" << std::endl;

        interfaceMode = cli::getInput<char>("Your choice: ");

        if (interfaceMode == '1' || interfaceMode == '2') {
            break;
        }
        std::cout << "Invalid choice." << std::endl;
    }

    char storageMode;
    while (true) {
        std::cout << "\nSelect storage mode:" << std::endl;
        std::cout << "1. File storage (contacts.txt)" << std::endl;
        std::cout << "2. Database storage (PostgreSQL)" << std::endl;

        storageMode = cli::getInput<char>("Your choice: ");

        if (storageMode == '1' || storageMode == '2') {
            break;
        }
        std::cout << "Invalid choice." << std::endl;
    }

    ContactStorage* storage = nullptr;

    if (storageMode == '2') {
        const auto db = new DbStorage("localhost", 5432, "phonebook_db","postgres", "password");

        std::cout << "Connecting to database...\n" << std::endl;
        if (db->init()) {
            storage = db;
        } else {
            const std::string errorText = db->getLastError() + "\n\nFalling back to file storage.";

            if (interfaceMode == '2') {
                QMessageBox::critical(nullptr, "Storage error", QString::fromStdString(errorText));
            } else {
                std::cerr << errorText << std::endl;
            }

            delete db;
            storage = new FileStorage("contacts.txt");
        }
    } else {
        storage = new FileStorage("contacts.txt");
        std::cout << "Using file storage." << std::endl;
    }

    Phonebook phonebook;

    std::cout << "\nLoading contacts..." << std::endl;
    const std::vector<Contact> loadedContacts = storage->load();

    if (!storage->getLastError().empty()) {
        const std::string errorText = storage->getLastError();

        if (interfaceMode == '2') {
            QMessageBox::critical(nullptr, "Storage error", QString::fromStdString(errorText));
        } else {
            std::cerr << errorText << std::endl;
        }
    }

    for (const auto& contact : loadedContacts) {
        phonebook.addContactFromStorage(contact);
    }
    phonebook.initializeNextId();
    std::cout << "Loaded " << loadedContacts.size() << " contact(s)." << std::endl;

    int exitCode = 0;

    if (interfaceMode == '1') {
        bool running = true;
        while (running) {
            cli::displayMenu();

            const char menuChoice = cli::getInput<char>("Your choice: ");
            switch (menuChoice) {
                case '1': {
                    cli::printAllContacts(phonebook);
                    break;
                }
                case '2': {
                    cli::searchContacts(phonebook);
                    break;
                }
                case '3': {
                    cli::addContact(phonebook);
                    break;
                }
                case '4': {
                    cli::editContact(phonebook);
                    break;
                }
                case '5': {
                    cli::deleteContact(phonebook);
                    break;
                }
                case '6': {
                    cli::sortContacts(phonebook);
                    break;
                }
                case '0': {
                    running = false;
                    break;
                }
                default: {
                    std::cout << "Invalid input." << std::endl;
                    break;
                }
            }
        }

        const char saveChoice = cli::getInput<char>("\nSave changes? (Y/n): ");
        while (true) {
            if (saveChoice == 'N' || saveChoice == 'n') {
                std::cout << "Changes discarded." << std::endl;
                break;
            }

            if (saveChoice == 'Y' || saveChoice == 'y') {
                std::cout << "Saving contacts..." << std::endl;
                if (storage->save(phonebook.getAllContacts())) {
                    std::cout << "Contacts saved." << std::endl;
                } else {
                    std::cout << "Save error: " << storage->getLastError() << std::endl;
                }
                break;
            }
            std::cout << "Invalid input." << std::endl;
        }
    } else {
        MainWindow w(phonebook, storage);
        w.show();
        exitCode = app.exec();
    }

    delete storage;
    return exitCode;
}
